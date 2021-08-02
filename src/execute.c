/*
 * Copyright (c) 2021, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "execute.h"

void close_pipes(int (*pipes)[2], int count)
{
  for (unsigned int i = 0; i < count; i++)
  {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }
}

int execute_command(struct Command *cmd, int pipe_count, int (*pipes)[2])
{
  struct Builtin *builtin;
  if ((builtin = check_builtin(cmd)) != NULL)
    return exec_builtin(builtin, cmd);

  pid_t cmd_pid = fork();

  if (cmd_pid == -1)
  {
    fprintf(stderr, "error: exec_command: fork error\n");
    return -1;
  }

  if (cmd_pid == 0)
  {
    int input_fd = cmd->fds[0];
    int output_fd = cmd->fds[1];

    if (input_fd != -1 && input_fd != STDIN_FILENO)
      dup2(input_fd, STDIN_FILENO);

    if (output_fd != -1 && output_fd != STDOUT_FILENO)
      dup2(output_fd, STDOUT_FILENO);

    if (pipes != NULL)
      close_pipes(pipes, pipe_count);

    for (unsigned int operator = 0; operator < OPERATORS_SIZE; operator++)
    {
      /* printf("type: %d to %s\n", operator, cmd->redirects[operator]); */
      if (cmd->redirects[operator] != NULL)
      {
        switch (operator)
        {
          case REDIRECT_OUT:
            handle_redirect_out(cmd->redirects[operator], 0);
            break;
          case REDIRECT_OUT_APPEND:
            handle_redirect_out(cmd->redirects[operator], 1);
            break;
          case REDIRECT_IN:
            handle_redirect_in(cmd->redirects[operator]);
            break;
          case REDIRECT_ERROR:
            handle_redirect_error(cmd->redirects[operator], 0);
            break;
          case REDIRECT_ERROR_APPEND:
            handle_redirect_error(cmd->redirects[operator], 1);
            break;
          case REDIRECT_ALL:
            handle_redirect_all(cmd->redirects[operator]);
            break;
        }
      }
    }

    execvp(cmd->name, cmd->argv);

    switch(errno)
    {
      case ENOENT:
        fprintf(stderr, "error: exec_command: command not found: %s\n", cmd->name);
        break;
      case EACCES:
        fprintf(stderr, "error: exec_command: permission denied: %s\n", cmd->name);
        break;
      default:
        fprintf(stderr, "error: exec_command: %s\n", strerror(errno));
        break;
    }

    cleanup_commands(command);
    _exit(EXIT_FAILURE);
  }

  return cmd_pid;
}

int execute_commands(struct Command *cmd)
{
  struct Command *c;
  unsigned int exec_ret = 0;
  unsigned int command_count = 0;

  c = command;
  while (c != NULL)
  {
    command_count++;
    c = c->next;
  }

  if (command_count == 1)
  {
    command->fds[STDIN_FILENO] = STDIN_FILENO;
    command->fds[STDOUT_FILENO] = STDOUT_FILENO;
    exec_ret = execute_command(command, 0, NULL);
    wait(NULL);
  }
  else
  {
    unsigned int pipe_count = command_count - 1, counter = 1;

    struct Builtin *builtin;
    for (c = command; c != NULL; c = c->next)
      if ((builtin = check_builtin(c)) != NULL)
        fprintf(stderr, "error: exec_commands: built-ins in pipeline not supported.\n");

    int (*pipes)[2] = calloc(pipe_count * sizeof(int[2]), 1);

    if (pipes == NULL)
    {
      fprintf(stderr, "error: exec_commands: calloc failed\n");
      return 0;
    }

    struct Command *last = command;
    command->fds[STDIN_FILENO] = STDIN_FILENO;
    for (c = command->next; c != NULL; c = c->next)
    {
      if (pipe(pipes[counter - 1]) == -1)
      {
        fprintf(stderr, "error: exec_commands: creating pipe failed.\n");
        cleanup_commands(command);
        close_pipes(pipes, pipe_count);
        free(pipes);
        exit(EXIT_FAILURE);
      }

      c->prev->fds[STDOUT_FILENO] = pipes[counter - 1][1];
      c->fds[STDIN_FILENO] = pipes[counter - 1][0];
      counter++;
      last = c;
    }
    last->fds[STDOUT_FILENO] = STDOUT_FILENO;

    for (c = command; c != NULL; c = c->next)
      exec_ret = execute_command(c, pipe_count, pipes);

    close_pipes(pipes, pipe_count);

    for (c = command; c != NULL; c = c->next)
      wait(NULL);

    free(pipes);
  }

  return exec_ret;
}

int execute_simple_command(struct Cmd *command, int pipe_count, int (*pipes)[2])
{
  pid_t command_pid = fork();

  if (command_pid == -1)
  {
    fprintf(stderr, "error: execute_command: fork error\n");
    return -1;
  }

  if (command_pid == 0)
  {
    printf("Executing command\n");

    int input_fd = command->fds[0];
    int output_fd = command->fds[1];

    if (input_fd != -1 && input_fd != STDIN_FILENO)
      dup2(input_fd, STDIN_FILENO);

    if (output_fd != -1 && output_fd != STDOUT_FILENO)
      dup2(output_fd, STDOUT_FILENO);

    if (pipes != NULL)
      close_pipes(pipes, pipe_count);

    struct Redirect *current = command->redirects;
    while (current != NULL)
    {
      switch (current->type)
      {
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

    switch(errno)
    {
      case ENOENT:
        fprintf(stderr, "error: exec_command: command not found: %s\n", command->argv[0]);
        break;
      case EACCES:
        fprintf(stderr, "error: exec_command: permission denied: %s\n", command->argv[0]);
        break;
      default:
        fprintf(stderr, "error: exec_command: %s\n", strerror(errno));
        break;
    }

    _exit(EXIT_FAILURE);
  }

  return command_pid;
}

int execute_andor(struct AndOr *andor)
{
  printf("andor\n");
}

int execute_pipeline(struct Pipeline *pipeline)
{
  if (pipeline->pipe_count == 0)
  {
    execute_simple_command(pipeline->commands, 0, NULL);
    wait(NULL);
  }

  struct Cmd *current = pipeline->commands;
  int (*pipes)[2] = calloc(pipeline->pipe_count * sizeof(int[2]), 1);
  int counter = 0, execute_return;

  if (pipes == NULL)
  {
    fprintf(stderr, "error: exec_commands: calloc failed\n");
    return 0;
  }

  struct Cmd *last = pipeline->commands;
  pipeline->commands->fds[STDIN_FILENO] = STDIN_FILENO;
  while (current != NULL)
  {
    if (pipe(pipes[counter - 1]) == -1)
    {
      fprintf(stderr, "error: exec_commands: creating pipe failed.\n");
      close_pipes(pipes, pipeline->pipe_count);
      free(pipes);
      exit(EXIT_FAILURE);
    }

    if (current->next != NULL)
      current->next->fds[STDIN_FILENO] = pipes[counter - 1][0];

    current->fds[STDOUT_FILENO] = pipes[counter - 1][1];
    counter++;
    last = current;
    current = current->next;
  }
  last->fds[STDOUT_FILENO] = STDOUT_FILENO;

  current = pipeline->commands;
  while (current != NULL)
  {
    execute_return = execute_simple_command(current, pipeline->pipe_count, pipes);
    current = current->next;
  }

  close_pipes(pipes, pipeline->pipe_count);

  current = pipeline->commands;
  while (current != NULL)
  {
    wait(NULL);
    current = current->next;
  }
  free(pipes);

  return execute_return;
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
        execute_simple_command(current->command, 0, NULL);
        wait(NULL);
        break;
      default:
        printf("Node not implemented!\n");
        return 1;
    }
    current = current->next;
  }
}
