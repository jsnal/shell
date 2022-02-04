/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "hashtable.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

hashtable_t *hashtable_create()
{
  hashtable_t *table = malloc(sizeof(hashtable_t));
  if (table == NULL) {
    errln("%s", strerror(errno));
  }

  table->length = 0;
  table->capacity = INITIAL_CAPACITY;

  table->entries = calloc(table->capacity, sizeof(hashtable_entry_t));
  if (table->entries == NULL) {
    free(table);
    errln("%s", strerror(errno));
    return NULL;
  }

  return table;
}

void hashtable_destroy(hashtable_t *table)
{
  for (int i = 0; i < table->capacity; i++) {
    if (table->entries[i].key != NULL) {
      free((void*) table->entries[i].key);
    }
  }

  free(table->entries);
  free(table);
}

static uint64_t hash_key(const char *key)
{
  uint64_t hash = FNV_OFFSET;
  for (const char *p = key; *p; p++) {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}

void *hashtable_get(hashtable_t *table, const char *key)
{
  int index = (int) (hash_key(key) & (uint64_t) (table->capacity - 1));

  for (; table->entries[index].key != NULL; index++) {
    if (strcmp(key, table->entries[index].key) == 0) {
      return table->entries[index].value;
    }

    if (index >= table->capacity) {
      index = 0;
    }
  }

  return NULL;
}

static const char *hashtable_set_entry(hashtable_t *table, const char *key,
    void *value)
{
  int index = (int) (hash_key(key) & (uint64_t) (table->capacity - 1));

  for (; table->entries[index].key != NULL; index++) {
    if (strcmp(key, table->entries[index].key) == 0) {
      table->entries[index].value = value;
      return table->entries[index].key;
    }

    if (index >= table->capacity) {
      index = 0;
    }
  }

  table->entries[index].key = (char*) strdup(key);
  table->entries[index].value = value;
  table->length++;

  return key;
}

size_t hashtable_length(const hashtable_t *table)
{
  return table->length;
}
