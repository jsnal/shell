#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// If there are more than 256 tokens then your command is too long...
#define TEXT_MAX 256
#define TOKENS_SIZE 17

#define SET_TYPE(type) \
  ts->tokenType = (type)

#define SET_TYPE_INC(type) \
  SET_TYPE(type); \
  index++

#define TO_STRING(type, text) \
  case type: \
    return text; \

enum TokenType {
    TT_UNKNOWN,
    TT_END_OF_INPUT,
    TT_TEXT,

    /* Operators */
    TT_NEW_LINE,
    TT_AMP, TT_AMPAMP, TT_LPAREN, TT_RPAREN, TT_SEMICOLON, TT_DOUBLE_SEMICOLON,
    TT_PIPE, TT_PIPEPIPE, TT_LESS, TT_LESSLESS, TT_LESSAMP,TT_LESSGREATER,
    TT_LESSLPAREN, TT_GREATER,TT_GREATERGREATER, TT_GREATERPIPE, TT_GREATERAMP,
    TT_GREATERLPAREN, TT_ONEGREATER, TT_TWOGREATER, TT_AMPGREATER,

    /* Reserved Words */
    TT_IF, TT_THEN, TT_ELSE, TT_ELIF, TT_FI, TT_DO, TT_DONE, TT_CASE, TT_ESAC,
    TT_WHILE, TT_UNTIL, TT_FOR, TT_LBRACE, TT_RBRACE, TT_BANG, TT_IN,
    TT_FUNCTION,
};

enum TextUnitType {
  TUT_STRING,
  TUT_PARAM,
  TUT_CMDSUB,
  TUT_ARITH,
};

struct SourceBuffer {
  char *contents;
  size_t length;
};

struct TokenState {
  int error;
  enum TokenType tokenType;
  char text[TEXT_MAX];
  struct SourceBuffer src;
  size_t index;
  size_t next_index;
};

struct Token {
  struct Token *next, *prev;
  enum TokenType tokenType;
  char *text;
};

void next_token(struct TokenState*);
struct Token *tokenize(char*);
void tokens_to_string(struct Token*);
void cleanup_token_list(struct Token*);
#endif
