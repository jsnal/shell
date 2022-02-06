/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef DEBUG_H
#define DEBUG_H

#define RED   "\x1B[31m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define BOLD  "\x1B[1m"
#define RESET "\x1B[0m"

int errln(const char *fmt, ...);
int warnln(const char *fmt, ...);
int vdbgln(const char *fmt, ...);

#ifndef DEBUG
#define dbgln(fmt, args...) vdbgln(fmt, ## args)
#else
#define dbgln(fmt, args...)
#endif

#endif
