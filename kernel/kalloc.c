// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "vm.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

extern int refcount[]; // defined in vm.c
extern struct spinlock refcount_lock;

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&refcount_lock, "refcount");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
// Must hold refcount_lock when calling this.
void
kfree(void *pa)
{
  int rfcount;

  rfcount = REFCOUNT((uint64)pa);

  if (rfcount > 0) return;

  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

ushort flags[(PHYSTOP - KERNBASE) / PGSIZE];
ushort* getflag(uint64 addr) {
  return &flags[(addr - KERNBASE) / PGSIZE];
}

// Return the number of bytes of free memory
int
freememcount()
{
  int count;
  struct run *r;

  count = 0;
  memset(flags, 0, sizeof(flags));
  r = kmem.freelist;
  while (r) {
    count += 1;
    *getflag((uint64)r) = 1;
    r = r->next;
  }
  printf("These physical pages are not free after the kernel:\n");
  int cnt = 0;
  for (uint64 i = PGROUNDUP((uint64)end); i < PHYSTOP; i += PGSIZE) {
    if (*getflag(i) == 0) {
      printf("%p ", i);
      if (cnt % 8 == 7) {
        printf("\n");
      }
      cnt++;
    }
  }
  printf("\n");
  return count;
}
