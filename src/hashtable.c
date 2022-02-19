/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "debug.h"
#include "hashtable.h"
#include "util.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

hashtable_t *hashtable_create()
{
  hashtable_t *table = xmalloc(sizeof(hashtable_t));

  table->length = 0;
  table->capacity = INITIAL_CAPACITY;
  table->entries = xcalloc(table->capacity, sizeof(hashtable_entry_t));

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

static const char *hashtable_put_entry(hashtable_entry_t *entries, int capacity,
    const char *key, void *value, size_t *length)
{
  int index = (int) (hash_key(key) & (uint64_t) (capacity - 1));

  for (; entries[index].key != NULL; index++) {
    if (strcmp(key, entries[index].key) == 0) {
      entries[index].value = value;
      return entries[index].key;
    }

    if (index >= capacity) {
      index = 0;
    }
  }

  if (length != NULL) {
    key = xstrdup(key);
    if (key == NULL) {
      return NULL;
    }
    (*length)++;
  }

  entries[index].key = (char*) key;
  entries[index].value = value;

  return key;
}

static bool hashtable_resize(hashtable_t *table)
{
  int new_capacity = table->capacity * 2;
  if (new_capacity < table->capacity) {
    return false;
  }

  hashtable_entry_t *new_entries =
      (hashtable_entry_t*) xcalloc(new_capacity, sizeof(hashtable_entry_t));

  for (int i = 0; i < table->capacity; i++) {
    hashtable_entry_t entry = table->entries[i];

    if (entry.key != NULL) {
      hashtable_put_entry(new_entries, new_capacity, entry.key,
          entry.value, NULL);
    }
  }

  free(table->entries);
  table->entries = new_entries;
  table->capacity = new_capacity;
  return true;
}

const char *hashtable_put(hashtable_t *table, const char *key, void *value)
{
  if (value == NULL) {
    return NULL;
  }

  if (table->length >= table->capacity / 2) {
    if (!hashtable_resize(table)) {
      return NULL;
    }
  }

  return hashtable_put_entry(table->entries, table->capacity, key,
      value, &table->length);
}

size_t hashtable_length(const hashtable_t *table)
{
  return table->length;
}
