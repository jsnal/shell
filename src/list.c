/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "list.h"
#include "util.h"
#include <stdbool.h>

static bool out_of_bounds(list_t *list, int index)
{
  return index < 0 || index >= list->size;
}

list_t *list_create()
{
  list_t *list = (list_t*) xmalloc(sizeof(list_t));

  list->size = 0;
  list->head = (list_entry_t*) xmalloc(sizeof(list_entry_t));
  list->tail = NULL;

  return list;
}

void list_destroy(list_t *list)
{
  list_entry_t *freeable, *current = list->head->next;
  for (int i = 0; i < list->size; i++) {
    freeable = current;
    current = current->next;

    if (freeable->value != NULL) {
      free(freeable->value);
    }
    free(freeable);
  }

  free(list->head);
  free(list);
}

void *list_get(list_t *list, int index)
{
  if (out_of_bounds(list, index)) {
    return NULL;
  }

  list_entry_t *current = list->head->next;

  for (int i = 0; i < index; i++) {
    current = current->next;
  }

  return current ? current->value : NULL;
}

int list_get_index(list_t *list, void *value)
{
  list_entry_t *current = list->head->next;
  for (int i = 0; i < list->size; i++) {
    if (current->value == value) {
      return i;
    }
    current = current->next;
  }

  return -1;
}

bool list_add(list_t *list, int index, void *value)
{
  if (index < 0 || index > list->size) {
    return false;
  }

  list_entry_t *new_entry = (list_entry_t*) xmalloc(sizeof(list_entry_t));
  new_entry->value = value;

  if (index == 0) {
    new_entry->next = list->head->next;
    list->head->next = new_entry;

    if (list->size == 0) {
      list->tail = list->head->next;
    }
  } else if (index == list->size) {
    list->tail->next = new_entry;
    list->tail = list->tail->next;
  } else {
    list_entry_t *current = list->head->next;

    for (int i = 0; i < index - 1; i++) {
      current = current->next;
    }

    new_entry->next = current->next;
    current->next = new_entry;
  }

  list->size++;
  return true;
}

bool list_append(list_t *list, void *value)
{
  return list_add(list, list->size, value);
}

void *list_set(list_t *list, int index, void *value)
{
  if (out_of_bounds(list, index)) {
    return NULL;
  }

  list_entry_t *current = list->head;

  for (int i = 0; i <= index; i++) {
    current = current->next;
  }

  void *old_value = NULL;
  if (current != NULL) {
    old_value = current->value;
    current->value = value;
  }

  return old_value;
}

void *list_remove(list_t *list, int index)
{
  if (out_of_bounds(list, index)) {
    return NULL;
  }

  list_entry_t *old = NULL;

  if (index == 0) {
    old = list->head->next;
    list->head->next = list->head->next->next;
  } else if (index == list->size - 1) {
    list_entry_t *current = list->head->next;

    for (int i = 0; i < list->size - 2; i++) {
      current = current->next;
    }

    old = list->tail;
    list->tail = current;
  } else {
    list_entry_t *current = list->head->next;

    for (int i = 0; i < index - 1; i++) {
      current = current->next;
    }

    old = current->next;
    current->next = current->next->next;
  }

  list->size--;
  void *value = old->value;
  free(old);
  return value;
}

size_t list_size(list_t *list)
{
  if (list == NULL) {
    return -1;
  }

  return list->size;
}

list_iterator_t *list_iterator_create(list_t *list)
{
  if (!list) {
    return NULL;
  }

  list_iterator_t *it = (list_iterator_t*) xmalloc(sizeof(list_iterator_t));
  it->list = list;
  it->position = 0;
  it->remove_allowed = false;

  return it;
}

void list_iterator_destroy(list_iterator_t *it)
{
  free(it);
}

bool list_iterator_has_next(list_iterator_t *it)
{
  return list_size(it->list) != it->position;
}

void *list_iterator_next(list_iterator_t *it)
{
  if (!list_iterator_has_next(it)) {
    return NULL;
  }

  it->remove_allowed = true;
  return list_get(it->list, it->position++);
}

void *list_iterator_remove(list_iterator_t *it)
{
  if (!it->remove_allowed) {
    return NULL;
  }

  it->remove_allowed = false;
  return list_remove(it->list, --it->position);
}
