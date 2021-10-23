#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Usage: sleep number-of-ticks\n");
    exit(1);
  }
  else {
    int numticks = atoi(argv[1]);
    sleep(numticks);
  }
  exit(0);
}