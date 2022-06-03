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

static bool is_empty_line(const char *line)
{
  int len = strlen(line);

  if (len < 0) {
    return true;
  }

  for (int i = 0; i < len; i++) {
    if (!isspace(line[i])) {
      return false;
    }
  }

  return true;
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

    resize_buffer_t *b = resize_buffer_create(32);

    while (c != EOF) {
      resize_buffer_append_char(b, c);
      c = fgetc(stdin);
    }

    tokens = tokenize(b->buffer);
    if (args->dump_tokens) {
      tokens_to_string(tokens);
    }
  } else {
    dbgln("Starting shell interactive mode");

    int command_ret = 0, readline_status;
    char prompt[64];
    char *history_line, *line;

    for(;;) {
      snprintf(prompt, 64, "%s$ ", "shell");

      line = readline(prompt, &readline_status);
      if (readline_status == -1)
        return EXIT_FAILURE;

      if (!is_empty_line(line)) {
        tokens = tokenize(line);

        if (!tokens) {
          continue;
        }

        if (args->dump_tokens) {
          tokens_to_string(tokens);
        }

        /* tree_t *tree = parse(tokens); */
        /* if (args->dump_ast) { */
        /*   tree_to_string(tree); */
        /* } */

        /* if (!tree) { */
        /*   continue; */
        /* } */
        /* command_ret = execute(tree); */

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
