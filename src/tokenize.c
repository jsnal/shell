/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "tokenize.h"
#include "util.h"
#include "list.h"

#define RESERVED_SYMBOLS_SIZE 12

static char reserved_symbols[RESERVED_SYMBOLS_SIZE] = {
  '(', ')', '{', '}', '!', ';', '&', '|', '>', '<', '#', '='
};

void cleanup_token_list(list_t *tokens)
{
  token_t *t = NULL;
  int size = list_size(tokens);
  for (int i = 0; i < size; i++) {
    t = (token_t*) list_remove(tokens, 0);
    if (t->type == TT_TEXT) {
      free(t->text);
    }
    free(t);
  }

  list_destroy(tokens);
}

token_type_e tokenize_reserved_words(char *t)
{
  token_type_e ret = TT_TEXT;

  switch (t[0])
  {
    case 'c':
      if (t[1] == L'a' && t[2] == L's' && t[3] == L'e' && t[4]== L'\0')
        ret = TT_CASE;
      break;
    case 'd':
      if (t[1] == 'o')
      {
        if (t[2] == '\0')
          ret = TT_DO;

        if (t[2] == 'n' && t[3] == 'e' && t[4] == '\0')
          ret = TT_DONE;
      }
      break;
    case 'e':
      if (t[1] == 'l')
      {
        if (t[2] == 'i' && t[3] == 'f' && t[4] == '\0')
          ret = TT_ELIF;

        if (t[2] == 's' && t[3] == 'e' && t[4] == '\0')
          ret = TT_ELSE;
      }
      if (t[1] == 's' && t[2] == 'a' && t[3] == 'c' && t[4] == '\0')
        ret = TT_ESAC;
      break;
    case 'f':
      if (t[1] == 'i' && t[2] == '\0')
        ret = TT_FI;

      if (t[1] == 'o' && t[2] == 'r' && t[3] == '\0')
        ret = TT_FOR;

      if (t[1] == 'u' && t[2] == 'n' && t[3] == 'c' && t[4] == 't' &&
          t[5] == 'i' && t[6] == 'o' && t[7] == 'n' && t[8] == '\0')
          ret = TT_FUNCTION;
      break;
    case 'i':
      if (t[1] == 'f' && t[2] == '\0')
        ret = TT_IF;

      if (t[1] == 'n' && t[2] == '\0')
        ret = TT_IN;
      break;
    case 't':
      if (t[1] == 'h' && t[2] == 'e' && t[3] == 'n' && t[4] == '\0')
        ret = TT_THEN;
      break;
    case 'u':
      if (t[1] == 'n' && t[2] == 't' && t[3] == 'i' && t[4] == 'l' &&
          t[5] == '\0')
        ret = TT_UNTIL;
      break;
    case 'w':
      if (t[1] == 'h' && t[2] == 'i' && t[3] == 'l' && t[4] == 'e' &&
          t[5] == '\0')
        ret = TT_WHILE;
      break;
    default:
      ret = TT_TEXT;
      break;
  }

  free(t);
  return ret;
}

int is_reserved_text(const char c)
{
  for (int i = 0; i < RESERVED_SYMBOLS_SIZE; i++) {
    if (c == reserved_symbols[i]) {
      return 1;
    }
  }

  return 0;
}

int tokenize_text(struct TokenState *ts, size_t index)
{
  int single_quote = 0;

  if (ts->src.buffer[index] == ' ')
    return ++index;

  if (ts->src.buffer[index] == '\0')
  {
    SET_TYPE(TT_END_OF_INPUT);
    return 0;
  }

  for (size_t i = 0;;i++)
  {
    if (ts->src.buffer[index] == ' ' || ts->src.buffer[index] == '\0' ||
        is_reserved_text(ts->src.buffer[index]))
      break;

    ts->text[i] = ts->src.buffer[index];
    index++;
  }

  if (single_quote)
    SET_TYPE(TT_TEXT);
  else
    SET_TYPE(tokenize_reserved_words(xstrdup(ts->text)));

  return index;
}

void next_token(struct TokenState *ts)
{
  size_t index = ts->next_index, start_index = ts->index;

  switch (ts->src.buffer[index])
  {
    case '#':
    case '\0':
      SET_TYPE(TT_END_OF_INPUT);
      break;
    case '\n':
      SET_TYPE_INC(TT_NEW_LINE);
      break;
    case '(':
      SET_TYPE_INC(TT_LPAREN);
      break;
    case ')':
      SET_TYPE_INC(TT_RPAREN);
      break;
    case '{':
      SET_TYPE_INC(TT_LBRACE);
      break;
    case '}':
      SET_TYPE_INC(TT_RBRACE);
      break;
    case '!':
      SET_TYPE_INC(TT_BANG);
      break;
    case '=':
      SET_TYPE_INC(TT_EQUAL);
      break;
    case ';':
      index++;
      if (ts->src.buffer[index] == ';')
      {
        SET_TYPE_INC(TT_DOUBLE_SEMICOLON);
      }
      else
        SET_TYPE(TT_SEMICOLON);
      break;
    case '&':
      index++;
      switch (ts->src.buffer[index])
      {
        case '&':
          SET_TYPE_INC(TT_AMPAMP);
          break;
        case '>':
          SET_TYPE_INC(TT_AMPGREATER);
          break;
        default:
          SET_TYPE(TT_AMP);
          break;
      }
      break;
    case '|':
      index++;
      if (ts->src.buffer[index] == '|')
      {
        SET_TYPE_INC(TT_PIPEPIPE);
      }
      else
        SET_TYPE(TT_PIPE);
      break;
    case '>':
      index++;
      switch (ts->src.buffer[index])
      {
        case '>':
          SET_TYPE_INC(TT_GREATERGREATER);
          break;
        case '(':
          SET_TYPE_INC(TT_GREATERLPAREN);
          break;
        case '&':
          SET_TYPE_INC(TT_GREATERAMP);
          break;
        case '|':
          SET_TYPE_INC(TT_GREATERPIPE);
          break;
        default:
          SET_TYPE(TT_GREATER);
          break;
      }
      break;
    case '<':
      index++;
      switch (ts->src.buffer[index])
      {
        case '<':
          SET_TYPE_INC(TT_LESSLESS);
          break;
        case '>':
          SET_TYPE_INC(TT_LESSGREATER);
          break;
        case '(':
          SET_TYPE_INC(TT_LESSLPAREN);
          break;
        case '&':
          SET_TYPE_INC(TT_LESSAMP);
          break;
        default:
          SET_TYPE(TT_LESS);
          break;
      }
      break;
    case '1':
      index++;
      if (ts->src.buffer[index] == '>')
      {
        SET_TYPE_INC(TT_ONEGREATER);
        break;
      }
      index--;
    case '2':
      index++;
      if (ts->src.buffer[index] == '>')
      {
        SET_TYPE_INC(TT_TWOGREATER);
        break;
      }
      index--;
    default:
      index = tokenize_text(ts, index);
      break;
  }

  ts->index = start_index;
  ts->next_index = index;
}

list_t *tokenize(char *line)
{
  struct TokenState ts = {
    .error = 0,
    .type = TT_UNKNOWN,
    .text = { '\0' },
    .src.buffer = line,
    .src.length = strlen(line),
    .index = 0,
    .next_index = 0,
  };

  list_t *tokens = list_create();

  while (ts.type != TT_END_OF_INPUT) {
    if (ts.src.buffer[ts.next_index] == ' ') {
      ts.next_index++;
      continue;
    }

    next_token(&ts);

    if (ts.type == TT_END_OF_INPUT)
      break;

    token_t *new_token = xcalloc(1, sizeof(token_t));
    new_token->type = ts.type;

    if (ts.type == TT_TEXT) {
      new_token->text = xstrdup(ts.text);
      memset(ts.text, '\0', TEXT_MAX);
    }

    list_append(tokens, new_token);
  }

  return tokens;
}

char *token_stringify(token_type_e token)
{
  switch (token)
  {
    TO_STRING(TT_UNKNOWN, "Unknown");
    TO_STRING(TT_END_OF_INPUT, "EOF");
    TO_STRING(TT_TEXT, "Text");
    TO_STRING(TT_NEW_LINE, "New Line");
    TO_STRING(TT_AMP, "Amp");
    TO_STRING(TT_AMPAMP, "AmpAmp");
    TO_STRING(TT_LPAREN, "Left Paren");
    TO_STRING(TT_RPAREN, "Right Paren");
    TO_STRING(TT_SEMICOLON, "Semicolon");
    TO_STRING(TT_EQUAL, "Equal");
    TO_STRING(TT_DOUBLE_SEMICOLON, "Double Semicolon");
    TO_STRING(TT_PIPE, "Pipe");
    TO_STRING(TT_PIPEPIPE, "PipePipe");
    TO_STRING(TT_LESS, "Less");
    TO_STRING(TT_LESSLESS, "LessLess");
    TO_STRING(TT_LESSAMP, "LessAmp");
    TO_STRING(TT_LESSGREATER, "LessGreater");
    TO_STRING(TT_LESSLPAREN, "LessLeft Paren");
    TO_STRING(TT_GREATER, "Greater");
    TO_STRING(TT_GREATERGREATER, "GreaterGreater");
    TO_STRING(TT_GREATERPIPE, "GreaterPipe");
    TO_STRING(TT_GREATERAMP, "GreaterAmp");
    TO_STRING(TT_GREATERLPAREN, "GreaterLeft Paren");
    TO_STRING(TT_ONEGREATER, "One Greater");
    TO_STRING(TT_TWOGREATER, "Two Greater");
    TO_STRING(TT_AMPGREATER, "AmpGreater");
    TO_STRING(TT_IF, "If");
    TO_STRING(TT_THEN, "Then");
    TO_STRING(TT_ELSE, "Else");
    TO_STRING(TT_ELIF, "Elif");
    TO_STRING(TT_FI, "Fi");
    TO_STRING(TT_DO, "Do");
    TO_STRING(TT_DONE, "Done");
    TO_STRING(TT_CASE, "Case");
    TO_STRING(TT_ESAC, "Esac");
    TO_STRING(TT_WHILE, "While");
    TO_STRING(TT_UNTIL, "Until");
    TO_STRING(TT_FOR, "For");
    TO_STRING(TT_LBRACE, "Left Brace");
    TO_STRING(TT_RBRACE, "Right Brace");
    TO_STRING(TT_BANG, "Bang");
    TO_STRING(TT_IN, "In");
    TO_STRING(TT_FUNCTION, "Function");
  }
  return "Symbol not found";
}

void tokens_to_string(list_t *tokens)
{
  token_t *t = NULL;
  int size = list_size(tokens);
  for (int i = 0; i < size; i++) {
    t = (token_t*) list_get(tokens, i);

    if (t->type == TT_TEXT) {
      printf("[%s]:'%s'\n", token_stringify(t->type), t->text);
    } else {
      printf("[%s]\n", token_stringify(t->type));
    }
  }
}
