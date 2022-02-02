/*
 * Copyright (c) 2021, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef BUILTIN_H
#define BUILTIN_H

#include "shell.h"
#include "history.h"

struct Builtin {
  char *name;
  int (*func)(struct Command *);
};

struct Builtin *check_builtin(struct Command*);
int exec_builtin(struct Builtin*, struct Command*);

#endif
