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
  cleanup_commands();
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
      if (parse_line(line) == -1)
        fprintf(stderr, "error: problem parsing line\n");

      /* struct Command *c = command; */
      /* while (c != NULL) */
      /* { */
      /*   printf("command: %s\n", c->name); */
      /*   c = c->next; */
      /* } */

      command_ret = exec_commands(command);
      cleanup_commands();
    }

    free(line);
    /* printf("command return: %d\n", command_ret); */

    if (command_ret == -1)
      break;
  }

  return EXIT_SUCCESS;
}
