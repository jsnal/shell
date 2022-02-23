#include "test.h"
#include "../src/hashtable.h"

TEST_CASE(create)
{
  hashtable_t *table = hashtable_create();
  ASSERT_NOT_NULL(table);
  hashtable_destroy(table);
}
TEST_CASE_END();

TEST_MAIN("test_hashtable")
{
  TEST_START();
  TEST_RUN(create);
  TEST_END();
}
