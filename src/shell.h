#ifndef SHELL_H
#define SHELL_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include "exec.h"
#include "command.h"

#define HISTORY_SIZE 50

struct History {
  unsigned int top;
  char *list[HISTORY_SIZE];
};

int shell();
struct History *history;

#endif
