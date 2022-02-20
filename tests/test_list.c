#include "tester.h"
#include <list.h>

#include <stdlib.h>


TEST_CASE(add)
{
  list_t *list = list_create();

  int *a = (int*) malloc(sizeof(int));
  *a = 10;

  list_add(list, 0, a);

  ASSERT_EQ_INT(*a, *((int*) list_get(list, 0)));

  list_destroy(list);
}


TEST_MAIN(list) {
  add();
}
