/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

int vdbgln(const char *fmt, ...)
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "[%H:%M:%S]", tm_info);

    va_list args;
    fprintf(stderr, BLUE BOLD "%s " RESET, buffer);
    va_start(args, fmt);
    int ret = vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    return ret;
}

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

