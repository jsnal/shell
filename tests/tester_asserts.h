#ifndef TEST_ASSERTS_H
#define TEST_ASSERTS_H

#define RED   "\x1B[31m"
#define GREEN "\x1B[32m"
#define BOLD  "\x1B[1m"
#define RESET "\x1B[0m"

int fail(const char *fmt, ...);
int pass();

#define ASSERT_EQ_INT(a, b) 																													 \
  if (a != b) { 																																			 \
    fail("ASSERT_EQ_INT(%d, %d) failed expected = %d, found = %d", a, b, a, b); \
  } else {                                                                             \
    pass("");																																	 \
  }																																										 \

#endif
