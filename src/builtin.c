/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "builtin.h"
#include "debug.h"
#include "history.h"
#include "shell.h"

static int handle_cd(command_t *command)
{
  unsigned int cd_ret = 0;
  char *cwd;

  if (command->argc > 2)
  {
    fprintf(stderr, "cd: too many arguments\n");
    return 1;
  }

  if (command->argc == 1) {
    /* cd_ret = chdir(SYS_HOME); */
  } else if (strcmp(command->argv[1], "-") == 0) {
    /* if (OLDPWD == NULL) */
    /*   printf("cd: no last working directory"); */
    /*  */
    /* cd_ret = chdir(OLDPWD); */
    /* printf("%s\n", OLDPWD); */
  }
  else
    cd_ret = chdir(command->argv[1]);


  if (cd_ret != 0) {
    errln("%s: %s", strerror(errno), command->argv[1]);
    return 1;
  }

  /* setenv("OLDPWD", getenv("PWD"), 1); */
  /* if ((cwd = getcwd(NULL, 0)) != NULL) */
  /*   setenv("PWD", cwd, 1); */
  /* free(cwd); */

  return 0;
}

static int handle_exit(command_t *command)
{
  printf("Goodbye!\n");
  return EXIT_SUCCESS;
}

static int handle_history(command_t *command)
{
  for (unsigned int i = 0; history->list[i] != NULL; i++) {
    printf(" %d  %s\n", i + 1, history->list[i]);
  }

  return 0;
}

static builtin_t builtin[] = {
  { "cd",      handle_cd },
  { "logout",  handle_exit },
  { "exit",    handle_exit },
  { "history", handle_history }
};

int exec_builtin(builtin_t *builtin, command_t *command)
{
  return (builtin->func)(command);
}

builtin_t *check_builtin(command_t *command)
{
  builtin_t *front = builtin;
  builtin_t *back = builtin + sizeof(builtin) / sizeof(builtin[0]);

  while (front < back) {
    if (strcmp(command->argv[0], front->name) == 0) {
      return front;
    }

    front++;
  }

  return NULL;
}
