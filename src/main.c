/*
 * Copyright (c) 2021, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "shell.h"
#include <getopt.h>

static struct option const longopts[] =
{
  {"print-ast", no_argument, NULL, 'a'},
  {"print-tokens", no_argument, NULL, 't'},
  {NULL, 0, NULL, 0}
};

void usage()
{
  printf("Usage: \n");
  exit(0);
}

int main(int argc, char *argv[])
{
  int print_ast = 0;
  int print_tokens = 0;

  for (;;)
  {
    char arg = getopt_long(argc, argv, "+at", longopts, NULL);

    if (arg == -1)
      break;

    switch (arg)
    {
      case 'a':
        print_ast = 1;
        break;
      case 't':
        print_tokens = 1;
        break;
      default:
        usage();
        break;
    }
  }

  return shell(print_ast, print_tokens);
}
