#ifndef PARSE_H
#define PARSE_H

#include "tokenize.h"
#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define ARG_MAX 512

#define TODO \
  printf("Node type not implemented yet.\n"); \
  return NULL

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

struct Redirect {
  struct Redirect *next;
  enum RedirectType type;
  char *file;
};

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

// TODO: rename this after I delete the old one
struct Cmd {
  struct Cmd *next;
  size_t argc;
  char *argv[ARG_MAX];
  enum TerminatorType terminator_type;
  struct Redirect *redirects;
};

struct Pipeline {
  struct Pipeline *next;
  struct Cmd *commands;
  enum AndOrType andor_type;
};

struct AndOr {
  struct AndOr *next;
  struct Pipeline *pipelines;
};

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
  enum NodeType node_type;
  union {
    struct Cmd *command;
    struct Pipeline *pipeline;
    struct AndOr *andor;
  };
};

struct Tree {
  struct Node *nodes;
};

struct ParseState {
  struct Token *tokens_list;
  enum NodeType node_type;
  size_t index;
};

struct Tree *parse(struct Token*);
void tree_to_string(struct Tree*);
char *terminator_to_string(enum TerminatorType);
void command_to_string(struct Cmd*, int space);
void pipeline_to_string(struct Pipeline*, int space);
void andor_to_string(struct AndOr*);
#endif
