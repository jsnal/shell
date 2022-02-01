#ifndef DEBUG_H
#define DEBUG_H

#define RED   "\x1B[31m"
#define GREEN "\x1B[32m"
#define BOLD  "\x1B[1m"
#define RESET "\x1B[0m"

int errln(const char *fmt, ...);

#endif
