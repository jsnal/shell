#include "builtin.h"

int handle_cd(struct Command *cmd)
{
  unsigned int cd_ret = 0;

  if (cmd->argc > 2)
  {
    fprintf(stderr, "cd: too many arguments\n");
    return 1;
  }

  if (cmd->argc == 1)
  {
    cd_ret = chdir(SYS_HOME);
  }
  else
    cd_ret = chdir(cmd->argv[1]);


  if (cd_ret != 0)
  {
    switch (errno)
    {
      case ENOENT:
        fprintf(stderr, "cd: no such directory: %s\n", cmd->argv[1]);
        break;
      default:
        fprintf(stderr, "cd: unable to change directory\n");
        break;
    }

    return 1;
  }
  return 0;
}

int handle_exit(struct Command *cmd)
{
  printf("Goodbye!\n");
  return -1;
}

int handle_history(struct Command *cmd)
{
  for (unsigned int i = 0; history->list[i] != NULL; i++)
  {
    printf(" %d  %s\n", i + 1, history->list[i]);
  }

  return 0;
}

static struct Builtin builtin[] = {
  { "cd",      handle_cd },
  { "logout",  handle_exit },
  { "exit",    handle_exit },
  { "history", handle_history }
};

int exec_builtin(struct Builtin *builtin, struct Command *cmd)
{
  return (builtin->func)(cmd);
}

struct Builtin *check_builtin(struct Command *cmd)
{
  struct Builtin *front = builtin;
  struct Builtin *back = builtin + sizeof(builtin)/sizeof(builtin[0]);

  while (front < back)
  {
    if (strcmp(cmd->name, front->name) == 0)
      return front;

    front++;
  }

  return NULL;
}
