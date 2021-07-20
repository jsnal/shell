#ifndef EXEC_H
#define EXEC_H

#include "builtin.h"
#include "command.h"
#include "redirect.h"
#include "parse.h"
#include <dirent.h>
#include <sys/wait.h>

int execute(struct Tree*);

#endif
