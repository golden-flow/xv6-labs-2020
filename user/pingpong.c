#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p[2];
  char msg[2];
  pipe(p);
  if (fork() == 0) { // child process 
    int pid = getpid();
    read(p[0], msg, 1);
    printf("%d: received ping\n", pid);
    write(p[1], "t", 1);
    exit(0);
  }
  else { // parent process
    int pid = getpid();
    write(p[1], "t", 1);
    wait((int*)0);
    read(p[0], msg, 1);
    printf("%d: received pong\n", pid);
    close(p[0]);
    close(p[1]);
    exit(0);
  }
}
