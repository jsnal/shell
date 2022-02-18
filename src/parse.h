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

typedef enum RedirectTypeEnum {
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
} redirect_type_e;

typedef struct RedirectStruct {
  struct RedirectStruct *next;
  redirect_type_e type;
  char *file;
} redirect_t;

typedef enum TerminatorTypeEnum {
  MT_BACKGROUND,
  MT_NONE,
  MT_SEQUENCE,
} terminator_type_e;

typedef enum AndOrTypeEnum {
  AOT_AND,
  AOT_OR,
  AOT_NONE,
} andor_type_e;

typedef struct CommandStruct {
  struct CommandStruct *next;
  size_t argc;
  char *argv[ARG_MAX];
  int fds[2];
  pid_t pid;
  terminator_type_e terminator;
  list_t *redirects;
} command_t;

typedef struct PipelineStruct {
  struct PipelineStruct *next;
  command_t *commands;
  andor_type_e type;
  int pipe_count;
} pipeline_t;

typedef struct AndOrStruct {
  struct AndOrStruct *next;
  pipeline_t *pipelines;
} andor_t;

typedef enum NodeTypeEnum {
  NT_ANDOR,
  NT_CASE,
  NT_ERROR,
  NT_FUNCTION,
  NT_IF,
  NT_PIPELINE,
  NT_SIMPLE_COMMAND,
  NT_WHILE,
} node_type_e;

typedef struct NodeStruct {
  struct NodeStruct *next;
  node_type_e type;
  union {
    command_t *command;
    pipeline_t *pipeline;
    andor_t *andor;
  };
} node_t;

typedef struct TreeStruct {
  node_t *nodes;
} tree_t;

typedef struct ParseStateStruct {
  struct Token *tokens_list;
  node_type_e type;
  size_t index;
} parse_state_t;

tree_t *parse(struct Token*);
void tree_to_string(tree_t*);
char *terminator_to_string(terminator_type_e);
void command_to_string(command_t*, int space);
void pipeline_to_string(pipeline_t*, int space);
void andor_to_string(andor_t*);
#endif
