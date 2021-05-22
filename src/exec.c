#include "exec.h"

void close_pipes(int (*pipes)[2], int count)
{
  for (unsigned int i = 0; i < count; i++)
  {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }
}

int exec_command(struct Commands* cmds, struct Command *cmd, int (*pipes)[2])
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
      close_pipes(pipes, cmds->count - 1);

    execvp(cmd->name, cmd->argv);

    switch(errno)
    {
      case ENOENT:
        fprintf(stderr, "shell: command not found: %s\n", cmd->name);
        break;
      case EACCES:
        fprintf(stderr, "shell: permission denied: %s\n", cmd->name);
        break;
      default:
        fprintf(stderr, "shell: %s\n", strerror(errno));
        break;
    }

    cleanup_commands(commands);
    _exit(EXIT_FAILURE);
  }

  return cmd_pid;
}

int exec_commands(struct Commands *cmds)
{
  unsigned int exec_ret = 0;

  if (cmds->count == 1)
  {
    cmds->list[0]->fds[STDIN_FILENO] = STDIN_FILENO;
    cmds->list[0]->fds[STDOUT_FILENO] = STDOUT_FILENO;
    exec_ret = exec_command(cmds, cmds->list[0], NULL);
    wait(NULL);
  }
  else
  {
    unsigned int pipe_count = cmds->count - 1;

    struct Builtin *builtin;
    for (unsigned int i = 0; i < cmds->count; i++)
      if ((builtin = check_builtin(cmds->list[i])) != NULL)
        fprintf(stderr, "error: exec_commands: built-ins in pipeline not supported.\n");

    int (*pipes)[2] = calloc(pipe_count * sizeof(int[2]), 1);

    if (pipes == NULL)
    {
      fprintf(stderr, "error: exec_commands: calloc failed\n");
      return 0;
    }

    cmds->list[0]->fds[STDIN_FILENO] = STDIN_FILENO;
    for (unsigned int i = 1; i < cmds->count; i++)
    {
      if (pipe(pipes[i - 1]) == -1)
      {
        fprintf(stderr, "error: exec_commands: creating pipe failed.\n");
        cleanup_commands(commands);
        close_pipes(pipes, pipe_count);
        free(pipes);
        exit(EXIT_FAILURE);
      }

      cmds->list[i - 1]->fds[STDOUT_FILENO] = pipes[i - 1][1];
      cmds->list[i]->fds[STDIN_FILENO] = pipes[i - 1][0];
    }
    cmds->list[pipe_count]->fds[STDOUT_FILENO] = STDOUT_FILENO;

    for (unsigned int i = 0; i < cmds->count; i++)
      exec_ret = exec_command(cmds, cmds->list[i], pipes);

    close_pipes(pipes, pipe_count);

    for (unsigned int i = 0; i < cmds->count; ++i)
      wait(NULL);

    free(pipes);
  }

  return exec_ret;
}
