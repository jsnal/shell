#include "shell.h"

char *line;

/*
 * Check for blank or space only line.
 */
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

void set_system_environment_variables()
{
  if ((SYS_PATH = getenv("PATH")) == NULL)
    SYS_PATH = "/bin";

  if ((SYS_HOME = getenv("HOME")) == NULL)
    SYS_HOME = getpwuid(getuid())->pw_dir;
}

void clean()
{
  free(line);
  cleanup_commands(commands);
}

void exit_clean(int code)
{
  clean();
  exit(code);
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
    c->argv[lineTokenCount++] = lineToken;

  c->name = c->argv[0];
  c->argc = lineTokenCount;
  return c;
}

struct Commands *parse_line(char *line)
{
  unsigned int commandCount = 0, count = 0;
  char *commandToken, *savePtr, *lineCopy = line;

  while (*lineCopy != '\0')
  {
    if (*lineCopy == '|')
      commandCount++;
    lineCopy++;
  }

  commandCount++;
  commands = calloc(sizeof(struct Commands) + commandCount * sizeof(struct Command*), 1);

  if (commands == NULL)
  {
    fprintf(stderr, "error: parse_line: calloc failed\n");
    exit_clean(EXIT_FAILURE);
  }

  for (commandToken = strtok_r(line, "|", &savePtr);
       commandToken;
       commandToken = strtok_r(NULL, "|", &savePtr))
    commands->list[count++] = parse_command(commandToken);

  commands->count = commandCount;
  return commands;
}

char *read_line(void)
{
  int buf_size = 128;
  char *line = malloc(buf_size * sizeof(char));
  unsigned int i = 0;
  char c;

  if (line == NULL)
  {
    fprintf(stderr, "error: read_command: malloc failed\n");
    exit_clean(EXIT_FAILURE);
  }

  while ((c = getchar()) != '\n')
  {
    if (c == EOF)
    {
      free(line);
      exit(EXIT_SUCCESS);
    }

    if (i >= buf_size)
      line = realloc(line, buf_size * 2);

    line[i++] = c;
  }

  line[i] = '\0';
  return line;
}

int shell()
{
  unsigned int command_ret = 0;

  for(;;)
  {
    set_system_environment_variables();
    fputs("$ ", stdout);
    line = read_line();

    if (!is_empty_line(line))
    {
      commands = parse_line(line);
      command_ret = exec_commands(commands);
      cleanup_commands(commands);
    }

    free(line);

    if (command_ret == -1)
      break;
  }

  return EXIT_SUCCESS;
}
