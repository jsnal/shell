#include "parse.h"

void cleanup_tokenized_command()
{
}

struct AndOr *parse_command_list(struct Token *tokens_list)
{
}

struct AndOr *parse_andor(struct Token *tokens_list)
{}

struct Pipeline *parse_pipeline(struct Token *tokens_list)
{}

struct Cmd *parse_simple_command(struct Token *tokens_list)
{
  struct Token *current = tokens_list, *last = NULL;

  while (current != NULL)
  {
    printf("%s\n", current->text);
    last = current;
    current = current->next;
  }

  /* printf("the last token was %s\n", last->text); */
}

struct Node *parse_to_node(enum NodeType node_type, struct Token *tokens_list)
{
  printf("parsing %d\n", node_type);
  struct Node *node = calloc(1, sizeof(struct Node));
  node->node_type = node_type;

  switch (node_type)
  {
    case NT_ANDOR:
      node->andor = parse_andor(tokens_list);
      break;
    case NT_CASE:
      TODO;
    case NT_FUNCTION:
      TODO;
    case NT_IF:
      TODO;
    case NT_PIPELINE:
      node->pipeline = parse_pipeline(tokens_list);
      break;
    case NT_SIMPLE_COMMAND:
      node->command = parse_simple_command(tokens_list);
      break;
    case NT_WHILE:
      TODO;
    default:
      fprintf(stderr, "error: parse_to_node: unknown node type\n");
      free(node);
      return NULL;
  }

  return node;
}

int scan_tokens_for_andor(struct Token *tokens_list)
{
  struct Token *current = tokens_list;

  while (current != NULL)
  {
    if (current->tokenType == TT_AMPAMP || current->tokenType == TT_PIPEPIPE)
      return 1;

    current = current->next;
  }

  return 0;
}

int scan_tokens_for_pipeline(struct Token *tokens_list)
{
  struct Token *current = tokens_list;

  while (current != NULL)
  {
    if (current->tokenType == TT_PIPE)
      return 1;

    current = current->next;
  }

  return 0;
}

enum NodeType scan_tokens_for_node_type(struct Token *tokens_list)
{
  switch (tokens_list->tokenType)
  {
    case TT_IF:
      return NT_IF;
    case TT_WHILE:
      return NT_WHILE;
    case TT_CASE:
      return NT_CASE;
    case TT_FUNCTION:
      return NT_FUNCTION;
    case TT_TEXT:
      if (scan_tokens_for_andor(tokens_list))
        return NT_ANDOR;

      if (scan_tokens_for_pipeline(tokens_list))
        return NT_PIPELINE;

      return NT_SIMPLE_COMMAND;
    default:
      fprintf(stderr, "error: illegal symbol\n");
      return NT_ERROR;
  }
}

int parse(struct Token *tokens_list)
{
  struct Tree tree = {
    .nodes = NULL,
  };

  enum NodeType current_type;
  if ((current_type = scan_tokens_for_node_type(tokens_list)) == NT_ERROR)
    return 1;

  /* tree.nodes = */
  parse_to_node(current_type, tokens_list);
}
