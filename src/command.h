#ifndef COMMAND_H
#define COMMAND_H

#include "tokenize.h"
#include <stdlib.h>

//TODO:REMOVE
#define OPERATORS_SIZE 7

char *SYS_PATH;
char *SYS_HOME;
char *PWD;
char *OLDPWD;

enum RedirectOperators {
  REDIRECT_ALL,
  REDIRECT_ERROR,
  REDIRECT_ERROR_APPEND,
  REDIRECT_IN,
  REDIRECT_IN_APPEND,
  REDIRECT_OUT,
  REDIRECT_OUT_APPEND,
};

// enum ListOperator {
//   l_And,
//   l_None,
//   l_Or,
//   l_Pipe,
//   l_Sequential,
// };

struct Command {
  unsigned int argc;
  char *name;
  char *argv[1024];
  char *redirects[TOKENS_SIZE];
  // enum TokenState listOpt;
  int fds[2];
  struct Command *next;
  struct Command *prev;
};

struct Command *command;

void cleanup_commands(struct Command*);
#endif
