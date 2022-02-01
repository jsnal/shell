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

