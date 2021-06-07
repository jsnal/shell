#include "parse.h"

void cleanup_tokenized_command(char** tokenizedCommand)
{
  for (unsigned int i = 0; *tokenizedCommand[i] != '\0'; i++)
    free(tokenizedCommand[i]);

  free(tokenizedCommand);
}

struct Token *slice_tokens(struct Token *tokens, int start, int end)
{
  int elements = (end - start + 1);
  int bytes = sizeof(struct Token) * elements;

  struct Token *slice = malloc(bytes);
  memcpy(slice, tokens + start, bytes);

  return slice;
}

struct Command *parse_command(struct Token *tokens, enum TokenState type, int size)
{
  unsigned int commandCount = 0;
  struct Command *c = calloc(sizeof(struct Command), 1);

  if (c == NULL)
  {
    fprintf(stderr, "error: parse_command: calloc failed\n");
    exit(EXIT_FAILURE);
    /* FIXME exit_clean(EXIT_FAILURE); */
  }

  if (tokens[0].type != s_Text)
  {
    fprintf(stderr, "error: parse_line: problems parsing line\n");
    return NULL;
  }

  for (unsigned int i = 0; i < size; i++)
  {
    switch (tokens[i].type)
    {
      case s_RedirectAll:
      case s_RedirectError:
      case s_RedirectErrorAppend:
      case s_RedirectIn:
      case s_RedirectInAppend:
      case s_RedirectOut:
      case s_RedirectOutAppend:
        c->redirects[tokens[i].type] = strdup(tokens[i + 1].data);
        i++;
        break;
      case s_Text:
      case s_TextLiteral:
        c->argv[commandCount++] = strdup(tokens[i].data);
        break;
      default:
        fprintf(stderr, "error: parse_line: problems parsing line\n");
        return NULL;
    }
  }

  cleanup_tokens(tokens);

  c->name = c->argv[0];
  c->argc = commandCount;
  c->next = NULL;
  c->prev = NULL;
  return c;
}

int parse_line(char *line)
{
  struct Token *tokens = tokenize_line(line);
  struct Command *next;

  struct Command *test = parse_command(tokens, s_Empty, 4);

  printf("%s\n", test->name);
  for (unsigned int i = 0; i < test->argc; i++)
    printf("%s\n", test->argv[i]);

  for (unsigned int i = 0; i < TOKENS_SIZE; i++)
    if (test->redirects[i])
      printf("%s by %d\n", test->redirects[i], i);

  /* unsigned int start = 0, end = 0, absolute_end = 0; */
  /* enum TokenState first_opt = s_Empty; */
  /* for (unsigned int i = 0; i < TOKENS_LIMIT && tokens[i].data != NULL; i++) */
  /* { */
  /*   if (tokens[i].type == s_And        || */
  /*       tokens[i].type == s_Or         || */
  /*       tokens[i].type == s_Pipe       || */
  /*       tokens[i].type == s_Semicolon) */
  /*   { */
  /*     first_opt = tokens[i].type; */
  /*     break; */
  /*   } */
  /*  */
  /*   end++; */
  /* } */
  /*  */
  /* // Special case to set the front of the LinkedList */
  /* command = parse_command(slice_tokens(tokens, start, end - 1), first_opt, end - start); */
  /* start = end + 1; */
  /*  */
  /* for (unsigned int i = end; i < TOKENS_LIMIT && tokens[i].data != NULL; i++) */
  /* { */
  /*   if (tokens[i].type == s_And        || */
  /*       tokens[i].type == s_Or         || */
  /*       tokens[i].type == s_Pipe       || */
  /*       tokens[i].type == s_Semicolon) */
  /*   { */
  /*     next->next = parse_command(slice_tokens(tokens, start, end - 1), tokens[i].type, end - start); */
  /*     next->next->prev = next; */
  /*     next = next->next; */
  /*     start = end + 1; */
  /*     end++; */
  /*   } */
  /*  */
  /*   end++; */
  /*   absolute_end++; */
  /* } */

  cleanup_tokens(tokens);
  return 0;
}
