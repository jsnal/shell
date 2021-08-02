#ifndef BUILTIN_H
/*
 * Copyright (c) 2021, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#define BUILTIN_H

#include "shell.h"
#include "history.h"
#include "command.h"

struct Builtin {
  char *name;
  int (*func)(struct Command *);
};

struct Builtin *check_builtin(struct Command*);
int exec_builtin(struct Builtin*, struct Command*);

#endif
