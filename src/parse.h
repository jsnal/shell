/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PARSE_H
#define PARSE_H

#include "tokenize.h"
#include "list.h"
#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define ARG_MAX 512

#define TODO \
  warnln("Node type not implemented yet")

enum RedirectType {
  RT_INPUT,    /* <file */
  RT_OUTPUT,   /* >file */
  RT_ERROR,    /* 2>file */
  RT_ALL,      /* &>file */
  RT_CLOBBER,  /* >|file */
  RT_APPEND,   /* >>file */
  RT_INOUT,    /* <>file */
  RT_DUPIN,    /* <&fd */
  RT_DUPOUT,   /* >&fd */
  RT_HERE,     /* <<END */
  RT_PROCIN,   /* <(command) */
  RT_PROCOUT,  /* >(command) */
};

typedef struct RedirectStruct {
  struct Redirect *next;
  enum RedirectType type;
  char *file;
} redirect_t;

enum TerminatorType {
  MT_BACKGROUND,
  MT_NONE,
  MT_SEQUENCE,
};

enum AndOrType {
  AOT_AND,
  AOT_OR,
  AOT_NONE,
};

typedef struct CommandStruct {
  struct CommandStruct *next;
  size_t argc;
  char *argv[ARG_MAX];
  int fds[2];
  pid_t pid;
  enum TerminatorType terminator;
  list_t *redirects;
} command_t;

typedef struct PipelineStruct {
  struct PipelineStruct *next;
  struct CommandStruct *commands;
  enum AndOrType type;
  int pipe_count;
} pipeline_t;

typedef struct AndOrStruct {
  struct AndOrStruct *next;
  struct PipelineStruct *pipelines;
} andor_t;

enum NodeType {
  NT_ANDOR,
  NT_CASE,
  NT_ERROR,
  NT_FUNCTION,
  NT_IF,
  NT_PIPELINE,
  NT_SIMPLE_COMMAND,
  NT_WHILE,
};

struct Node {
  struct Node *next;
  enum NodeType type;
  union {
    command_t *command;
    pipeline_t *pipeline;
    andor_t *andor;
  };
};

struct Tree {
  struct Node *nodes;
};

typedef struct ParseStateStruct {
  struct Token *tokens_list;
  enum NodeType type;
  size_t index;
} parse_state_t;

struct Tree *parse(struct Token*);
void tree_to_string(struct Tree*);
char *terminator_to_string(enum TerminatorType);
void command_to_string(command_t*, int space);
void pipeline_to_string(pipeline_t*, int space);
void andor_to_string(andor_t*);
#endif
