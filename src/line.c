/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>
#include "debug.h"
#include "util.h"
#include "line.h"

static char *unsupported_terminals[] = {"dumb", "emacs", NULL};
static struct termios orig_termios;

static int is_unsupported_terminal()
{
  char *term = getenv("TERM");

  if (term == NULL) {
    return 0;
  }

  for (int i = 0; unsupported_terminals[i]; i++) {
    // FIXME: upper and lower case possibly?
    if (!strcmp(term, unsupported_terminals[i])) {
      return 1;
    }
  }

  return 0;
}

static int get_columns(int fd_in, int fd_out)
{
  struct winsize ws;

  if (ioctl(1, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    /* TODO create a fallback way of getting the columns */
    errln("Unable to get columns. Defaulting to 80.");
    return 80;
  }

  return ws.ws_col;
}

static bool refresh(line_state_t *state)
{
  char escape_sequence[64];
  char *buffer = state->buffer;
  size_t line_length = state->line_length;
  size_t cursor_position = state->cursor_position;
  resize_buffer_t *resize_buffer = create_resize_buffer(32);

  while (state->prompt_length + state->cursor_position >= state->columns) {
    buffer++;
    line_length--;
    cursor_position--;
  }

  while ((state->prompt_length + state->line_length) > state->columns) {
    line_length--;
  }

  /* Move the cursor to the left edge */
  snprintf(escape_sequence, 64, "\r");
  resize_buffer_append(resize_buffer, escape_sequence);

  resize_buffer_append(resize_buffer, state->prompt);
  resize_buffer_append(resize_buffer, state->buffer);

  /* Erase to right if something was deleted */
  snprintf(escape_sequence, 64, "\x1b[0K");
  resize_buffer_append(resize_buffer, escape_sequence);

  /* Move cursor to original position */
  snprintf(escape_sequence, 64, "\r\x1b[%dC", (int) (state->cursor_position + state->prompt_length));
  resize_buffer_append(resize_buffer, escape_sequence);

  if (write(state->fd_out, resize_buffer->buffer, resize_buffer->length) == -1) {
    return false;
  }

  destroy_resize_buffer(resize_buffer);
  return true;
}

static bool edit_insert(line_state_t *state, char c)
{
  if (state->line_length >= state->bufferlen) {
    return false;
  }

  if (state->line_length != state->cursor_position) {
    memmove(state->buffer + state->cursor_position + 1,
            state->buffer + state->cursor_position,
            state->line_length - state->cursor_position);
  }

  state->buffer[state->cursor_position] = c;
  state->cursor_position++;
  state->line_length++;
  state->buffer[state->line_length] = '\0';

  /* If possible, don't redraw the whole line */
  if (state->line_length == state->cursor_position &&
      state->prompt_length + state->line_length < state->columns) {
    write(state->fd_out, &c, 1);
  } else {
    return refresh(state);
  }

  return true;
}

static bool edit_backspace(line_state_t *state)
{
  if (state->cursor_position > 0 && state->line_length > 0) {
    memmove(state->buffer + state->cursor_position - 1,
            state->buffer + state->cursor_position,
            state->line_length - state->cursor_position);
    state->cursor_position--;
    state->line_length--;
    state->buffer[state->line_length] = '\0';

    if (!refresh(state)) {
      return false;
    }
  }

  return true;
}

static bool edit_move_left(line_state_t *state)
{
  if (state->line_length > 0 && state->cursor_position > 0) {
    state->cursor_position--;
    if (!refresh(state)) {
      return false;
    }
  }
  return true;
}

static bool edit_move_right(line_state_t *state)
{
  if (state->line_length > 0 && state->cursor_position != state->line_length) {
    state->cursor_position++;
    if (!refresh(state)) {
      return false;
    }
  }
  return true;
}

static bool edit_kill_word_backwards(line_state_t *state)
{
  int old_position = state->cursor_position;

  while (state->cursor_position > 0 && state->buffer[state->cursor_position - 1] == ' ') {
    state->cursor_position--;
  }

  while (state->cursor_position > 0 && state->buffer[state->cursor_position - 1] != ' ') {
    state->cursor_position--;
  }

  memmove(state->buffer + state->cursor_position, state->buffer + old_position, state->line_length - old_position + 1);
  state->line_length -= old_position - state->cursor_position;
  if (!refresh(state)) {
    return false;
  }

  return true;
}

static bool edit_clear_screen(line_state_t *state)
{
  char *escape_sequence = "\x1b[H\x1b[2J";
  if (write(state->fd_out, escape_sequence, strlen(escape_sequence)) <= 0) {
    return false;
  }

  if (!refresh(state)) {
    return false;
  }

  return true;
}

static bool edit(char *buffer, size_t bufferlen, const char *prompt)
{
  line_state_t state = {
    .fd_in = STDIN_FILENO,
    .fd_out = STDOUT_FILENO,
    .buffer = buffer,
    .bufferlen = bufferlen,
    .prompt = prompt,
    .prompt_length = strlen(prompt),
    .line_length = 0,
    .cursor_position = 0,
    .columns = get_columns(STDIN_FILENO, STDOUT_FILENO),
  };

  state.buffer[0] = '\0';
  state.bufferlen--;

  if (write(state.fd_out, prompt, state.prompt_length) == -1)
    return -1;

  for (;;) {
    char escape_sequence[3];
    char c;
    int nread;

    nread = read(state.fd_in, &c, 1);
    if (nread <= 0)
      return state.line_length;

    switch (c) {
      case ENTER:
        return state.line_length;
      case ESC:
        if (read(state.fd_in, escape_sequence, 2) == -1) {
          break;
        }

        if (escape_sequence[0] == '[') {
          switch (escape_sequence[1]) {
            case 'A':
              dbgln("Up arrow!");
              break;
            case 'B':
              dbgln("Down arrow!");
              break;
            case 'C':
              PERFORM_EDIT(edit_move_right);
              break;
            case 'D':
              PERFORM_EDIT(edit_move_left);
              break;
          }
        }

        break;
      case CTRL_B:
        PERFORM_EDIT(edit_move_left);
        break;
      case CTRL_D:
      case CTRL_C:
        return false;
      case CTRL_F:
        PERFORM_EDIT(edit_move_right);
        break;
      case BACKSPACE:
      case CTRL_H:
        PERFORM_EDIT(edit_backspace);
        break;
      case CTRL_L:
        PERFORM_EDIT(edit_clear_screen);
        break;
      case CTRL_K:
        PERFORM_EDIT(edit_kill_word_backwards);
        break;
      default:
        if (!edit_insert(&state, c)) {
          return false;
        }
        break;
    }
  }

  return true;
}

/* This function is pretty common in all line editing libraries
 * so I pretty much completely stole it. :) */
static bool enable_raw_mode(int fd)
{
  struct termios raw;

  if (!isatty(STDIN_FILENO)) {
    return false;
  }

  if (tcgetattr(fd, &orig_termios) == -1) {
    return false;
  }
  // Modify the original mode
  raw = orig_termios;

  // Input modes: no break, no CR to NL, no parity check, no strip char,
  // no start/stop output control.
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  // Output modes - disable post processing
  raw.c_oflag &= ~(OPOST);

  // Control modes - set 8 bit chars
  raw.c_cflag |= (CS8);

  // Local modes - choing off, canonical off, no extended functions,
  // no signal chars (^Z,^C)
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  // Control chars - set return condition: min number of bytes and timer.
  // We want read to return every single byte, without timeout.
  raw.c_cc[VMIN] = 1;

  // 1 byte, no timer
  raw.c_cc[VTIME] = 0;

  // Put terminal in raw mode after flushing
  if (tcsetattr(fd, TCSAFLUSH, &raw) < 0) {
    return false;
  }

  return true;
}

static char *handle_unsupported_terminal(const char *prompt)
{
  int buf_size = 128;
  char *ret_line = xmalloc(buf_size * sizeof(char));
  unsigned int i = 0;
  char c;

  printf("%s", prompt);
  fflush(stdout);

  while ((c = getchar()) != '\n') {
    if (c == EOF) {
      free(ret_line);
      exit(EXIT_SUCCESS);
    }

    if (i >= buf_size) {
      ret_line = realloc(ret_line, buf_size * 2);
    }

    ret_line[i++] = c;
  }

  ret_line[i] = '\0';
  return ret_line;
}

static int disable_raw_mode(int fd)
{
  return tcsetattr(fd, TCSAFLUSH, &orig_termios);
}

static bool handle_read_raw(char *buffer, size_t bufferlen, const char *prompt)
{
  if (bufferlen == 0) {
    return false;
  }

  if (!enable_raw_mode(STDIN_FILENO)) {
    return false;
  }

  bool status = edit(buffer, bufferlen, prompt);
  disable_raw_mode(STDIN_FILENO);
  putchar('\n');
  return status;
}

char *readline(const char* prompt, int *status)
{
  char buffer[LINE_LENGTH];
  *status = 1;

  if (!isatty(STDIN_FILENO)) {
    errln("No support for standard input scripts.\nExiting...");
    *status = -1;
    return NULL;
  } else if (is_unsupported_terminal()) {
    *status = 2;
    return handle_unsupported_terminal(prompt);
  } else {
    if (!handle_read_raw(buffer, LINE_LENGTH, prompt)) {
      *status = -1;
    }
    return xstrdup(buffer);
  }
}
