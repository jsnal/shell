/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef LINE_H
#define LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define LINE_LENGTH 4096

#define PERFORM_EDIT(edit) \
  if (!edit(&state)) return false

enum KeyAction {
  KEY_NULL = 0,
  CTRL_B = 2,
  CTRL_C = 3,
  CTRL_D = 4,
  CTRL_F = 6,
  CTRL_H = 8,
  CTRL_K = 11,
  CTRL_L = 12,
  CTRL_W = 23,
  ESC = 27,
  BACKSPACE = 127,
  ENTER = 13,
};

typedef struct LineStateStruct {
  int fd_in;
  int fd_out;
  char *buffer;
  size_t bufferlen;
  const char *prompt;
  size_t prompt_length;
  size_t cursor_position;
  size_t line_length;
  size_t columns;
} line_state_t;

char *readline(const char*, int*);

#endif
