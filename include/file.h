static void * read_file(const char * filename)
{
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "open(%s): %s\n", filename, strerror(errno));
    return NULL;
  }

  off_t size = lseek(fd, 0, SEEK_END);
  if (size == (off_t)-1) {
    fprintf(stderr, "lseek(%s, SEEK_END): %s\n", filename, strerror(errno));
    return NULL;
  }

  off_t start = lseek(fd, 0, SEEK_SET);
  if (start == (off_t)-1) {
    fprintf(stderr, "lseek(%s, SEEK_SET): %s\n", filename, strerror(errno));
    return NULL;
  }

  void * buf = malloc(size+1);

  ssize_t read_size = read(fd, buf, size);
  if (read_size == -1) {
    fprintf(stderr, "read(%s): %s\n", filename, strerror(errno));
    return NULL;
  }
  ((char*)buf)[read_size] = 0;

  return buf;
}
