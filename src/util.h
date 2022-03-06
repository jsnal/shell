/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

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

resize_buffer_t *resize_buffer_create(size_t capacity);
bool resize_buffer_append_char(resize_buffer_t *b, const char str);
bool resize_buffer_append_str(resize_buffer_t *b, const char *str);
void destroy_resize_buffer(resize_buffer_t *b);

#endif
