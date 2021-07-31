#include "line.h"

int is_unsupported_terminal()
{
  char *term = getenv("TERM");

  if (term == NULL)
    return 0;

  for (int i = 0; unsupported_terminals[i]; i++)
    if (!strcasecmp(term, unsupported_terminals[i]))
      return 1;

  return 0;
}

int edit_insert(struct LineState *state, char c)
{
  if (state->line_length < state->bufferlen)
  {

  }
}

int edit(int fd_in, int fd_out, char *buffer, size_t bufferlen, const char *prompt)
{
  struct LineState state = {
    .fd_in = fd_in,
    .fd_out = fd_out,
    .buffer = buffer,
    .bufferlen = bufferlen,
    .prompt = prompt,
    .prompt_length = strlen(prompt),
    .line_length = 0,
    .cursor_position = 0,
  };

  state.buffer[0] = '\0';
  state.bufferlen--;

  if (write(state.fd_out, prompt, state.prompt_length) == -1)
    return -1;

  for (;;)
  {
    char c, seq[3];
    int nread;

    nread = read(state.fd_in, &c, 1);
    if (nread <= 0)
      return state.line_length;

    switch (c)
    {
      case 13:
        printf("enter found!\n");
        return state.line_length;
      default:
        printf("character\n");
        state.buffer = "test";
        break;
    }
    return 0;
  }
}

char *handle_unsupported_terminal(const char *prompt)
{
  int buf_size = 128;
  char *ret_line = malloc(buf_size * sizeof(char));
  unsigned int i = 0;
  char c;

  printf("%s", prompt);
  fflush(stdout);

  if (ret_line == NULL)
  {
    fprintf(stderr, "error: read_command: malloc failed\n");
    exit(EXIT_FAILURE);
  }

  while ((c = getchar()) != '\n')
  {
    if (c == EOF)
    {
      free(ret_line);
      exit(EXIT_SUCCESS);
    }

    if (i >= buf_size)
      ret_line = realloc(ret_line, buf_size * 2);

    ret_line[i++] = c;
  }

  ret_line[i] = '\0';
  return ret_line;
}

int enable_raw_mode(int fd)
{
  struct termios raw;

  if (!isatty(STDIN_FILENO))
    goto fatal;

  if (tcgetattr(fd, &orig_termios) == -1)
    goto fatal;

  raw = orig_termios;  /* modify the original mode */
  /* input modes: no break, no CR to NL, no parity check, no strip char,
   * no start/stop output control. */
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  /* output modes - disable post processing */
  raw.c_oflag &= ~(OPOST);
  /* control modes - set 8 bit chars */
  raw.c_cflag |= (CS8);
  /* local modes - choing off, canonical off, no extended functions,
   * no signal chars (^Z,^C) */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  /* control chars - set return condition: min number of bytes and timer.
   * We want read to return every single byte, without timeout. */
  raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0; /* 1 byte, no timer */

  /* put terminal in raw mode after flushing */
  if (tcsetattr(fd, TCSAFLUSH, &raw) < 0)
    goto fatal;
  return 0;

fatal:
  return -1;
}

int disable_raw_mode(int fd)
{
  tcsetattr(fd, TCSAFLUSH, &orig_termios);
}

int handle_read_raw(char *buffer, size_t bufferlen, const char *prompt)
{
  if (bufferlen == 0)
    return -1;

  if (enable_raw_mode(STDIN_FILENO) == -1)
    return -1;
  edit(STDIN_FILENO, STDOUT_FILENO, buffer, bufferlen, prompt);
  disable_raw_mode(STDIN_FILENO);
  printf("\n");
}

char *readline(const char* prompt)
{
  char buffer[4096];

  if (!isatty(STDIN_FILENO))
  {
    fprintf(stderr, "error: readline: input not a tty\n");
    exit(EXIT_FAILURE);
  }
  else if (is_unsupported_terminal())
  {
    return handle_unsupported_terminal(prompt);
  }
  else
  {
    int count = handle_read_raw(buffer, 4096, prompt);
    return strdup(buffer);
  }
}
