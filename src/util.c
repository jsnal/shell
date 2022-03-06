/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

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

resize_buffer_t *resize_buffer_create(size_t capacity)
{
  resize_buffer_t *rb = (resize_buffer_t*) xmalloc(sizeof(resize_buffer_t));
  rb->buffer = (char*) xmalloc(sizeof(char) * capacity);
  rb->length = 0;
  rb->capacity = capacity;

  return rb;
}

void destroy_resize_buffer(resize_buffer_t *b)
{
  free(b->buffer);
  free(b);
}

static bool resize_buffer(resize_buffer_t *b, size_t length)
{
  if (b->length + length > b->capacity) {
    b->capacity *= 2;
    b->buffer = realloc(b->buffer, b->capacity);

    if (b->buffer == NULL) {
      return false;
    }
  }

  return true;
}

bool resize_buffer_append_char(resize_buffer_t *b, const char c)
{
  if (!resize_buffer(b, 1)) {
    return false;
  }

  b->buffer[b->length++] = c;
  b->buffer[b->length] = '\0';
  return true;
}

bool resize_buffer_append_str(resize_buffer_t *b, const char *str)
{
  size_t length = strlen(str);

  if (!resize_buffer(b, length)) {
    return false;
  }

  memcpy(b->buffer + b->length, str, length);
  b->length += length;
  b->buffer[b->length] = '\0';

  return true;
}
