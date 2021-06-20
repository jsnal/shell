#include "parse.h"

void consume_token(struct ParseState **ps, struct Token *token)
{
  struct Token *current = (*ps)->tokens_list;

  while (current != NULL && current->id != token->id)
    current = current->next;

  // Front of list
  if (current->prev == NULL)
  {
    (*ps)->tokens_list = current->next;
    (*ps)->tokens_list->prev = NULL;
    free(current);
    return;
  }

  // Back of list
  if (current->next == NULL)
  {
    (*current).prev->next = NULL;
    free(current);
    return;
  }

  (*current).prev->next = current->next;
  (*current).next->prev = current->prev;
  free(current);
}

void cleanup_tokenized_command()
{
}

struct AndOr *parse_command_list(struct Token *tokens_list)
{
}

struct AndOr *parse_andor(struct ParseState *ps)
{}

struct Pipeline *parse_pipeline(struct ParseState *ps)
{}

struct Redirect *scan_tokens_for_redirect(struct ParseState *ps)
{
  struct Token *current = ps->tokens_list;
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
  consume_token(&ps, current->next);
  consume_token(&ps, current);

  return redirect;
}

struct Cmd *parse_simple_command(struct ParseState *ps)
{
  struct Redirect *redirects = scan_tokens_for_redirect(ps), *next_redirect;
  while ((next_redirect = scan_tokens_for_redirect(ps)) != NULL)
    redirects->next = next_redirect;

  struct Token *current = ps->tokens_list;
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

struct Node *parse_to_node(struct ParseState *ps)
{
  struct Node *node = calloc(1, sizeof(struct Node));
  node->node_type = ps->node_type;

  switch (ps->node_type)
  {
    case NT_ANDOR:
      node->andor = parse_andor(ps);
      break;
    case NT_CASE:
      TODO;
    case NT_FUNCTION:
      TODO;
    case NT_IF:
      TODO;
    case NT_PIPELINE:
      node->pipeline = parse_pipeline(ps);
      break;
    case NT_SIMPLE_COMMAND:
      if ((node->command = parse_simple_command(ps)) == NULL)
        return NULL;
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

int scan_tokens_for_andor(struct ParseState *ps)
{
  struct Token *current = ps->tokens_list;

  while (current != NULL)
  {
    if (current->tokenType == TT_AMPAMP || current->tokenType == TT_PIPEPIPE)
      return 1;

    current = current->next;
  }

  return 0;
}

int scan_tokens_for_pipeline(struct ParseState *ps)
{
  struct Token *current = ps->tokens_list;

  while (current != NULL)
  {
    if (current->tokenType == TT_PIPE)
      return 1;

    current = current->next;
  }

  return 0;
}

enum NodeType scan_tokens_for_node_type(struct ParseState *ps)
{
  switch (ps->tokens_list->tokenType)
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
      if (scan_tokens_for_andor(ps))
        return NT_ANDOR;

      if (scan_tokens_for_pipeline(ps))
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
  struct ParseState ps = {
    .tokens_list = tokens_list,
    .node_type = NT_ERROR,
    .index = 0,
  };

  if ((ps.node_type = scan_tokens_for_node_type(&ps)) == NT_ERROR)
    return NULL;

  tree->nodes = parse_to_node(&ps);

  return tree;
}

void command_to_string(struct Cmd *cmd)
{
  printf("   argc: %ld\n   argv: ", cmd->argc);
  for (size_t i = 0; i < cmd->argc; i++)
    printf("[%s]", cmd->argv[i]);
  printf("\n");

  if (cmd->redirects == NULL)
    printf("   redirects: (NULL)\n");
  else
    printf("   redirects: ");

  struct Redirect *r = cmd->redirects;
  while (r != NULL)
  {
    printf("[%d:%s]", r->type, r->file);
    r = r->next;
  }
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
