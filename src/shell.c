#include "shell.h"

char *line;

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

int get_command_operator(char *token)
{
  printf("call\n");
  unsigned int i = 0;
  while (token[i] != '\0')
  {
    printf("%c\n", token[i]);
    i++;
  }
  return 0;
}

struct Command *parse_command(char *line)
{
  char *lineToken;
  unsigned int lineTokenCount = 0;
  struct Command *c = calloc(sizeof(struct Command), 1);

  if (c == NULL)
  {
    fprintf(stderr, "error: parse_command: calloc failed\n");
    exit_clean(EXIT_FAILURE);
  }

  for (lineToken = strtok(line, " "); lineToken; lineToken = strtok(NULL, " "))
  {
    /* get_command_operator(lineToken); */
    c->argv[lineTokenCount++] = lineToken;
  }

  c->name = c->argv[0];
  c->argc = lineTokenCount;
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
