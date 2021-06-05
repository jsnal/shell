#include "shell.h"

char *line;
static struct CommandOperators commandOperators[] = {
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

void cleanup_history()
{
  for (unsigned int i = 0; history->list[i] != NULL; i++)
    free(history->list[i]);
  free(history);
}

void clean()
{
  if (command) cleanup_commands();
  if (history) cleanup_history();
}

void exit_clean(int code)
{
  clean();
  exit(code);
}

int is_empty_line(char *line)
{
  int len = strlen(line);

  if (len < 0)
    return 1;

  for (unsigned int i = 0; i < len; i++)
  {
    if (!isspace(line[i]))
      return 0;
  }

  return 1;
}

void initialize_system_environment_variables()
{
  char *cwd;
  if ((cwd = getcwd(NULL, 0)) != NULL)
    setenv("PWD", cwd, 1);
  free(cwd);

  if ((SYS_HOME = getenv("HOME")) == NULL)
    SYS_HOME = getpwuid(getuid())->pw_dir;

  setenv("OLDPWD", SYS_HOME, 1);
}

void set_system_environment_variables()
{
  if ((SYS_PATH = getenv("PATH")) == NULL)
    SYS_PATH = "/bin";

  PWD = getenv("PWD");
  OLDPWD = getenv("OLDPWD");
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
    exit_clean(EXIT_FAILURE);
  }

  char** tokenizedCommand = tokenize_command(line);
  for (unsigned int i = 0; *tokenizedCommand[i] != '\0'; i++)
  {
    for (unsigned int j = 0; j < OPERATORS_SIZE; j++)
    {
      if (strcmp(tokenizedCommand[i], commandOperators[j].token) == 0)
      {
        if (tokenizedCommand[i + 1] == NULL)
          fprintf(stderr, "error: parse_command: invalid redirect syntax\n");

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

char *read_line(void)
{
  int buf_size = 128;
  char *ret_line = malloc(buf_size * sizeof(char));
  unsigned int i = 0;
  char c;

  if (ret_line == NULL)
  {
    fprintf(stderr, "error: read_command: malloc failed\n");
    exit_clean(EXIT_FAILURE);
  }

  while ((c = getchar()) != '\n')
  {
    if (c == EOF)
    {
      free(ret_line);
      exit_clean(EXIT_SUCCESS);
    }

    if (i >= buf_size)
      ret_line = realloc(ret_line, buf_size * 2);

    ret_line[i++] = c;
  }

  ret_line[i] = '\0';
  return ret_line;
}

int shell()
{
  unsigned int command_ret = 0;
  char *history_line;

  if (initialize_history() == -1)
    fprintf(stderr, "error: history: unable to initialize history\n");

  initialize_system_environment_variables();

  for(;;)
  {
    set_system_environment_variables();
    printf("%s$ ", PWD);
    line = read_line();

    if (!is_empty_line(line))
    {
      history_line = strdup(line);
      if (parse_line(line) == -1)
        fprintf(stderr, "error: problem parsing line\n");

#if DEBUG
      struct Command *c = command;
      while (c != NULL)
      {
        printf("command: %s: argc: %d\n", c->name, c->argc);
        c = c->next;
      }
#endif

      command_ret = exec_commands(command);
      cleanup_commands();

      push_history(history_line);
      free(history_line);
    }

    free(line);

#if DEBUG
    printf("command return: %d\n", command_ret);
#endif

    if (command_ret == -1)
      break;
  }

  cleanup_history();
  return EXIT_SUCCESS;
}
