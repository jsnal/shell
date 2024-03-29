/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef EXEC_H
#define EXEC_H

#include "builtin.h"
#include "redirect.h"
#include "parse.h"
#include <dirent.h>
#include <sys/wait.h>

int execute(tree_t*);

typedef struct PipelineStateStruct {
  int command_count;
  int (*pfds)[2];
} pipelinestate_t;

#endif
