/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct ListEntryStruct {
  struct ListEntryStruct *next;
  void *value;
} list_entry_t;

typedef struct ListStruct {
  list_entry_t *head;
  list_entry_t *tail;
  size_t size;
} list_t;

typedef struct ListIteratorStruct {
  list_t *list;
  int position;
  bool remove_allowed;
} list_iterator_t;


list_t *list_create();
void *list_get(list_t *list, int index);
bool list_add(list_t *list, int index, void *value);
bool list_append(list_t *list, void *value);
void *list_set(list_t *list, int index, void *value);
void *list_remove(list_t *list, int index);
void list_destroy(list_t *list);
size_t list_size(list_t *list);
list_iterator_t *list_iterator_create(list_t *list);
bool list_iterator_has_next(list_iterator_t *it);
void *list_iterator_next(list_iterator_t *it);
void *list_iterator_remove(list_iterator_t *it);

#endif
