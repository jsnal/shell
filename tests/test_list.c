/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "test.h"
#include "../src/list.h"
#include <stdlib.h>

TEST_CASE(create_destroy)
{
  list_t *list = list_create();
  ASSERT_NOT_NULL(list);

  list_destroy(list);
}
TEST_CASE_END();

TEST_CASE(add)
{
  list_t *list = list_create();

  int *a = (int*) malloc(sizeof(int)); *a = 10;
  int *b = (int*) malloc(sizeof(int)); *b = 20;
  int *c = (int*) malloc(sizeof(int)); *c = 30;
  int *d = (int*) malloc(sizeof(int)); *d = 40;
  int *e = (int*) malloc(sizeof(int)); *e = 50;

  ASSERT_TRUE(list_add(list, 0, a));

  ASSERT_EQ_INT(1, list_size(list));
  ASSERT_EQ_INT(*a, *((int*) list_get(list, 0)));

  ASSERT_TRUE(list_add(list, list_size(list), b));

  ASSERT_EQ_INT(2, list_size(list));
  ASSERT_EQ_INT(*a, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 1)));

  ASSERT_TRUE(list_add(list, 0, c));
  ASSERT_TRUE(list_add(list, 0, d));

  ASSERT_EQ_INT(4, list_size(list));
  ASSERT_EQ_INT(*d, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*c, *((int*) list_get(list, 1)));
  ASSERT_EQ_INT(*a, *((int*) list_get(list, 2)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 3)));

  ASSERT_TRUE(list_add(list, 3, e));

  ASSERT_EQ_INT(5, list_size(list));
  ASSERT_EQ_INT(*d, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*c, *((int*) list_get(list, 1)));
  ASSERT_EQ_INT(*a, *((int*) list_get(list, 2)));
  ASSERT_EQ_INT(*e, *((int*) list_get(list, 3)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 4)));

  ASSERT_FALSE(list_add(list, 100, e));
  ASSERT_FALSE(list_add(list, -1, e));

  list_destroy(list);
}
TEST_CASE_END();

TEST_CASE(get)
{
  list_t *list = list_create();

  int *a = (int*) malloc(sizeof(int)); *a = 10;
  int *b = (int*) malloc(sizeof(int)); *b = 20;
  int *c = (int*) malloc(sizeof(int)); *c = 30;
  int *d = (int*) malloc(sizeof(int)); *d = 40;
  int *e = (int*) malloc(sizeof(int)); *e = 50;

  list_append(list, a);
  list_append(list, b);
  list_append(list, c);
  list_append(list, d);
  list_append(list, e);

  ASSERT_EQ_INT(5, list_size(list));
  ASSERT_EQ_INT(*a, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 1)));
  ASSERT_EQ_INT(*c, *((int*) list_get(list, 2)));
  ASSERT_EQ_INT(*d, *((int*) list_get(list, 3)));
  ASSERT_EQ_INT(*e, *((int*) list_get(list, 4)));

  list_destroy(list);
}
TEST_CASE_END();

TEST_CASE(append)
{
  list_t *list = list_create();

  int *a = (int*) malloc(sizeof(int)); *a = 10;
  int *b = (int*) malloc(sizeof(int)); *b = 20;
  int *c = (int*) malloc(sizeof(int)); *c = 30;
  int *d = (int*) malloc(sizeof(int)); *d = 40;
  int *e = (int*) malloc(sizeof(int)); *e = 50;

  ASSERT_TRUE(list_append(list, a));

  ASSERT_EQ_INT(1, list_size(list));
  ASSERT_EQ_INT(*a, *((int*) list_get(list, 0)));

  ASSERT_TRUE(list_append(list, b));
  ASSERT_TRUE(list_append(list, d));

  ASSERT_EQ_INT(3, list_size(list));
  ASSERT_EQ_INT(*a, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 1)));
  ASSERT_EQ_INT(*d, *((int*) list_get(list, 2)));

  ASSERT_TRUE(list_add(list, 0, c));
  ASSERT_TRUE(list_append(list, e));

  ASSERT_EQ_INT(5, list_size(list));
  ASSERT_EQ_INT(*c, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*a, *((int*) list_get(list, 1)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 2)));
  ASSERT_EQ_INT(*d, *((int*) list_get(list, 3)));
  ASSERT_EQ_INT(*e, *((int*) list_get(list, 4)));

  list_destroy(list);
}
TEST_CASE_END();

TEST_CASE(set)
{
  list_t *list = list_create();

  int *a = (int*) malloc(sizeof(int)); *a = 10;
  int *b = (int*) malloc(sizeof(int)); *b = 20;
  int *c = (int*) malloc(sizeof(int)); *c = 30;

  list_append(list, a);
  list_append(list, b);
  ASSERT_EQ_INT(2, list_size(list));

  ASSERT_EQ_INT(*a, *((int*) list_set(list, 0, b)));

  ASSERT_EQ_INT(2, list_size(list));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 1)));

  ASSERT_EQ_INT(*b, *((int*) list_set(list, 0, c)));

  ASSERT_EQ_INT(2, list_size(list));
  ASSERT_EQ_INT(*c, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 1)));

  list_destroy(list);
}
TEST_CASE_END();

TEST_CASE(remove)
{
   list_t *list = list_create();

  int *a = (int*) malloc(sizeof(int)); *a = 10;
  int *b = (int*) malloc(sizeof(int)); *b = 20;
  int *c = (int*) malloc(sizeof(int)); *c = 30;
  int *d = (int*) malloc(sizeof(int)); *d = 40;
  int *e = (int*) malloc(sizeof(int)); *e = 50;

  list_append(list, a);
  list_append(list, b);
  list_append(list, c);
  list_append(list, d);
  list_append(list, e);

  ASSERT_EQ_INT(5, list_size(list));

  ASSERT_EQ_INT(*a, *((int*) list_remove(list, 0)));
  ASSERT_EQ_INT(4, list_size(list));

  ASSERT_EQ_INT(*e, *((int*) list_remove(list, list_size(list) - 1)));
  ASSERT_EQ_INT(3, list_size(list));

  ASSERT_EQ_INT(*c, *((int*) list_remove(list, 1)));
  ASSERT_EQ_INT(2, list_size(list));

  ASSERT_EQ_INT(*b, *((int*) list_remove(list, 0)));
  ASSERT_EQ_INT(*d, *((int*) list_remove(list, 0)));
  ASSERT_EQ_INT(0, list_size(list));

  ASSERT_NULL(list_remove(list, 0));
  ASSERT_EQ_INT(0, list_size(list));

  ASSERT_NULL(list_remove(list, -1));
  ASSERT_NULL(list_remove(list, 100));

  list_destroy(list);
}
TEST_CASE_END();

TEST_CASE(mixed)
{
   list_t *list = list_create();

  int *a = (int*) malloc(sizeof(int)); *a = 10;
  int *b = (int*) malloc(sizeof(int)); *b = 20;
  int *c = (int*) malloc(sizeof(int)); *c = 30;
  int *d = (int*) malloc(sizeof(int)); *d = 40;

  ASSERT_TRUE(list_append(list, a));
  ASSERT_EQ_INT(1, list_size(list));

  ASSERT_EQ_INT(*a, *((int*) list_remove(list, 0)));
  ASSERT_EQ_INT(0, list_size(list));

  ASSERT_TRUE(list_add(list, 0, b));
  ASSERT_TRUE(list_add(list, 0, a));
  ASSERT_EQ_INT(*a, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 1)));
  ASSERT_EQ_INT(2, list_size(list));

  ASSERT_EQ_INT(*a, *((int*) list_set(list, 0, c)));
  ASSERT_EQ_INT(2, list_size(list));
  ASSERT_EQ_INT(*c, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 1)));

  ASSERT_TRUE(list_append(list, d));
  ASSERT_EQ_INT(*c, *((int*) list_get(list, 0)));
  ASSERT_EQ_INT(*b, *((int*) list_get(list, 1)));
  ASSERT_EQ_INT(*d, *((int*) list_get(list, 2)));

  list_destroy(list);
}
TEST_CASE_END();

TEST_MAIN("test_list")
{
  TEST_START();
  TEST_RUN(create_destroy);
  TEST_RUN(get);
  TEST_RUN(add);
  TEST_RUN(append);
  TEST_RUN(set);
  TEST_RUN(remove);
  TEST_RUN(mixed);
  TEST_END();
}
