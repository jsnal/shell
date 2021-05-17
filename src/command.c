#include "command.h"

void cleanup_commands(struct Commands *cmds)
{
  for (unsigned int i = 0; i < cmds->count; i++)
    free(cmds->list[i]);

  free(cmds);
}
