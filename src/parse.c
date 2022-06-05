/*
 * Copyright (c) 2021-2022, Jason Long.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "debug.h"
#include "list.h"
#include "parse.h"
#include "util.h"

token_t *consume_token(parse_state_t *ps, token_t *token)
{
  int index = list_get_index(ps->tokens, token);

  if (index == -1) {
    return NULL;
  }

  return list_remove(ps->tokens, index);
}

static token_t *peek(const parse_state_t *ps)
{
  return list_get(ps->tokens, 0);
}

static redirect_t *parse_redirect(parse_state_t *ps, token_t *r, redirect_type_e type, int *status)
{
  token_t *next_token = list_get(ps->tokens, list_get_index(ps->tokens, r) + 1);
  if (next_token == NULL || next_token->type != TT_WORD) {
    *status = -1;
    return NULL;
  }

  redirect_t *redirect = (redirect_t*) xcalloc(1, sizeof(redirect_t));
  redirect->type = type;
  redirect->file = next_token->word;
  consume_token(ps, next_token);
  consume_token(ps, r);

  *status = 1;
  return redirect;
}

static bool parse_redirects(parse_state_t *ps, list_t *redirects)
{
  token_t *current = ps->tokens_list;
  redirect_t *redirect = NULL;
  int status = 0;

  while (current != NULL) {
    redirect = NULL;
    status = 0;

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

      list_append(redirects, redirect);
    }
    current = current->next;
  }

  return true;
}

command_t *parse_simple_command(parse_state_t *ps)
{
  command_t *cmd = xcalloc(1, sizeof(command_t));
  size_t argc = 0;

  cmd->redirects = list_create();
  cmd->terminator = MT_NONE;
  cmd->pid = -1;

  if (!parse_redirects(ps, cmd->redirects)) {
    errln("Invalid redirect");
    return NULL;
  }

  token_t *token = NULL;
  list_iterator_t *it = list_iterator_create(ps->tokens);

  while (list_iterator_has_next(it)) {
    token = (token_t*) list_iterator_next(it);
    switch (token->type) {
      case TT_WORD:
        // FIXME: this is probably broken...
        // cmd->argv[argc++] = token->word;
        strcpy(cmd->argv[argc++], token->word);
        break;
      case TT_AMP:
        cmd->terminator = MT_BACKGROUND;
        cmd->argc = argc;
        return cmd;
      case TT_SEMICOLON:
        cmd->terminator = MT_SEQUENCE;
        cmd->argc = argc;
        return cmd;
      default:
        errln("Simple command failed to parse");
        return NULL;
    }

    consume_token(ps, token);
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
      free(consumed_token);

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

  free(pipeline_ps);

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

node_t *parse_to_node(parse_state_t *ps)
{
  node_t *node = (node_t*) xcalloc(1, sizeof(node_t));
  node->type = ps->type;

  switch (ps->type) {
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
      if ((node->pipeline = parse_pipeline(ps)) == NULL) {
        return NULL;
      }

      break;
    case NT_SIMPLE_COMMAND:
      if ((node->command = parse_simple_command(ps)) == NULL) {
        return NULL;
      }

      break;
    case NT_WHILE:
      TODO;
    default:
      errln("Unknown node type to parse");
      free(node);
      return NULL;
  }

  return node;
}

int scan_tokens_for_andor(parse_state_t *ps)
{
  token_t *token = NULL;
  list_iterator_t *it = list_iterator_create(ps->tokens);
  while (list_iterator_has_next(it)) {
    token = (token_t*) list_iterator_next(it);
    if (token->type == TT_AMPAMP || token->type == TT_PIPEPIPE) {
      return true;
    }
  }

  return false;
}

bool scan_tokens_for_pipeline(parse_state_t *ps)
{
  list_iterator_t *it = list_iterator_create(ps->tokens);
  while (list_iterator_has_next(it)) {
    if (((token_t*) list_iterator_next(it))->type == TT_PIPE) {
      return true;
    }
  }

  return false;
}

int scan_tokens_for_next_node_type(parse_state_t *ps)
{
  token_t *token = peek(ps);
  if (token == NULL) {
    errln("Next token is NULL");
    return -1;
  }

  switch (token->type) {
    case TT_IF:
      ps->type = NT_IF;
      break;
    case TT_WHILE:
      ps->type = NT_WHILE;
      break;
    case TT_CASE:
      ps->type = NT_CASE;
      break;
    case TT_FUNCTION:
      ps->type = NT_FUNCTION;
      break;
    case TT_WORD:
      if (scan_tokens_for_andor(ps)) {
        ps->type = NT_ANDOR;
        break;
      }

      if (scan_tokens_for_pipeline(ps)) {
        ps->type = NT_PIPELINE;
        break;
      }

      ps->type = NT_SIMPLE_COMMAND;
      break;
    default:
      errln("Illegal symbol found");
      return -1;
  }

  return 0;
}

tree_t *parse(list_t *tokens)
{
  tree_t *tree = xcalloc(1, sizeof(tree_t));
  tree->nodes = list_create();

  parse_state_t ps = {
    .tokens = tokens,
    .type = NT_SIMPLE_COMMAND,
    .index = 0,
  };


//  token_t *token = NULL;
//  list_iterator_t *it = list_iterator_create(ps.tokens);
//  while (list_iterator_has_next(it)) {
//    token = (token_t*) list_iterator_next(it);
//  }

  node_t *node = NULL;

  while (list_size(ps.tokens) > 0) {
    if (scan_tokens_for_next_node_type(&ps) == -1) {
      return NULL;
    }

    if ((node = parse_to_node(&ps)) == NULL) {
      return NULL;
    }

    list_append(tree->nodes, node);
  }

//  while (ps.tokens != NULL) {
//    if (head_node == NULL) {
//      head_node = current = parse_to_node(&ps);
//    } else {
//      current->next = parse_to_node(&ps);
//      current = current->next;
//    }
//  }

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
