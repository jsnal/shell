#ifndef EXEC_H
#define EXEC_H

#include "builtin.h"
#include "command.h"
#include "redirect.h"
#include <dirent.h>
#include <sys/wait.h>

int exec_commands(struct Command*);

#endif
