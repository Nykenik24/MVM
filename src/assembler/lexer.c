#include "mvm/assembler/lexer.h"
#include "mvm/error.h"
// #include <ctype.h>
#include <stdlib.h>
#include <string.h>

const char *kindstr[] = {
    [T_OP] = "Operation", [T_NUM] = "Number", [T_COMMA] = "Comma"};

int is_cap_alpha(char c) { return ((c >= 'A' && c <= 'Z') || c == '_'); }
int is_low_alpha(char c) { return ((c >= 'a' && c <= 'z') || c == '_'); }

int is_num(char c) { return (c >= '0' && c <= '9'); }

mvm_asm_token *new_token(const char *text, mvm_asm_tk_kind kind) {
  mvm_asm_token *token = malloc(sizeof *token);
  if (!token)
    return NULL;

  size_t len = strlen(text);
  token->text = malloc(len + 1);
  if (!token->text) {
    free(token);
    return NULL;
  }
  memcpy(token->text, text, len + 1);
  token->kind = kind;
  return token;
}

mvm_asm_token_list *new_token_list(void) {
  mvm_asm_token_list *list = malloc(sizeof *list);
  if (!list)
    return NULL;

  list->tk_num = 0;
  list->tk_cap = 16;
  list->tokens = calloc(list->tk_cap, sizeof *list->tokens);
  if (!list->tokens) {
    free(list);
    mvm_errno = MVM_ERROR_WHEN_CREATING;
    errprint("token list");
    exit(1);
  }
  return list;
}

void token_list_push(mvm_asm_token_list *list, const char *text,
                     mvm_asm_tk_kind kind) {
  if (list->tk_num >= list->tk_cap) {
    size_t new_cap = list->tk_cap * 2;
    mvm_asm_token **new_arr = realloc(list->tokens, new_cap * sizeof *new_arr);
    if (!new_arr) {
      mvm_errno = MVM_ERROR_WHEN_CREATING;
      errprint("new token list");
      exit(1);
      return;
    }
    list->tokens = new_arr;
    list->tk_cap = new_cap;
  }

  mvm_asm_token *tok = new_token(text, kind);
  if (!tok) {
    mvm_errno = MVM_ERROR_WHEN_CREATING;
    errprint("token");
    exit(1);
    return;
  }

  list->tokens[list->tk_num++] = tok;
}

void free_token(mvm_asm_token *token) {
  if (!token)
    return;
  free(token->text);
  free(token);
}

void free_token_list(mvm_asm_token_list *list) {
  if (!list)
    return;
  for (size_t i = 0; i < list->tk_num; ++i) {
    free_token(list->tokens[i]);
  }
  free(list->tokens);
  free(list);
}

mvm_asm_token_list *lex(const char *source) {
  if (!source)
    return NULL;

  mvm_asm_token_list *tokens = new_token_list();
  if (!tokens)
    return NULL;

  size_t len = strlen(source);
  size_t i = 0;

  while (i < len) {
    char c = source[i];

    if (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' ||
        c == '\v') {
      i++;
      continue;
    }

    if (c == ';') {
      while (c != '\n') {
        i++;
        c = source[i];
      }
      continue;
    }

    if (is_cap_alpha(c)) {
      char buf[256];
      size_t bufi = 0;

      while (i < len && is_cap_alpha(source[i])) {
        if (bufi + 1 < sizeof buf) {
          buf[bufi++] = source[i];
        }
        i++;
      }

      buf[bufi] = '\0';
      token_list_push(tokens, buf, T_OP);
      continue;
    }

    if (is_num(c)) {
      char buf[256];
      size_t bufi = 0;

      while (i < len && is_num(source[i])) {
        if (bufi + 1 < sizeof buf) {
          buf[bufi++] = source[i];
        }
        i++;
      }

      buf[bufi] = '\0';
      token_list_push(tokens, buf, T_NUM);
      continue;
    }

    if (c == ',') {
      char buf[2] = {',', '\0'};
      token_list_push(tokens, buf, T_COMMA);
      i++;
      continue;
    }

    if (c == '\'') {
      i++;
      c = source[i];
      char buf[2] = {c, '\0'};
      i++;
      if (source[i] != '\'') {
        mvm_errno = MVM_LEX_UNKNOWN_CHAR;
        errprint("%c (instead of single quote in char token)");
        exit(1);
      }
      i++;
      token_list_push(tokens, buf, T_CHAR);
      continue;
    }

    if (is_low_alpha(c)) {
      char buf[256];
      size_t bufi = 0;

      while (i < len && is_low_alpha(source[i])) {
        if (bufi + 1 < sizeof buf) {
          buf[bufi++] = source[i];
        }
        i++;
      }

      buf[bufi] = '\0';
      token_list_push(tokens, buf, T_ID);
      continue;
    }

    mvm_errno = MVM_LEX_UNKNOWN_CHAR;
    errprint("%c", c);
    exit(1);
  }

  return tokens;
}
