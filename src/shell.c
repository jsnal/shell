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
  if (command) cleanup_commands(command);
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
      struct Token *tokens = tokenize_line(line);
      for (unsigned int i = 0; i < 256; i++)
      {
        if (tokens[i].data != NULL)
          printf("%d: %s - %d\n", i, tokens[i].data, tokens[i].type);
      }

      cleanup_tokens(tokens);

/*       history_line = strdup(line); */
/*       if (parse_line(line) == -1) */
/*         goto no_execute; */
/*  */
/*       command_ret = execute_commands(command); */
/*       cleanup_commands(command); */
/*  */
/* no_execute: */
/*       push_history(history_line); */
/*       free(history_line); */
    }

    free(line);

    if (command_ret == -1)
      break;
  }

  cleanup_history();
  return EXIT_SUCCESS;
}
