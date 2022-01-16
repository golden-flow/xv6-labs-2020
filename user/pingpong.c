#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
  int p[2];
  char byte;

  pipe(p);
  if (fork() == 0) { // child
    read(p[0], &byte, 1);
    close(p[0]);
    fprintf(1, "%d: received ping\n", getpid());
    write(p[1], &byte, 1);
    close(p[1]);
  } else { // parent
    write(p[1], "a", 1);
    close(p[1]);
    wait((int*)0);
    read(p[0], &byte, 1);
    close(p[0]);
    fprintf(1, "%d: received pong\n", getpid());
  }
  exit(0);
}
