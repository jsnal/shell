/*
 * Copyright (c) 2021, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef REDIRECT_H
#define REDIRECT_H

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handle_redirect_in(char*);
void handle_redirect_out(char*, int);
void handle_redirect_error(char*, int);
void handle_redirect_all(char*);

#endif
