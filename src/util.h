#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct ResizeBufferStruct {
  char *buffer;
  size_t length;
  size_t capacity;
} resize_buffer_t;

void fatal();
char *xstrdup(const char *s);
void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
bool resize_buffer_append(resize_buffer_t *b, const char *str);
resize_buffer_t *create_resize_buffer(size_t capacity);
void destroy_resize_buffer(resize_buffer_t *b);

#endif
