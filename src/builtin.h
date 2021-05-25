#ifndef BUILTIN_H
#define BUILTIN_H

#include "shell.h"
#include "command.h"

struct Builtin {
  char *name;
  int (*func)(struct Command *);
};

struct Builtin *check_builtin(struct Command*);
int exec_builtin(struct Builtin*, struct Command*);

#endif
