/*
 * Copyright (c) 2021, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "execute.h"
#include "debug.h"
#include <stdbool.h>

static void close_pipes(struct PipelineSummary *ps)
{
  for (int i = 0; i < ps->command_count - 1; i++) {
    close(ps->pfds[i][0]);
    close(ps->pfds[i][1]);
  }
}

static int execute_simple_command(struct Command *command, struct PipelineSummary *ps)
{
  pid_t command_pid = fork();

  if (command_pid == -1) {
    fprintf(stderr, "error: execute_command: fork error\n");
    return -1;
  }

  if (command_pid == 0) {
    if (ps) {
      if (command->fds[0] != STDIN_FILENO) {
        dup2(command->fds[0], STDIN_FILENO);
      }

      if (command->fds[1] != STDOUT_FILENO) {
        dup2(command->fds[1], STDOUT_FILENO);
      }

      close_pipes(ps);
    }

    struct Redirect *current = command->redirects;
    while (current != NULL) {
      switch (current->type) {
        case RT_INPUT:
          handle_redirect_in(current->file);
          break;
        case RT_OUTPUT:
          handle_redirect_out(current->file, 0);
          break;
        case RT_ERROR:
          handle_redirect_error(current->file, 0);
          break;
        case RT_ALL:
          handle_redirect_all(current->file);
          break;
        case RT_APPEND:
          handle_redirect_out(current->file, 1);
          break;
        default:
          break;
      }
      current = current->next;
    }

    execvp(command->argv[0], command->argv);

    errln("%s: %s", strerror(errno), command->argv[0]);
    exit(EXIT_FAILURE);
  }

  return command_pid;
}

int execute_andor(struct AndOr *andor)
{
  printf("andor\n");
}

int execute_pipeline(struct Pipeline *pipeline)
{
  if (pipeline->pipe_count == 0) {
    execute_simple_command(pipeline->commands, NULL);
    return 0;
  }

  struct PipelineSummary ps = {
    .command_count = pipeline->pipe_count + 1,
    .pfds = malloc(sizeof(int[2]) * pipeline->pipe_count)
  };

  for (int i = 0; i < pipeline->pipe_count; i++) {
    if (pipe(ps.pfds[i]) == -1) {
      errln("Unable to create pipes\n");
      exit(EXIT_FAILURE);
    }
  }

  struct Command *current = pipeline->commands;
  current->fds[STDIN_FILENO] = STDIN_FILENO;

  for (int i = 0; i < ps.command_count; i++) {
    if (i == 0) {
      current->fds[STDOUT_FILENO] = ps.pfds[i][STDOUT_FILENO];
    } else if (i == ps.command_count - 1) {
      current->fds[STDIN_FILENO] = ps.pfds[i - 1][STDIN_FILENO];
    } else {
      current->fds[STDIN_FILENO] = ps.pfds[i - 1][STDIN_FILENO];
      current->fds[STDOUT_FILENO] = ps.pfds[i][STDOUT_FILENO];
    }

    if (current->next) {
      current = current->next;
    }
  }
  current->fds[STDOUT_FILENO] = STDOUT_FILENO;

  for (current = pipeline->commands; current != NULL; current = current->next) {
    if (execute_simple_command(current, &ps) < 0) {
      errln("Problem running command %s\n", current->argv[0]);
    }
  }

  close_pipes(&ps);

  for (current = pipeline->commands; current != NULL; current = current->next) {
    wait(NULL);
  }

  return EXIT_SUCCESS;
}

int execute(struct Tree *tree)
{
  struct Node *current = tree->nodes;

  while (current != NULL)
  {
    switch (current->type)
    {
      case NT_ANDOR:
        execute_andor(current->andor);
        break;
      case NT_PIPELINE:
        execute_pipeline(current->pipeline);
        break;
      case NT_SIMPLE_COMMAND:
        execute_simple_command(current->command, NULL);
        wait(NULL);
        break;
      default:
        printf("Node not implemented!\n");
        return 1;
    }
    current = current->next;
  }
}
