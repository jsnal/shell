#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>

void fatal();
char *xstrdup(const char *s);
void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
void xfree(void *ptr);

#endif
