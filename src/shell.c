/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "debug.h"
#include "shell.h"
#include "variable.h"

#include "list.h"
#include "util.h"

char *line;

void exit_clean(int code)
{
  exit(code);
}

int is_empty_line(char *line)
{
  int len = strlen(line);

  if (len < 0)
    return 1;

  for (unsigned int i = 0; i < len; i++)
  {
    if (!isspace(line[i]))
      return 0;
  }

  return 1;
}

void initialize_system_environment_variables()
{
  /* char *cwd; */
  /* if ((cwd = getcwd(NULL, 0)) != NULL) */
  /*   setenv("PWD", cwd, 1); */
  /* free(cwd); */
  /*  */
  /* if ((SYS_HOME = getenv("HOME")) == NULL) */
  /*   SYS_HOME = getpwuid(getuid())->pw_dir; */
  /*  */
  /* setenv("OLDPWD", SYS_HOME, 1); */
}

void set_system_environment_variables()
{
  /* if ((SYS_PATH = getenv("PATH")) == NULL) */
  /*   SYS_PATH = "/bin"; */
  /*  */
  /* PWD = getenv("PWD"); */
  /* OLDPWD = getenv("OLDPWD"); */
}

char *read_line(void)
{
  int buf_size = 128;
  char *ret_line = malloc(buf_size * sizeof(char));
  unsigned int i = 0;
  char c;

  if (ret_line == NULL)
  {
    fprintf(stderr, "error: read_command: malloc failed\n");
    exit_clean(EXIT_FAILURE);
  }

  while ((c = getchar()) != '\n')
  {
    if (c == EOF)
    {
      free(ret_line);
      exit_clean(EXIT_SUCCESS);
    }

    if (i >= buf_size)
      ret_line = realloc(ret_line, buf_size * 2);

    ret_line[i++] = c;
  }

  ret_line[i] = '\0';
  return ret_line;
}

int shell(const shell_arguments_t *args)
{
  variable_scope_t *main_scope;
  if ((main_scope = initialize_main_scope()) == NULL) {
    errln("Failed to initialize main scope");
  }

  list_t *tokens = NULL;

  if (!isatty(STDIN_FILENO)) {
    dbgln("Running shell in script mode");

    char c;
    if ((c = fgetc(stdin)) == EOF) {
      return 0;
    }

    int len = 0, capacity = 5;
    char *buf = (char*) malloc(sizeof(char) * capacity + 1);

    while (c != EOF) {
      if (len >= capacity) {
        capacity *= 2;
        buf = (char*) realloc(buf, capacity * sizeof(char) + 1);
      }

      buf[len++] = c;
      c = fgetc(stdin);
    }
    buf[len] = '\0';

    tokens = tokenize(buf);
    if (args->dump_tokens) {
      tokens_to_string(tokens);
    }
  } else {
    dbgln("Starting shell interactive mode");

    int command_ret = 0, readline_status;
    char prompt[64];
    char *history_line;

    for(;;) {
      snprintf(prompt, 64, "%s$ ", "shell");

      line = readline(prompt, &readline_status);
      if (readline_status == -1)
        return EXIT_FAILURE;

      if (!is_empty_line(line)) {
        tokens = tokenize(line);
        if (args->dump_tokens) {
          tokens_to_string(tokens);
        }

        //      tree_t *tree = parse(tokens_list);
        //      if (print_ast)
        //        tree_to_string(tree);

        //      if (!tree) {
        //        continue;
        //      }
        //      command_ret = execute(tree);

        /* history_line = strdup(line); */
        /*       if (parse_line(line) == -1) */
        /*         goto no_execute; */
        /*  */
        /*       command_ret = execute_commands(command); */
        /*       cleanup_commands(command); */
        /*  */
        /* no_execute: */
        /*       push_history(history_line); */
        /*       free(history_line); */
      }

      cleanup_token_list(tokens);
      free(line);
    }
  }

  return EXIT_SUCCESS;
}
