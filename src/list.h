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
void *list_get(list_t*, int index);
int list_get_index(list_t*, void*);
bool list_add(list_t*, int index, void *value);
bool list_append(list_t*, void *value);
void *list_set(list_t*, int index, void *value);
void *list_remove(list_t*, int index);
void list_destroy(list_t*);
size_t list_size(list_t*);
list_iterator_t *list_iterator_create(list_t*);
void list_iterator_destroy(list_iterator_t*);
bool list_iterator_has_next(list_iterator_t*);
void *list_iterator_next(list_iterator_t*);
void *list_iterator_remove(list_iterator_t*);

#endif
