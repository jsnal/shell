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

#endif
