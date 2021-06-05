#ifndef SHELL_H
#define SHELL_H

#include "command.h"
#include "exec.h"
#include "history.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int shell();

#endif
