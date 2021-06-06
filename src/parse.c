#include "parse.h"

struct CommandOperators commandOperators[] = {
  { REDIRECT_OUT,          ">" },
  { REDIRECT_OUT_APPEND,   ">>" },
  { REDIRECT_IN,           "<" },
  { REDIRECT_ERROR,        "2>" },
  { REDIRECT_ERROR_APPEND, "2>>" },
  { REDIRECT_ALL,          "&>" }
};

void cleanup_tokenized_command(char** tokenizedCommand)
{
  for (unsigned int i = 0; *tokenizedCommand[i] != '\0'; i++)
    free(tokenizedCommand[i]);

  free(tokenizedCommand);
}

char** tokenize_command(char *line)
{
  char token[128];
  unsigned int commandCount = 0, tokenCounter = 0;
  unsigned int lineLength = strlen(line);
  char **tokenized = malloc(256 * sizeof(char*));
  memset(token, 0, 128);

  for (unsigned int i = 0; i < lineLength; i++)
  {
    // Ignore about leading or trailing whitespace
    if ((i == 0 || i == lineLength) && line[i] == ' ')
      continue;

    if (line[i] == ' ')
    {
      if (token[0] != 0)
      {
        tokenized[commandCount++] = strdup(token);
        memset(token, 0, 128);
        tokenCounter = 0;
      }
      continue;
    }
    else if (line[i] == '2' && line[i + 1] == '>' && line[i + 2] == '>')
    {
      if (token[0] != 0)
      {
        tokenized[commandCount++] = strdup(token);
        memset(token, 0, 128);
        tokenCounter = 0;
      }

      token[0] = line[i]; token[1] = line[i + 1]; token[2] = line[i + 2]; token[3] = '\0';
      tokenized[commandCount++] = strdup(token);
      memset(token, 0, 128);
      i++;
      continue;
    }
    else if (((line[i] == '2' || line[i] == '&') && line[i + 1] == '>') ||
        (line[i] == '>' && line[i + 1] == '>'))
    {
      if (token[0] != 0)
      {
        tokenized[commandCount++] = strdup(token);
        memset(token, 0, 128);
        tokenCounter = 0;
      }

      token[0] = line[i]; token[1] = line[i + 1]; token[2] = '\0';
      tokenized[commandCount++] = strdup(token);
      memset(token, 0, 128);
      i++;
      continue;
    }
    else if (line[i] == '>' || line[i] == '<')
    {
      if (token[0] != 0)
      {
        tokenized[commandCount++] = strdup(token);
        memset(token, 0, 128);
        tokenCounter = 0;
      }

      token[0] = line[i]; token[1] = '\0';
      tokenized[commandCount++] = strdup(token);
      memset(token, 0, 128);
      continue;
    }

    token[tokenCounter++] = line[i];
  }

  if (token[0] != 0)
    tokenized[commandCount++] = strdup(token);

  tokenized[commandCount] = "\0";

  return tokenized;
}

struct Command *parse_command(char *line)
{
  unsigned int commandCount = 0;
  struct Command *c = calloc(sizeof(struct Command), 1);

  if (c == NULL)
  {
    fprintf(stderr, "error: parse_command: calloc failed\n");
    exit(EXIT_FAILURE);
    /* FIXME exit_clean(EXIT_FAILURE); */
  }

  char** tokenizedCommand = tokenize_command(line);
  for (unsigned int i = 0; *tokenizedCommand[i] != '\0'; i++)
  {
    for (unsigned int j = 0; j < OPERATORS_SIZE; j++)
    {
      if (strcmp(tokenizedCommand[i], commandOperators[j].token) == 0)
      {
        // Invalid syntax if the first string is a redirect operator or if there
        // isn't a file after the operator.
        if (*tokenizedCommand[i + 1] == '\0' || i == 0)
        {
          fprintf(stderr, "error: parse_command: invalid redirect syntax\n");
          cleanup_tokenized_command(tokenizedCommand);
          cleanup_commands(c);
          return NULL;
        }

        c->redirects[commandOperators[j].name] = strdup(tokenizedCommand[i + 1]);
        i++;
        goto command_operator_found;
      }
    }

    c->argv[commandCount++] = strdup(tokenizedCommand[i]);
command_operator_found:;
  }

  cleanup_tokenized_command(tokenizedCommand);

  c->name = c->argv[0];
  c->argc = commandCount;
  c->next = NULL;
  c->prev = NULL;
  return c;
}

int parse_line(char *line)
{
  struct Command *next;
  char *commandToken, *savePtr;

  // Set the front of the LinkedList
  commandToken = strtok_r(line, "|", &savePtr);
  if (!commandToken)
    return -1;

  command = parse_command(commandToken);
  if (command == NULL)
    return -1;

  next = command;

  // Fill out the rest of the LinkedList if it exists
  commandToken = strtok_r(NULL, "|", &savePtr);
  while(commandToken != NULL)
  {
    next->next = parse_command(commandToken);
    next->next->prev = next;
    next = next->next;
    commandToken = strtok_r(NULL, "|", &savePtr);
  }

  return 0;
}

