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

struct Redirect *scan_tokens_for_redirect(struct Token **index)
{
  struct Token *current = (*index);
  struct Redirect *redirect = calloc(1, sizeof(struct Redirect));

  while (current != NULL)
  {
    switch (current->tokenType)
    {
      case TT_LESS:
        redirect->type = RT_INPUT;
        goto found_redirect;
      case TT_LESSGREATER:
        redirect->type = RT_INOUT;
        goto found_redirect;
      case TT_GREATER:
        redirect->type = RT_OUTPUT;
        goto found_redirect;
      case TT_GREATERGREATER:
        redirect->type = RT_APPEND;
        goto found_redirect;
      case TT_GREATERPIPE:
        redirect->type = RT_CLOBBER;
        goto found_redirect;
      case TT_ONEGREATER:
        redirect->type = RT_OUTPUT;
        goto found_redirect;
      case TT_TWOGREATER:
        redirect->type = RT_ERROR;
        goto found_redirect;
      case TT_AMPGREATER:
        redirect->type = RT_ALL;
        goto found_redirect;
      case TT_LESSLESS:
      case TT_LESSAMP:
      case TT_LESSLPAREN:
      case TT_GREATERAMP:
      case TT_GREATERLPAREN:
        TODO;
      default:
        break;
    }

    current = current->next;
  }

  free(redirect);
  return NULL;

found_redirect:
  if (current->next == NULL || current->next->tokenType != TT_TEXT)
  {
    fprintf(stderr, "error: no redirection file\n");
    // TODO: fail without exiting whole shell
    return NULL;
  }

  redirect->file = current->next->text;

  if (current->next->next == NULL)
    *index = NULL;
  else
    *index = (*current).next->next;

  return redirect;
}

struct Cmd *parse_simple_command(struct Token *tokens_list)
{
  struct Token *current = tokens_list;
  struct Token *redirect_token_index = tokens_list;

  struct Redirect *redirects = scan_tokens_for_redirect(&redirect_token_index);
  while (redirect_token_index != NULL && redirects != NULL)
    redirects->next = scan_tokens_for_redirect(&redirect_token_index);

  struct Redirect *tmp = redirects;
  while (tmp != NULL)
  {
    printf("%d - %s\n", tmp->type, tmp->file);
    tmp = tmp->next;
  }

  struct Cmd *cmd = calloc(1, sizeof(struct Cmd));
  size_t argc = 0;

  while (current != NULL)
  {
    switch (current->tokenType)
    {
      case TT_TEXT:
        cmd->argv[argc++] = current->text;
        break;
      case TT_AMP:
        cmd->background = 1;
        break;
      case TT_SEMICOLON:
        cmd->sequence = 1;
        break;
      default:
        break;
    }

    current = current->next;
  }

  cmd->argc = argc;
  cmd->redirects = redirects;

  return cmd;
}

struct Node *parse_to_node(enum NodeType node_type, struct Token *tokens_list)
{
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

struct Tree *parse(struct Token *tokens_list)
{
  struct Tree *tree = calloc(1, sizeof(struct Tree));

  enum NodeType current_type;
  if ((current_type = scan_tokens_for_node_type(tokens_list)) == NT_ERROR)
    return NULL;

  tree->nodes = parse_to_node(current_type, tokens_list);

  return tree;
}

void command_to_string(struct Cmd *cmd)
{
  printf("   argc: %ld\n   ", cmd->argc);
  for (size_t i = 0; i < cmd->argc; i++)
    printf("[%s]", cmd->argv[i]);
  printf("\n");
}

void tree_to_string(struct Tree *tree)
{
  if (tree == NULL)
  {
    printf("(NULL)\n");
    return;
  }
  printf("Tree\n");

  if (tree->nodes == NULL)
  {
    printf(" (NULL)\n");
    return;
  }
  printf(" Nodes\n");

  if (tree->nodes->node_type == NT_SIMPLE_COMMAND)
  {
    if (tree->nodes->command == NULL)
    {
      printf("  (NULL)\n");
      return;
    }
    printf("  Simple Command\n");
    command_to_string(tree->nodes->command);
  }
}
