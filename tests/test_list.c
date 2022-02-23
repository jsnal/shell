#include "test.h"
#include "../src/list.h"
#include <stdlib.h>


TEST_CASE(add)
{
  list_t *list = list_create();

  int *a = (int*) malloc(sizeof(int));
  *a = 10;

  list_add(list, 0, a);

  ASSERT_EQ_INT(*a, *((int*) list_get(list, 0)));
  ASSERT_EQ_STR("foo", "bar");

  list_destroy(list);
}
TEST_CASE_END();

TEST_MAIN("test_list")
{
  TEST_START();
  TEST_RUN(add);
  TEST_END();
}
