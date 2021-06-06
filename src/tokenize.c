#include "tokenize.h"

struct Token *tokenize_line(char *line)
{
  struct Token *tokens = calloc(TOKENS_LIMIT * sizeof(struct Token), 1);
  unsigned int lineLength = strlen(line), dataPosition = 0, tokenPosition = 0;
  char data[128];
  memset(data, '\0', 128);

  for (unsigned int i = 0; i <= lineLength; i++)
  {
    switch (state)
    {
      case And:
        COMMIT(And);
        RETURN_START();
      case Background:
        if (line[i] == '&')
        {
          CONSUME();
          TO(And);
        }

        if (line[i] == '>')
        {
          CONSUME();
          TO(RedirectAll);
        }

        COMMIT(Background);
        RETURN_START();
      case Text:
        if (line[i] == ' ' || line[i] == '\t')
        {
          TO(Empty);
        }

        if (line[i] == '|')
        {
          COMMIT_AND_CONSUME(Text);
          TO(Pipe);
        }

        if (line[i] == '&')
        {
          COMMIT_AND_CONSUME(Text);
          TO(Background);
        }

        if (line[i] == '>')
        {
          COMMIT_AND_CONSUME(Text);
          TO(RedirectOut);
        }

        if (line[i] == '<')
        {
          COMMIT_AND_CONSUME(Text);
          TO(RedirectIn);
        }

        if (line[i] == '2' && line[i + 1] == '>')
        {
          COMMIT_AND_CONSUME(Text);
          i++;
          CONSUME();
          TO(RedirectError);
        }

        CONSUME();

        if (line[i] == '\0')
          goto end_of_line;

        TO(Text);
      case Empty:
        if (line[i] == ' ' || line[i] == '\t')
        {
          TO(Empty);
        }

        COMMIT(Text)
        RETURN_START();
      case Or:
        COMMIT(Or);
        RETURN_START();
      case Pipe:
        if (line[i] == '|')
        {
          CONSUME();
          TO(Or);
        }

        COMMIT(Pipe);
        RETURN_START();
      case RedirectAll:
        COMMIT(RedirectAll);
        RETURN_START();
      case RedirectError:
        if (line[i] == '>')
        {
          CONSUME();
          TO(RedirectErrorAppend);
        }

        COMMIT(RedirectError);
        RETURN_START();
      case RedirectErrorAppend:
        COMMIT(RedirectErrorAppend);
        RETURN_START();
      case RedirectIn:
        if (line[i] == '<')
        {
          CONSUME();
          TO(RedirectInAppend);
        }

        COMMIT(RedirectIn);
        RETURN_START();
      case RedirectInAppend:
        COMMIT(RedirectInAppend);
        RETURN_START();
      case RedirectOut:
        if (line[i] == '>')
        {
          CONSUME();
          TO(RedirectOutAppend);
        }

        COMMIT(RedirectOut);
        RETURN_START();
      case RedirectOutAppend:
        COMMIT(RedirectOutAppend);
        RETURN_START();
      default:
        fprintf(stderr, "Entered unimplemented state.\n");
        break;
    }
  }

end_of_line:
  COMMIT(Text)

  return tokens;
}

void cleanup_tokens(struct Token *tokens)
{
  for (unsigned int i = 0; i < TOKENS_LIMIT; i++)
    if (tokens[i].data != NULL)
      free(tokens[i].data);

  free(tokens);
}
