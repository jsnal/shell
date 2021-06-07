#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// If there are more than 256 tokens then your command is too long...
#define TOKENS_LIMIT 256
#define TOKENS_SIZE 17

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

#define RETURN_START() i--; TO(s_Text)

enum TokenState {
  s_And,
  s_Background,
  s_Comment,
  s_Empty,
  s_Or,
  s_Pipe,
  s_RedirectAll,
  s_RedirectError,
  s_RedirectErrorAppend,
  s_RedirectIn,
  s_RedirectInAppend,
  s_RedirectOut,
  s_RedirectOutAppend,
  s_Semicolon,
  s_Text,
  s_TextLiteral,
  s_Tilde,
};

struct Token {
  char *data;
  enum TokenState type;
};

enum TokenState state;
struct Token *tokenize_line(char*);
void cleanup_tokens(struct Token*);
#endif
