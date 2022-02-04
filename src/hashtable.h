/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

#define INITIAL_CAPACITY 16

// Constants for FNV-1a Hash code
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct HashTableEntry {
  const char *key;
  void *value;
} hashtable_entry_t;

typedef struct HashTable {
  hashtable_entry_t *entries;
  size_t capacity;
  size_t length;
} hashtable_t;

hashtable_t *hashtable_create();
void hashtable_destroy(hashtable_t *table);
void *hashtable_get(hashtable_t *table, const char *key);
const char *hashtable_put(hashtable_t *table, const char *key, void *value);
size_t hashtable_length(const hashtable_t *table);

#endif
