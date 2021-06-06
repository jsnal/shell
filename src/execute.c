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
