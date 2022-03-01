/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include "execute.h"
#include "line.h"
//#include "parse.h"
#include "tokenize.h"

typedef struct ShellArgumentsStruct {
  bool dump_ast;
  bool dump_tokens;
} shell_arguments_t;

int shell(const shell_arguments_t *args);

#endif
