#include "parse.h"

struct Token *consume_token(struct ParseState **ps, struct Token *token)
{
  if ((*ps)->tokens_list == NULL)
    return NULL;

  struct Token *current = (*ps)->tokens_list;
  size_t list_size;

  for (list_size = 0; current != NULL; list_size++)
    current = current->next;

  if (list_size == 1)
  {
    (*ps)->tokens_list = NULL;
    return token;
  }

  current = (*ps)->tokens_list;
  while (current != NULL && current->id != token->id)
    current = current->next;

  // Front of list
  if (current->prev == NULL)
  {
    (*ps)->tokens_list = (*ps)->tokens_list->next;
    (*ps)->tokens_list->prev = NULL;
    return current;
  }

  // Back of list
  if (current->next == NULL)
  {
    (*current).prev->next = NULL;
    return current;
  }

  (*current).prev->next = current->next;
  (*current).next->prev = current->prev;
  return current;
}

struct Redirect *scan_tokens_for_redirect(struct ParseState *ps)
{
  struct Token *current = ps->tokens_list;
  struct Redirect *redirect = calloc(1, sizeof(struct Redirect));

  while (current != NULL)
  {
    switch (current->token_type)
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
  if (current->next == NULL || current->next->text == NULL)
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
  while (redirects != NULL && (next_redirect = scan_tokens_for_redirect(ps)) != NULL)
    redirects->next = next_redirect;

  struct Token *current = ps->tokens_list;
  struct Cmd *cmd = calloc(1, sizeof(struct Cmd));
  size_t argc = 0;

  cmd->redirects = redirects;
  cmd->terminator_type = MT_NONE;

  while (current != NULL)
  {
    switch (current->token_type)
    {
      case TT_IF:    case TT_THEN: case TT_ELSE: case TT_ELIF: case TT_FI:
      case TT_DO:    case TT_DONE: case TT_CASE: case TT_ESAC: case TT_WHILE:
      case TT_UNTIL: case TT_FOR:  case TT_IN:   case TT_FUNCTION: case TT_TEXT:
        cmd->argv[argc++] = current->text;
        struct Token *consumed_token = consume_token(&ps, current);
        current = current->next;
        free(consumed_token);
        break;
      case TT_AMP:
        consume_token(&ps, current);
        cmd->terminator_type = MT_BACKGROUND;
        cmd->argc = argc;
        return cmd;
      case TT_SEMICOLON:
        consume_token(&ps, current);
        cmd->terminator_type = MT_SEQUENCE;
        cmd->argc = argc;
        return cmd;
      default:
        current = current->next;
        break;
    }
  }

  cmd->argc = argc;
  return cmd;
}

struct Pipeline *parse_pipeline(struct ParseState *ps)
{
  struct ParseState *pipeline_ps = calloc(1, sizeof(struct ParseState));
  struct Pipeline *pipeline = calloc(1, sizeof(struct Pipeline));

  /* LinkedList iterators */
  struct Token *current_token = ps->tokens_list, *current_subtoken = NULL;
  struct Cmd *current_command = NULL;

  while (current_token != NULL)
  {
    if (current_token->token_type == TT_PIPE)
    {
      struct Token *consumed_token = consume_token(&ps, current_token);
      current_token = current_token->next;
      free(consumed_token);

      if (current_command == NULL)
        pipeline->commands = current_command = parse_simple_command(pipeline_ps);
      else {
        current_command->next = parse_simple_command(pipeline_ps);
        current_command = current_command->next;
      }

      pipeline_ps->tokens_list = NULL;
      current_subtoken = NULL;
      continue;
    }

    struct Token *consumed_token = consume_token(&ps, current_token);
    current_token = current_token->next;
    consumed_token->prev = NULL;
    consumed_token->next = NULL;

    if (current_subtoken == NULL)
      pipeline_ps->tokens_list = current_subtoken = consumed_token;
    else {
      current_subtoken->next = consumed_token;
      consumed_token->prev = current_subtoken;
      current_subtoken = current_subtoken->next;
    }
  }

  // TODO: better error
  if (pipeline_ps->tokens_list == NULL)
  {
    fprintf(stderr, "error: parse_pipeline: nothing found after pipe\n");
    return NULL;
  }

  if (current_command == NULL)
    pipeline->commands = parse_simple_command(pipeline_ps);
  else
    current_command->next = parse_simple_command(pipeline_ps);

  free(pipeline_ps);

  pipeline->andor_type = AOT_NONE;
  return pipeline;
}

struct AndOr *parse_andor(struct ParseState *ps)
{
  struct ParseState *andor_ps = calloc(1, sizeof(struct ParseState));
  struct AndOr *andor = calloc(1, sizeof(struct AndOr));
  struct Token *current_token = ps->tokens_list, *current_subtoken = NULL;
  struct Pipeline *current_pipeline = NULL;

  while (current_token != NULL)
  {
    if (current_token->token_type == TT_AMPAMP || current_token->token_type == TT_PIPEPIPE)
    {
      struct Token *consumed_token = consume_token(&ps, current_token);
      current_token = current_token->next;

      if (current_pipeline == NULL)
        andor->pipelines = current_pipeline = parse_pipeline(andor_ps);
      else {
        current_pipeline->next = parse_pipeline(andor_ps);
        current_pipeline = current_pipeline->next;
      }

      if (consumed_token->token_type == TT_AMPAMP)
        current_pipeline->andor_type = AOT_AND;
      else
        current_pipeline->andor_type = AOT_OR;

      free(consumed_token);
      continue;
    }

    struct Token *consumed_token = consume_token(&ps, current_token);
    current_token = current_token->next;
    consumed_token->prev = NULL;
    consumed_token->next = NULL;

    if (andor_ps->tokens_list == NULL)
      andor_ps->tokens_list = current_subtoken = consumed_token;
    else {
      current_subtoken->next = consumed_token;
      current_subtoken = current_subtoken->next;
    }
  }

  if (andor_ps->tokens_list == NULL)
  {
    fprintf(stderr, "error: parse_andor: nothing found after and/or\n");
    return NULL;
  }

  current_pipeline->next = parse_pipeline(andor_ps);
  free(andor_ps);
  return andor;
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
      if ((node->pipeline = parse_pipeline(ps)) == NULL)
        return NULL;
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
    if (current->token_type == TT_AMPAMP || current->token_type == TT_PIPEPIPE)
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
    if (current->token_type == TT_PIPE)
      return 1;

    current = current->next;
  }

  return 0;
}

enum NodeType scan_tokens_for_node_type(struct ParseState *ps)
{
  switch (ps->tokens_list->token_type)
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
      fprintf(stderr, "error: scan_tokens_for_node_type: illegal symbol\n");
      return NT_ERROR;
  }
}

struct Tree *parse(struct Token *tokens_list)
{
  struct Tree *tree = calloc(1, sizeof(struct Tree));
  struct Node *head_node = NULL, *current = NULL;
  struct ParseState ps = {
    .tokens_list = tokens_list,
    .node_type = NT_ERROR,
    .index = 0,
  };


  while (ps.tokens_list != NULL)
  {
    if ((ps.node_type = scan_tokens_for_node_type(&ps)) == NT_ERROR)
      return NULL;

    if (head_node == NULL)
      head_node = current = parse_to_node(&ps);
    else
    {
      current->next = parse_to_node(&ps);
      current = current->next;
    }
  }

  tree->nodes = head_node;
  return tree;
}

char *terminator_to_string(enum TerminatorType tt)
{
  switch (tt)
  {
    case MT_BACKGROUND:
      return "background";
    case MT_NONE:
      return "(NULL)";
    case MT_SEQUENCE:
      return "sequence";
  }
  return "unknown value";
}

char *andor_type_to_string(enum AndOrType aot)
{
  switch (aot)
  {
    case AOT_AND:
      return "and";
    case AOT_OR:
      return "or";
    case AOT_NONE:
      return "(NULL)";
  }
  return "unknown value";
}

void command_to_string(struct Cmd *cmd)
{
  printf("   argc: %ld\n   argv: ", cmd->argc);
  for (size_t i = 0; i < cmd->argc; i++)
    printf("[%s]", cmd->argv[i]);
  printf("\n");

  if (cmd->redirects == NULL)
    printf("   redirects: (NULL)");
  else
    printf("   redirects: ");

  struct Redirect *r = cmd->redirects;
  while (r != NULL)
  {
    printf("[%d:%s]", r->type, r->file);
    r = r->next;
  }
  printf("\n");
  printf("   terminator: %s\n", terminator_to_string(cmd->terminator_type));
}

void pipeline_to_string(struct Pipeline *pipeline)
{
  struct Cmd *current = pipeline->commands;
  printf("   and/or: %s\n", andor_type_to_string(pipeline->andor_type));
  while (current != NULL)
  {
    printf("   Command\n");
    command_to_string(current);
    current = current->next;
  }
}

void andor_to_string(struct AndOr *andor)
{
  struct Pipeline *current = andor->pipelines;

  while (current != NULL)
  {
    printf("   Pipeline\n");
    pipeline_to_string(current);
    current = current->next;
  }
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

  struct Node *current = tree->nodes;
  while (current != NULL)
  {
    if (current->node_type == NT_SIMPLE_COMMAND)
    {
      if (current->command == NULL)
      {
        printf("  (NULL)\n");
        return;
      }
      printf("  Simple Command\n");
      command_to_string(current->command);
    }
    else if (current->node_type == NT_PIPELINE)
    {
      if (current->command == NULL)
      {
        printf("  (NULL)\n");
        return;
      }
      printf("  Pipeline\n");
      pipeline_to_string(current->pipeline);
    }
    else if (current->node_type == NT_ANDOR)
    {
      if (current->command == NULL)
      {
        printf("  (NULL)\n");
        return;
      }
      printf("  AndOr\n");
      andor_to_string(current->andor);
    }

    current = current->next;
  }
}
