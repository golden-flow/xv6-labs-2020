#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"

const char*
filename(const char* path)
{
  const char* p = path;
  const char* ret = 0;
  while (*p) {
    if (*p == '/') ret = 1 + p;
    p++;
  }
  return ret;
}

void
find(const char* path, const char* pattern)
{
  int fd;
  char buf[512];
  char* p;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot fstat %s\n", path);
    return;
  }

  switch (st.type) {
    case T_FILE:
      if (!strcmp(filename(path), pattern)) {
        printf("%s\n", path);
      }
      break;
    
    case T_DIR:
      if (strlen(path) + 1 + DIRSIZ + 1 < strlen(buf)) {
        printf("find: path to long\n");
        return;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;
        if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = '\0';
        find(buf, pattern);
      }
      break;
    
    default:
      break;
  }

  close(fd);
}

int
main(int argc, char* argv[])
{
  if (argc != 3) {
    printf("Usage: find path pattern\n");
    exit(0);
  }
  else {
    find(argv[1], argv[2]);
  }
  exit(0);
}
