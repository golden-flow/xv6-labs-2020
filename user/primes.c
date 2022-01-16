#include "kernel/types.h"
#include "user/user.h"

/**
 * @brief Read a number from the file descriptor `fd`, create a new 
 * process, and send the remaining numbers that cannot be divided by
 * the first one from `fd` to the new process through a pipe. The new 
 * process does this recursively.
 * 
 * It is assumed that at least one number can be read from `fd`,
 * which is a prime number.
 * 
 * `fd` is not closed.
 * 
 * @param fd the file descriptor for reading numbers
 */
void
process(int fd) {
  int prime;
  int next;
  int p[2];

  read(fd, &prime, 4);
  fprintf(1, "prime %d\n", prime);

  if (read(fd, &next, 4) == 0) {
    // no subsequent numbers
    return;
  }

  pipe(p);
  if (fork() == 0) { // child
    close(p[1]);
    process(p[0]);
    close(p[0]);
    exit(0);
  } else { // parent
    close(p[0]);
    do {
      if (next % prime) {
        write(p[1], &next, 4);
      }
    } while (read(fd, &next, 4) == 4);
    close(p[1]);
    wait(0);
  }
}

int
main(int argc, char* argv[]) {
  int p[2];

  pipe(p);
  if (fork() == 0) { // child
    close(p[1]);
    process(p[0]);
    close(p[0]);
  } else { // parent
    int i;

    close(p[0]);
    for (i = 2; i <= 35; i++) {
      write(p[1], &i, 4);
    }
    close(p[1]);
    wait(0);
  }
  exit(0);
}
