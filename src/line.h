#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

char *unsupported_terminals[] = {"dumb", "emacs", NULL};
struct termios orig_termios;

struct LineState {
  int fd_in;
  int fd_out;
  char *buffer;
  size_t bufferlen;
  const char *prompt;
  size_t prompt_length;
  size_t cursor_position;
  size_t line_length;
};

char *readline(const char*);
