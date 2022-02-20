#ifndef TEST_CASE_H
#define TEST_CASE_H

#include "tester_asserts.h"

void (*tests)();

#define TEST_CASE(name) \
  static void name() \

#define TEST_MAIN(name) \
  void name() \

#endif
