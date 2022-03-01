/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "shell.h"
#include <getopt.h>

static struct option const longopts[] = {
  { "dump-ast", no_argument, NULL, 'a' },
  { "dump-tokens", no_argument, NULL, 't' },
  { NULL, 0, NULL, 0 }
};

void usage()
{
  printf("Usage: \n");
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  shell_arguments_t args = {
    .dump_ast = false,
    .dump_tokens = false,
  };

  for (;;) {
    char arg = getopt_long(argc, argv, "+at", longopts, NULL);

    if (arg == -1) {
      break;
    }

    switch (arg) {
      case 'a':
        args.dump_ast = true;
        break;
      case 't':
        args.dump_tokens = true;
        break;
      default:
        usage();
        break;
    }
  }

  return shell(&args);
}
