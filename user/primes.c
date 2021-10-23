#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
pipeline(int fd)
{
  int base;
  int next = 0;
  int p[2];
  read(fd, &base, 4);
  
  printf("prime %d\n", base);
  if (read(fd, &next, 4) == 4) {
    pipe(p);
    if (fork() > 0) { // parent process 
      write(p[1], &next, 4);
      while (read(fd, &next, 4) == 4) {
        if (next % base) {
          write(p[1], &next, 4);
        }
      }
      close(p[1]);
      wait((int*)0);
      close(p[0]);
      exit(0);
    }
    else { // child process
      close(p[1]);
      pipeline(p[0]);
      close(p[0]);
      exit(0);
    }
  }
}

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  if (fork() > 0) { // parent process 
    int i;
    for (i = 2; i <= 35; i++) {
      write(p[1], &i, 4);
    }
    close(p[1]);
    wait((int*)0);
    close(p[0]);
    exit(0);
  }
  else { // child process
    close(p[1]);
    pipeline(p[0]);
    close(p[0]);
    exit(0);
  }
}
