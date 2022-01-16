#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

int
endswith(const char* str, const char* substr) {
  const char *p, *q;

  if (strlen(str) < strlen(substr)) {
    return 0;
  }
  p = str + strlen(str) - strlen(substr);
  q = substr;
  while (*p) {
    if (*p++ != *q++) return 0;
  }
  return 1;
}

void
checkdir(const char* path, const char* name) {
  int fd;
  char buf[512];
  char *p;
  struct stat st;
  struct dirent de;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot fstat %s\n", path);
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';
  switch (st.type) {
  case T_FILE:
    if (endswith(path, name)) {
      fprintf(1, "%s\n", path);
    }
    break;
  case T_DIR:
    while (read(fd, &de, sizeof(de)) > 0) {
      if (!strcmp(de.name, ".") || !strcmp(de.name, "..") || de.inum == 0) continue;
      if (strlen(de.name) + 1 + p - buf > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
      }
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = '\0';
      checkdir(buf, name);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(1, "Usage: find dir filename");
    exit(1);
  }
  checkdir(argv[1], argv[2]);
  exit(0);
}
