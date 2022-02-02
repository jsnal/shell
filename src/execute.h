/*
 * Copyright (c) 2021, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef EXEC_H
#define EXEC_H

#include "builtin.h"
#include "command.h"
#include "redirect.h"
#include "parse.h"
#include <dirent.h>
#include <sys/wait.h>

int execute(struct Tree*);

struct PipelineSummary {
  int command_count;
  int (*pfds)[2];
};

#endif
