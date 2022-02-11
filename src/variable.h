/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef VARIABLE_H
#define VARIABLE_H

#include "hashtable.h"

#define VAR_OLDPWD "OLDPWD"
#define VAR_PATH   "PATH"
#define VAR_PS1    "PS1"
#define VAR_PWD    "PWD"

typedef struct VariableScope {
  struct VariableScope *parent;
  hashtable_t *vars;
} variable_scope_t;

variable_scope_t *initialize_main_scope();
void destroy_scope(variable_scope_t *scope);

#endif
