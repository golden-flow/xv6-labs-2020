#include "kernel/types.h"
#include "user/user.h"

int
main (int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(1, "Usage: sleep num-of-ticks\n");
    exit(1);
  } 
  sleep(atoi(argv[1]));
  exit(0);
}
