#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_sigalarm(void)
{
  int ticks;
  uint64 handler;

  argint(0, &ticks);
  argaddr(1, (uint64*)&handler);
  struct proc* p = myproc();
  p->interval = ticks;
  p->handler = handler;
  p->ticks_since_last = 0;
  printf("[%d %p]\n", ticks, handler);

  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc* p = myproc();
  *p->trapframe = p->savedstate;
  p->ticks_since_last = 0;
  return 0;
}
