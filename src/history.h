/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef HISTORY_H
#define HISTORY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HISTORY_SIZE 50

struct History {
  unsigned int top;
  char *list[HISTORY_SIZE];
};

struct History *history;
int initialize_history();
int push_history(char*);

#endif
