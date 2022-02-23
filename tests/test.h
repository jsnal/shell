#ifndef TEST_CASE_H
#define TEST_CASE_H

#include "asserts.h"
#include <stdio.h>

#define TEST_CASE(name) \
  static int name() { \
    int failed = 0; \

#define TEST_CASE_END() \
    return failed; \
  } \

#define TEST_MAIN(name) \
  static const char* test_name = name; \
  int main()

#define TEST_START() \
  printf("Running: %s\n", test_name); \
  int total = 0, failed = 0 \

#define TEST_END() \
  printf("Finished: %s -- %d passed %d failed\n", test_name, total - failed, failed)

#define TEST_RUN(name) \
  total++; \
  failed += name()

#endif
