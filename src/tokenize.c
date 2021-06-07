#include "tokenize.h"

struct Token *tokenize_line(char *line)
{
  state = s_Text;
  struct Token *tokens = calloc(TOKENS_LIMIT * sizeof(struct Token), 1);
  unsigned int lineLength = strlen(line), dataPosition = 0, tokenPosition = 0;
  char data[128];
  memset(data, '\0', 128);

  for (unsigned int i = 0; i <= lineLength; i++)
  {
    switch (state)
    {
      case s_And:
        COMMIT(s_And);
        RETURN_START();
      case s_Background:
        if (line[i] == '&')
        {
          CONSUME();
          TO(s_And);
        }

        if (line[i] == '>')
        {
          CONSUME();
          TO(s_RedirectAll);
        }

        COMMIT(s_Background);
        RETURN_START();
      case s_Comment:
        CONSUME();
        if (line[i] == '\0')
        {
          COMMIT(s_Comment);
          goto end_of_line;
        }

        TO(s_Comment);
      case s_Empty:
        if (line[i] == ' ' || line[i] == '\t')
        {
          TO(s_Empty);
        }

        COMMIT(s_Text)
        RETURN_START();
      case s_Text:
        if (line[i] == ' ' || line[i] == '\t')
        {
          TO(s_Empty);
        }

        if (line[i] == '\'') { COMMIT(s_Text); TO(s_TextLiteral); }
        if (line[i] == '#')  { COMMIT_AND_CONSUME(s_Text); TO(s_Comment); }
        if (line[i] == '~')  { COMMIT_AND_CONSUME(s_Text); TO(s_Tilde); }
        if (line[i] == '|')  { COMMIT_AND_CONSUME(s_Text); TO(s_Pipe); }
        if (line[i] == ';')  { COMMIT_AND_CONSUME(s_Text); TO(s_Semicolon); }
        if (line[i] == '&')  { COMMIT_AND_CONSUME(s_Text); TO(s_Background); }
        if (line[i] == '>')  { COMMIT_AND_CONSUME(s_Text); TO(s_RedirectOut); }
        if (line[i] == '<')  { COMMIT_AND_CONSUME(s_Text); TO(s_RedirectIn); }

        if (line[i] == '2' && line[i + 1] == '>')
        {
          COMMIT_AND_CONSUME(s_Text);
          i++;
          CONSUME();
          TO(s_RedirectError);
        }

        CONSUME();

        if (line[i] == '\0')
          goto end_of_line;

        TO(s_Text);

      // FIXME: If there isn't a closing quote then we never know
      case s_TextLiteral:
        if (line[i] != '\'')
        {
          CONSUME();
          TO(s_TextLiteral);
        }

        COMMIT(s_TextLiteral);
        TO(s_Text);
      case s_Or:
        COMMIT(s_Or);
        RETURN_START();
      case s_Pipe:
        if (line[i] == '|')
        {
          CONSUME();
          TO(s_Or);
        }

        COMMIT(s_Pipe);
        RETURN_START();
      case s_RedirectAll:
        COMMIT(s_RedirectAll);
        RETURN_START();
      case s_RedirectError:
        if (line[i] == '>')
        {
          CONSUME();
          TO(s_RedirectErrorAppend);
        }

        COMMIT(s_RedirectError);
        RETURN_START();
      case s_RedirectErrorAppend:
        COMMIT(s_RedirectErrorAppend);
        RETURN_START();
      case s_RedirectIn:
        if (line[i] == '<')
        {
          CONSUME();
          TO(s_RedirectInAppend);
        }

        COMMIT(s_RedirectIn);
        RETURN_START();
      case s_RedirectInAppend:
        COMMIT(s_RedirectInAppend);
        RETURN_START();
      case s_RedirectOut:
        if (line[i] == '>')
        {
          CONSUME();
          TO(s_RedirectOutAppend);
        }

        COMMIT(s_RedirectOut);
        RETURN_START();
      case s_RedirectOutAppend:
        COMMIT(s_RedirectOutAppend);
        RETURN_START();
      case s_Semicolon:
        COMMIT(s_Semicolon);
        RETURN_START();
      case s_Tilde:
        COMMIT(s_Tilde);
        RETURN_START();
      default:
        fprintf(stderr, "Entered unimplemented state.\n");
        break;
    }
  }

end_of_line:
  COMMIT(s_Text)

  return tokens;
}

void cleanup_tokens(struct Token *tokens)
{
  for (unsigned int i = 0; i < TOKENS_LIMIT; i++)
    if (tokens[i].data != NULL)
      free(tokens[i].data);

  free(tokens);
}
