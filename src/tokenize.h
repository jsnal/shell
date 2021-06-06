#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// If there are more than 256 tokens then your command is too long...
#define TOKENS_LIMIT 256

#define TO(x)  \
  state = (x); \
  break

#define COMMIT(m_type) \
  if (data[0] != '\0') \
{ \
  tokens[tokenPosition].data = strdup(data); \
  tokens[tokenPosition].type = (m_type); \
  memset(data, '\0', 128); \
  dataPosition = 0; \
  tokenPosition++; \
}

#define CONSUME() data[dataPosition++] = line[i]

#define COMMIT_AND_CONSUME(m_type) \
  COMMIT(m_type) \
  CONSUME()

#define RETURN_START() i--; TO(Text)

enum TokenState {
  And,
  Background,
  Comment,
  Empty,
  Or,
  Pipe,
  RedirectAll,
  RedirectError,
  RedirectErrorAppend,
  RedirectIn,
  RedirectInAppend,
  RedirectOut,
  RedirectOutAppend,
  Semicolon,
  Text,
  TextLiteral,
  Tilde,
};

struct Token {
  char *data;
  enum TokenState type;
};

enum TokenState state;
struct Token *tokenize_line(char*);
void cleanup_tokens(struct Token*);
#endif
