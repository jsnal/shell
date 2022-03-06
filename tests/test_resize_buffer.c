/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "test.h"
#include "../src/util.h"

TEST_CASE(create_and_destroy)
{
  resize_buffer_t *b = create_resize_buffer(10);

  ASSERT_NOT_NULL(b);
  ASSERT_NOT_NULL(b->buffer);
  ASSERT_EQ_INT(10, b->capacity);
  ASSERT_EQ_INT(0, b->length);

  destroy_resize_buffer(b);
}
TEST_CASE_END();

TEST_CASE(append)
{
  resize_buffer_t *b = create_resize_buffer(10);

  ASSERT_TRUE(resize_buffer_append(b, "abc"));
  ASSERT_EQ_INT(3, b->length);
  ASSERT_EQ_STR("abc", b->buffer);
  ASSERT_TRUE(resize_buffer_append(b, "abc"));
  ASSERT_EQ_INT(6, b->length);
  ASSERT_EQ_STR("abcabc", b->buffer);

  ASSERT_TRUE(resize_buffer_append(b, "abcdefgh"));
  ASSERT_EQ_INT(14, b->length);
  ASSERT_EQ_INT(20, b->capacity);
  ASSERT_EQ_STR("abcabcabcdefgh", b->buffer);

  ASSERT_TRUE(resize_buffer_append(b, "ijklmnop"));
  ASSERT_EQ_INT(22, b->length);
  ASSERT_EQ_INT(40, b->capacity);
  ASSERT_EQ_STR("abcabcabcdefghijklmnop", b->buffer);

  destroy_resize_buffer(b);
}
TEST_CASE_END();

TEST_MAIN("test_resize_buffer")
{
  TEST_START();
  TEST_RUN(create_and_destroy);
  TEST_RUN(append);
  TEST_END();
}
