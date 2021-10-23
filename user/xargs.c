#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define FINISH 0
#define READING 1

int
main(int argc, char* argv[])
{
  int i;
  int nargc;
  int status;
  int retval;
  char ch;
  char* nargv[MAXARG];
  char* p = 0;
  nargc = argc - 1;
  for (i = 0; i < argc; i++) {
    nargv[i] = argv[i + 1];
  }
  status = FINISH;
  while (1) {
    retval = read(0, &ch, 1);
    if (ch == '\n' || retval == 0) {
      if (status == READING) {
        if (fork() == 0) { // child process
          exec(nargv[0], nargv);
          exit(0);
        }
        else {
          wait(0);
        }
        while (nargc != argc - 1) {
          free(nargv[--nargc]);
        }
        status = FINISH;
      }
      if (retval == 0) break;
    }
    else if (ch == ' ') {
      switch (status) {
        case FINISH:
          break;
        case READING:
          status = FINISH;
          p = 0;
          break;
      }
    }
    else {
      switch (status) {
        case FINISH:
          if (nargc == MAXARG) {
            fprintf(2, "xargs: too many arguments\n");
            exit(1);
          }
          nargv[nargc++] = malloc(256);
          p = nargv[nargc - 1];
          status = READING;
          // fall to READING
        case READING:
          if (p - nargv[nargc - 1] >= 255) {
            fprintf(2, "xargs: argument %d is too long\n", nargc - 1);
            exit(1);
          }
          *p++ = ch;
          *p = '\0';
          break;
      }
    }
  }
  exit(0);
}
