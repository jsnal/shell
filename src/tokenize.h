#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// If there are more than 256 tokens then your command is too long...
#define TOKENS_LIMIT 256
#define TOKENS_SIZE 17

#define SET_TYPE(type) \
  ts->tokenType = (type)

#define SET_TYPE_INC(type) \
  SET_TYPE(type); \
  index++

#define TO_STRING(type, text) \
  case type: \
    printf("[%s]\n", text); \
    break

enum TokenType {
    TT_UNKNOWN,
    TT_END_OF_INPUT,
    TT_TEXT,
    /* operators */
    TT_NEW_LINE,
    TT_AMP, TT_AMPAMP, TT_LPAREN, TT_RPAREN, TT_SEMICOLON, TT_DOUBLE_SEMICOLON,
    TT_PIPE, TT_PIPEPIPE, TT_LESS, TT_LESSLESS, TT_LESSAMP,TT_LESSGREATER,
    TT_LESSLPAREN, TT_GREATER,TT_GREATERGREATER, TT_GREATERPIPE, TT_GREATERAMP,
    TT_GREATERLPAREN,
    /* reserved words */
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
  char text[TOKENS_LIMIT];
  struct SourceBuffer src;
  size_t index;
  size_t next_index;
};

struct Token {
  enum TokenType tokenType;
  char *text;
};

void next_token(struct TokenState*);
int tokenize(char*, struct Token**, size_t*);
void tokenize_to_string(struct Token**, size_t);
void cleanup_token_list(struct Token**, int);
#endif
