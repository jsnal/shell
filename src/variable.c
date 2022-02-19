/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "hashtable.h"
#include "util.h"
#include "variable.h"
#include "debug.h"
#include <stdlib.h>

static char *xgetcwd()
{
  size_t pwd_len = 40;
  char *pwd = xmalloc(pwd_len);
}

variable_scope_t *initialize_main_scope()
{
  variable_scope_t *main_scope = (variable_scope_t*) xmalloc(sizeof(variable_scope_t));

  main_scope->parent = NULL;
  main_scope->vars = hashtable_create();

  char *pwd = getenv(VAR_PWD);

  if (pwd == NULL) {
    pwd = xgetcwd();
  }

  warnln("%s", pwd);


  return main_scope;
}

void destroy_scope(variable_scope_t *scope)
{
  hashtable_destroy(scope->vars);
  free(scope);
}
