/*
 * Copyright (c) 2021-2022-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef BUILTIN_H
#define BUILTIN_H

#include "parse.h"

typedef struct Builtin {
  char *name;
  int (*func)(command_t *command);
} builtin_t;

builtin_t *check_builtin(command_t *command);
int exec_builtin(builtin_t *builtin, command_t *command);

#endif
