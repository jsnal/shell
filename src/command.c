#include "command.h"

void cleanup_commands()
{
  struct Command *head;

  while (command != NULL)
  {
    head = command;
    command = command->next;
    for (unsigned int i = 0; head->argv[i] != NULL; i++)
      free(head->argv[i]);

    for (unsigned int i = 0; i < OPERATORS_SIZE; i++)
      if (head->redirects[i] != NULL)
        free(head->redirects[i]);

    free(head);
  }
}
