#include "util.h"
#include "debug.h"
#include "errno.h"
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static void fatal_sig_handler(int sig)
{
  errln("Exiting shell");
  exit(EXIT_FAILURE);
}

void fatal()
{
  errln("%s\nHanging...", strerror(errno));

  signal(SIGINT, fatal_sig_handler);

  // Hang the program forever
  for (;;) { }
}

void *xcalloc(size_t nmemb, size_t size)
{
  void *result = calloc(nmemb, size);

  if (!result) {
    fatal();
  }

  return result;
}

void *xmalloc(size_t size)
{
  void *result = malloc(size);

  if (!result) {
    fatal();
  }

  return result;
}

char *xstrdup(const char *s)
{
  size_t size = strlen(s) + 1;
  char *str = malloc(size);

  if (str) {
    memcpy(str, s, size);
  }

  return str;
}

bool resize_buffer_append(resize_buffer_t *b, const char *str, size_t length)
{
  char *new = realloc(b->buffer, b->length + length);

  if (new == NULL) {
    return false;
  }

  memcpy(new + b->length, str, length);
  b->buffer = new;
  b->length += length;
  return true;
}
