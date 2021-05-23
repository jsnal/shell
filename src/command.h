#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>

char *SYS_PATH;
char *SYS_HOME;

struct Command {
  unsigned int argc;
  char *name;
  char *argv[1024];
  int fds[2];
  struct Command *next;
  struct Command *prev;
};

// struct Commands {
//   int count;
//   struct Command *list[];
// };

struct Command *command;
int command_count;

void cleanup_commands();
#endif
