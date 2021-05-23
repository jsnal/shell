#ifndef EXEC_H
#define EXEC_H

#include <dirent.h>
#include <sys/wait.h>
#include "builtin.h"
#include "command.h"

int exec_commands(struct Command*);

#endif
