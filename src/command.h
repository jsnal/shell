#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>

char *SYS_PATH;
char *SYS_HOME;
char *PWD;
char *OLDPWD;

enum CommandOperators {
  NORMAL,
  REDIRECT_IN,
  REDIRECT_OUT,
  REDIRECT_ERROR,
  REDIRECT_ALLOUT,
};

struct Command {
  unsigned int argc;
  char *name;
  char *argv[1024];
  char *redirects[3];
  int fds[2];
  struct Command *next;
  struct Command *prev;
};

struct Command *command;

void cleanup_commands();
#endif
