#include "exec.h"

int exec_command(struct Command *cmd)
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

    if (output_fd != -1 && output_fd != STDIN_FILENO)
      dup2(output_fd, STDIN_FILENO);

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
    exec_ret = exec_command(cmds->list[0]);
    wait(NULL);
  }
  else
  {
    printf("has pipes!\n");
  }

  return exec_ret;
}
