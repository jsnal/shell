#include "tester_asserts.h"
#include <stdarg.h>
#include <stdio.h>

int fail(const char *fmt, ...)
{
    va_list args;
    fprintf(stdout, RED BOLD "FAIL: " RESET);
    va_start(args, fmt);
    int ret = vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "\n");

    return ret;
}

int pass()
{
    fprintf(stdout, GREEN BOLD "PASS" RESET);
    fprintf(stdout, "\n");
    return 0;
}
