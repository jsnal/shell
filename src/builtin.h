/*
 * Copyright (c) 2021, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef BUILTIN_H
#define BUILTIN_H

#include "parse.h"

typedef struct Builtin {
  char *name;
  int (*func)(struct Command *command);
} builtin_t;

builtin_t *check_builtin(struct Command *command);
int exec_builtin(builtin_t *builtin, struct Command *command);

#endif
