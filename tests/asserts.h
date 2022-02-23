#ifndef TEST_ASSERTS_H
#define TEST_ASSERTS_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define RED   "\x1B[31m"
#define BOLD  "\x1B[1m"
#define RESET "\x1B[0m"

static int fail(const char *file, const int line, const char *fmt, ...)
{
    va_list args;
    fprintf(stdout, RED BOLD "FAIL: " RESET "%s:%d:", file, line);
    va_start(args, fmt);
    int ret = vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "\n");

    return ret;
}

#define FAIL(fmt, ...)                        \
  fail(__FILE__, __LINE__, fmt, __VA_ARGS__); \
  failed++

#define ASSERT_EQ_INT(a, b) 																						 \
  if (a != b) { 																												 \
    FAIL("ASSERT_EQ_INT(%d, %d) expected = %d, found = %d", a, b, a, b); \
  }

#define ASSERT_EQ_STR(a, b) 																									      		 \
  if (strcmp(a, b) != 0) { 																															 \
    FAIL("ASSERT_EQ_STR(\"%s\", \"%s\") expected = \"%s\", found = \"%s\"", a, b, a, b); \
  }

#define ASSERT_NOT_NULL(a)                 \
  if (!a) {                                \
    FAIL("ASSERT_NOT_NULL(a) failed", ""); \
  }

#endif
