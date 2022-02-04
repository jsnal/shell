/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "debug.h"
#include <stdarg.h>
#include <stdio.h>

int errln(const char *fmt, ...)
{
    va_list args;
    fprintf(stderr, RED BOLD "shell: " RESET);
    va_start(args, fmt);
    int ret = vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    return ret;
}

int warnln(const char *fmt, ...)
{
    va_list args;
    fprintf(stdout, YELLOW BOLD "shell: " RESET);
    va_start(args, fmt);
    int ret = vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "\n");

    return ret;
}

