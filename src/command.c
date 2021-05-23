#include "command.h"

void cleanup_commands()
{
  struct Command *head;

  while (command != NULL)
  {
    head = command;
    command = command->next;
    free(head);
  }
}
