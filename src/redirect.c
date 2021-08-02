/*
 * Copyright (c) 2021, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "redirect.h"

void handle_redirect_in(char *file)
{
  int fd_in = open(file, O_RDONLY);

  if (fd_in == -1)
  {
    fprintf(stderr, "error: handle_redirect_in: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  dup2(fd_in, STDIN_FILENO);
}

void handle_redirect_out(char *file, int append)
{
  int fd_out;
  if (append)
    fd_out = open(file, O_CREAT | O_APPEND | O_WRONLY, 0644);
  else
    fd_out = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);

  if (fd_out == -1)
  {
    fprintf(stderr, "error: handle_redirect_out: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  dup2(fd_out, STDOUT_FILENO);
}

void handle_redirect_error(char *file, int append)
{
  int fd_out;
  if (append)
    fd_out = open(file, O_CREAT | O_APPEND | O_WRONLY, 0644);
  else
    fd_out = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);

  if (fd_out == -1)
  {
    fprintf(stderr, "error: handle_redirect_error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  dup2(fd_out, STDERR_FILENO);
}

void handle_redirect_all(char *file)
{
  int fd_out = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);

  if (fd_out == -1)
  {
    fprintf(stderr, "error: handle_redirect_all: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  dup2(fd_out, STDOUT_FILENO);
  dup2(fd_out, STDERR_FILENO);
}
