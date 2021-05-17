#ifndef BUILTIN_H
#define BUILTIN_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "command.h"

struct Builtin {
  char *name;
  int (*func)(struct Command *);
};

struct Builtin *check_builtin(struct Command*);
int exec_builtin(struct Builtin*, struct Command*);

#endif
