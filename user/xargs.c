#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAXLEN 512

int
main(int argc, char* argv[]) {
  int nargc;
  int do_break;
  char* nargv[MAXARG+1];
  char* p;
  char ch;
  int i;

  nargc = argc - 1;
  for (i = 1; i < argc; i++) {
    nargv[i - 1] = malloc(MAXLEN);
    strcpy(nargv[i - 1], argv[i]);
  }
  nargv[nargc] = malloc(MAXLEN);
  p = nargv[nargc];

  do_break = 0;
  while (read(0, &ch, 1) == 1) {
    switch (ch) {
    case ' ':
      *p = '\0';
      nargv[++nargc] = malloc(MAXLEN);
      p = nargv[nargc];
      break;

    case '\n':
      *p = '\0';
      nargv[++nargc] = 0;
      if ((fork() == 0)) { // child
        exec(nargv[0], nargv);
        fprintf(2, "xargs: exec %s failed. Arguments are: ", nargv[0]);
        int j;
        for (j = 0; nargv[j] != 0; j++) {
          fprintf(2, "%s ", nargv[j]);
        }
        fprintf(2, "\n");

        exit(1);
      } else { // parent
        wait(0);
        for (i = argc - 1; i < nargc; i++) {
          free(nargv[i]);
        }
        nargc = argc - 1;
        nargv[nargc] = malloc(MAXLEN);
        p = nargv[nargc];
      }
      break;

    default:
      *p++ = ch;
      break;
    }

    if (do_break) break;
  }
  exit(0);
}
