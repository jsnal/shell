/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "util.h"

// If there are more than 256 tokens then your command is too long...
#define TEXT_MAX 256
#define DEFAULT_WORD_LEN 32

#define SET_TYPE(token_type) \
  ts->type = (token_type)

#define SET_TYPE_INC(type) \
  SET_TYPE(type); \
  index++

#define TO_STRING(type, text) \
  case type: \
    return text; \

typedef enum TokenTypeEnum {
  TT_UNKNOWN,
  TT_END_OF_INPUT,
  TT_WORD,
  TT_TEXT,
  TT_NEW_LINE,
  TT_IO_NUMBER,

  /* Operators */
  TT_EQUAL,
  TT_AMP, TT_AMPAMP, TT_LPAREN, TT_RPAREN, TT_SEMICOLON, TT_DOUBLE_SEMICOLON, TT_PIPE, TT_PIPEPIPE,
  TT_LESS, TT_LESSLESS, TT_LESSLESSDASH, TT_LESSLESSLESS,  TT_LESSAMP, TT_LESSGREATER, TT_LESSLPAREN,
  TT_GREATER, TT_GREATERGREATER, TT_GREATERPIPE, TT_GREATERAMP, TT_GREATERLPAREN,
  TT_AMPGREATER,

  /* Reserved Words */
  TT_IF, TT_THEN, TT_ELSE, TT_ELIF, TT_FI, TT_DO, TT_DONE, TT_CASE, TT_ESAC,
  TT_WHILE, TT_UNTIL, TT_FOR, TT_LBRACE, TT_RBRACE, TT_BANG, TT_IN,
  TT_FUNCTION,
} token_type_e;

// TODO: Add "", '', and other string types
//enum TextUnitType {
//  TUT_STRING,
//  TUT_PARAM,
//  TUT_CMDSUB,
//  TUT_ARITH,
//};

typedef struct TokenState {
  int error;
  int next_index;
  token_type_e type;
  resize_buffer_t src;
  union {
    resize_buffer_t *word;
    char text[TEXT_MAX];
    unsigned int io_number;
  };
} token_state_t;

typedef struct Token {
  token_type_e type;
  union {
    char *text;
    unsigned int io_number;
  };
} token_t;

list_t *tokenize(char*);
void tokens_to_string(list_t *const tokens);
void cleanup_token_list(list_t *tokens);
#endif
