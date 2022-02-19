/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "debug.h"
#include "list.h"
#include "parse.h"
#include "util.h"

struct Token *consume_token(parse_state_t **ps, struct Token *token)
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

static redirect_t *parse_redirect(parse_state_t *ps,
                                  struct Token *r,
                                  redirect_type_e type,
                                  int *status)
{
  if (r->next == NULL || r->next->type != TT_TEXT) {
    *status = -1;
    return NULL;
  }

  redirect_t *redirect = (redirect_t*) xcalloc(1, sizeof(redirect_t));
  redirect->type = type;
  redirect->file = r->next->text;
  consume_token(&ps, r->next);
  consume_token(&ps, r);

  *status = 1;
  return redirect;
}

static bool parse_redirects(parse_state_t *ps, list_t *redirects)
{
  struct Token *current = ps->tokens_list;

  while (current != NULL) {
    redirect_t *redirect = NULL;
    int status = 0;

    switch (current->type) {
      case TT_LESS:
        redirect = parse_redirect(ps, current, RT_INPUT, &status);
        break;
      case TT_LESSGREATER:
        redirect = parse_redirect(ps, current, RT_INOUT, &status);
        break;
      case TT_ONEGREATER:
      case TT_GREATER:
        redirect = parse_redirect(ps, current, RT_OUTPUT, &status);
        break;
      case TT_GREATERGREATER:
        redirect = parse_redirect(ps, current, RT_APPEND, &status);
        break;
      case TT_GREATERPIPE:
        redirect = parse_redirect(ps, current, RT_CLOBBER, &status);
        break;
      case TT_TWOGREATER:
        redirect = parse_redirect(ps, current, RT_ERROR, &status);
        break;
      case TT_AMPGREATER:
        redirect = parse_redirect(ps, current, RT_ALL, &status);
        break;
      case TT_LESSLESS:
      case TT_LESSAMP:
      case TT_LESSLPAREN:
      case TT_GREATERAMP:
      case TT_GREATERLPAREN:
      case TT_EQUAL:
        TODO;
        return -1;
      default:
        break;
    }

    if (redirect != NULL) {
      if (status == -1) {
        return false;
      }

      list_add_next(redirects, redirect);
    }
    current = current->next;
  }

  return true;
}

command_t *parse_simple_command(parse_state_t *ps)
{
  struct Token *current = ps->tokens_list;
  command_t *cmd = xcalloc(1, sizeof(command_t));
  size_t argc = 0;

  cmd->redirects = list_create();
  cmd->terminator = MT_NONE;
  cmd->pid = -1;

  if (!parse_redirects(ps, cmd->redirects)) {
    errln("Invalid redirect");
    return NULL;
  }

  while (current != NULL) {
    switch (current->type) {
      case TT_IF:    case TT_THEN: case TT_ELSE: case TT_ELIF: case TT_FI:
      case TT_DO:    case TT_DONE: case TT_CASE: case TT_ESAC: case TT_WHILE:
      case TT_UNTIL: case TT_FOR:  case TT_IN:   case TT_FUNCTION: case TT_TEXT:
        cmd->argv[argc++] = current->text;
        struct Token *consumed_token = consume_token(&ps, current);
        current = current->next;
        xfree(consumed_token);
        break;
      case TT_AMP:
        consume_token(&ps, current);
        cmd->terminator = MT_BACKGROUND;
        cmd->argc = argc;
        return cmd;
      case TT_SEMICOLON:
        consume_token(&ps, current);
        cmd->terminator = MT_SEQUENCE;
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

pipeline_t *parse_pipeline(parse_state_t *ps)
{
  parse_state_t *pipeline_ps = xcalloc(1, sizeof(parse_state_t));
  pipeline_t *pipeline = xcalloc(1, sizeof(pipeline_t));
  int pipe_count = 0;

  /* LinkedList iterators */
  struct Token *current_token = ps->tokens_list, *current_subtoken = NULL;
  command_t *current_command = NULL;

  while (current_token != NULL)
  {
    if (current_token->type == TT_PIPE)
    {
      struct Token *consumed_token = consume_token(&ps, current_token);
      current_token = current_token->next;
      xfree(consumed_token);

      if (current_command == NULL)
        pipeline->commands = current_command = parse_simple_command(pipeline_ps);
      else {
        current_command->next = parse_simple_command(pipeline_ps);
        current_command = current_command->next;
      }

      pipe_count++;
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

  xfree(pipeline_ps);

  pipeline->type = AOT_NONE;
  pipeline->pipe_count = pipe_count;
  return pipeline;
}

andor_t *parse_andor(parse_state_t *ps)
{
  parse_state_t *andor_ps = xcalloc(1, sizeof(parse_state_t));
  andor_t *andor = xcalloc(1, sizeof(andor_t));
  struct Token *current_token = ps->tokens_list, *current_subtoken = NULL;
  pipeline_t *current_pipeline = NULL;

  while (current_token != NULL)
  {
    if (current_token->type == TT_AMPAMP || current_token->type == TT_PIPEPIPE)
    {
      struct Token *consumed_token = consume_token(&ps, current_token);
      current_token = current_token->next;

      if (current_pipeline == NULL)
        andor->pipelines = current_pipeline = parse_pipeline(andor_ps);
      else {
        current_pipeline->next = parse_pipeline(andor_ps);
        current_pipeline = current_pipeline->next;
      }

      if (consumed_token->type == TT_AMPAMP)
        current_pipeline->type = AOT_AND;
      else
        current_pipeline->type = AOT_OR;

      xfree(consumed_token);
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
  xfree(andor_ps);
  return andor;
}

node_t *parse_to_node(parse_state_t *ps)
{
  node_t *node = (node_t*) xcalloc(1, sizeof(node_t));
  node->type = ps->type;

  switch (ps->type)
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
      xfree(node);
      return NULL;
  }

  return node;
}

int scan_tokens_for_andor(parse_state_t *ps)
{
  struct Token *current = ps->tokens_list;

  while (current != NULL)
  {
    if (current->type == TT_AMPAMP || current->type == TT_PIPEPIPE)
      return 1;

    current = current->next;
  }

  return 0;
}

int scan_tokens_for_pipeline(parse_state_t *ps)
{
  struct Token *current = ps->tokens_list;

  while (current != NULL)
  {
    if (current->type == TT_PIPE)
      return 1;

    current = current->next;
  }

  return 0;
}

node_type_e scan_tokens_for_node_type(parse_state_t *ps)
{
  switch (ps->tokens_list->type) {
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
      errln("Illegal symbol found");
      return NT_ERROR;
  }
}

tree_t *parse(struct Token *tokens_list)
{
  tree_t *tree = xcalloc(1, sizeof(tree_t));
  node_t *head_node = NULL, *current = NULL;
  parse_state_t ps = {
    .tokens_list = tokens_list,
    .type = NT_ERROR,
    .index = 0,
  };


  while (ps.tokens_list != NULL) {
    if ((ps.type = scan_tokens_for_node_type(&ps)) == NT_ERROR) {
      return NULL;
    }

    if (head_node == NULL) {
      head_node = current = parse_to_node(&ps);
    } else {
      current->next = parse_to_node(&ps);
      current = current->next;
    }
  }

  tree->nodes = head_node;
  return tree;
}

/* To String Functions */

char *terminator_to_string(terminator_type_e tt)
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

char *andor_type_to_string(andor_type_e aot)
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

void command_to_string(command_t *cmd, int space)
{
  printf("%*sSimple Command\n", space, "");
  printf("%*sargc: %ld\n", space + 1, "", cmd->argc);
  printf("%*sargv: ", space + 1, "");

  for (size_t i = 0; i < cmd->argc; i++)
    printf("[%s]", cmd->argv[i]);
  printf("\n");

  printf("%*sredirects: ", space + 1, "");

  if (cmd->redirects == NULL)
    printf("(NULL)");

  redirect_t *r;
  for (int i = 0; i < list_size(cmd->redirects); i++) {
    r = (redirect_t*) list_get(cmd->redirects, i);
    printf("[%d:%s]", r->type, r->file);
  }
  printf("\n");
  printf("%*sterminator: %s\n", space + 1, "", terminator_to_string(cmd->terminator));
}

void pipeline_to_string(pipeline_t *pipeline, int space)
{
  command_t *current = pipeline->commands;
  printf("%*sPipeline\n", space, "");
  printf("%*spipe count: %d\n", space + 1, "", pipeline->pipe_count);
  printf("%*sand/or: %s\n", space + 1, "", andor_type_to_string(pipeline->type));

  while (current != NULL)
  {
    command_to_string(current, space + 1);
    current = current->next;
  }
}

void andor_to_string(andor_t *andor)
{
  pipeline_t *current = andor->pipelines;
  printf("  AndOr\n");

  while (current != NULL)
  {
    pipeline_to_string(current, 3);
    current = current->next;
  }
}

void tree_to_string(tree_t *tree)
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

  node_t *current = tree->nodes;
  while (current != NULL)
  {
    if (current->command == NULL)
    {
      printf("  (NULL)\n");
      return;
    }

    switch (current->type)
    {
      case NT_SIMPLE_COMMAND:
        command_to_string(current->command, 2);
        break;
      case NT_PIPELINE:
        pipeline_to_string(current->pipeline, 2);
        break;
      case NT_ANDOR:
        andor_to_string(current->andor);
        break;
      default:
        printf("  Node not supported\n");
        return;
    }
    current = current->next;
  }
}
