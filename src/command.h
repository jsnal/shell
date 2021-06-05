#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>

#define OPERATORS_SIZE 6

char *SYS_PATH;
char *SYS_HOME;
char *PWD;
char *OLDPWD;

enum Operators {
  REDIRECT_OUT,
  REDIRECT_OUT_APPEND,
  REDIRECT_IN,
  REDIRECT_ERROR,
  REDIRECT_ERROR_APPEND,
  REDIRECT_ALL
};

struct Command {
  unsigned int argc;
  char *name;
  char *argv[1024];
  char *redirects[OPERATORS_SIZE];
  int fds[2];
  struct Command *next;
  struct Command *prev;
};

struct CommandOperators {
  enum Operators name;
  char *token;
};

struct Command *command;

void cleanup_commands();
#endif
