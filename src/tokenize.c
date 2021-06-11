#include "tokenize.h"

void cleanup_token_list(struct Token *tokens_list[], int size)
{
  for (int i = 0; i < size; i++)
  {
    if (tokens_list[i]->text)
      free(tokens_list[i]->text);

    if (tokens_list[i])
      free(tokens_list[i]);
  }
}

enum TokenType tokenize_reserved_words(char *t)
{
  enum TokenType ret = TT_TEXT;

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

int tokenize_text(struct TokenState *ts, size_t index)
{
  if (ts->src.contents[index] == ' ')
    return ++index;

  if (ts->src.contents[index] == '\0')
  {
    SET_TYPE(TT_END_OF_INPUT);
    return 0;
  }

  memset(ts->text, '\0', TOKENS_SIZE);

  for (size_t i = 0;;i++)
  {
    if (ts->src.contents[index] == ' ')
      break;

    if (ts->src.contents[index] == '\0')
    {
      SET_TYPE(TT_END_OF_INPUT);
      break;
    }

    ts->text[i] = ts->src.contents[index];
    index++;
  }

  SET_TYPE(tokenize_reserved_words(strdup(ts->text)));

  /* SET_TYPE(TT_TEXT); */
  return index;
}

void next_token(struct TokenState *ts)
{
  size_t index = ts->next_index, start_index = ts->index;

  switch (ts->src.contents[index])
  {
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
    case ';':
      index++;
      if (ts->src.contents[index] == ';')
      {
        SET_TYPE_INC(TT_DOUBLE_SEMICOLON);
      }
      else
        SET_TYPE(TT_SEMICOLON);
      break;
    case '&':
      index++;
      if (ts->src.contents[index] == '&')
      {
        SET_TYPE_INC(TT_AMPAMP);
      }
      else
        SET_TYPE(TT_AMP);
      break;
    case '|':
      index++;
      if (ts->src.contents[index] == '|')
      {
        SET_TYPE_INC(TT_PIPEPIPE);
      }
      else
        SET_TYPE(TT_PIPE);
      break;
    case '>':
      index++;
      switch (ts->src.contents[index])
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
      switch (ts->src.contents[index])
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
    default:
      index = tokenize_text(ts, index);
      break;
  }

  ts->index = start_index;
  ts->next_index = index;
}

int tokenize(char *line, struct Token *tokens_list[], size_t *size)
{
  size_t position = 0;

  struct TokenState ts = {
    .error = 0,
    .tokenType = TT_UNKNOWN,
    .text = { '\0' },
    .src.contents = line,
    .src.length = strlen(line),
    .index = 0,
    .next_index = 0,
  };

  while (ts.tokenType != TT_END_OF_INPUT)
  {
    if (ts.src.contents[ts.next_index] == ' ')
    {
      ts.next_index++;
      continue;
    }

    next_token(&ts);

    if (ts.tokenType == TT_END_OF_INPUT)
      break;

    tokens_list[position] = malloc(sizeof(struct Token));
    tokens_list[position]->tokenType = ts.tokenType;

    if (ts.tokenType == TT_TEXT && ts.text != NULL)
      tokens_list[position]->text = strdup(ts.text);

    position++;
  }

  (*size) = position;

  return ts.error;
}

void tokenize_to_string(struct Token *tokens_list[], size_t size)
{
  printf("Found %ld tokens.\n\n", size);

  for (size_t i = 0; i < size; i++)
  {
    switch (tokens_list[i]->tokenType)
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
  }
}
