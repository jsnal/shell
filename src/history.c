/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "history.h"

int initialize_history()
{
  history = (struct History*) malloc(sizeof(struct History));

  if (history == NULL)
    return -1;

  for (unsigned int i = 0; i < HISTORY_SIZE; i++)
    history->list[i] = NULL;

  history->top = 0;
  return 0;
}

int push_history(char *cmd)
{
  char *c = (char*) malloc(strlen(cmd) + 1);
  if (c == NULL)
    fprintf(stderr, "error: push_history: malloc failed\n");

  strcpy(c, cmd);

  if (history->top >= HISTORY_SIZE)
  {
    free(history->list[0]);
    for (unsigned int i = 0; history->list[i] != NULL; i++)
      history->list[i] = history->list[i + 1];

    history->list[HISTORY_SIZE - 1] = c;
    return 0;
  }

  history->list[history->top] = c;
  history->top++;

  return 0;
}
