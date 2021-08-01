#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define LINE_LENGTH 4096

enum KeyAction {
  KEY_NULL = 0,
  CTRL_C = 3,
  CTRL_D = 4,
  CTRL_H = 8,
  BACKSPACE = 127,
  ENTER = 13,
};

struct AppendBuffer {
  char *buffer;
  size_t length;
};

struct LineState {
  int fd_in;
  int fd_out;
  char *buffer;
  size_t bufferlen;
  const char *prompt;
  size_t prompt_length;
  size_t cursor_position;
  size_t line_length;
  size_t columns;
};

char *readline(const char*, int*);
