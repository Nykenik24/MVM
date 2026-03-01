#ifndef _LEXER_H
#define _LEXER_H

#include <stddef.h>
#define MAX_TOKENS 0xFFFF

typedef enum { T_OP, T_NUM, T_CHAR, T_COMMA, T_ID } mvm_asm_tk_kind;

extern const char *kindstr[];

typedef struct {
  char *text;
  mvm_asm_tk_kind kind;
} mvm_asm_token;

typedef struct {
  mvm_asm_token **tokens;
  size_t tk_num;
  size_t tk_cap;
} mvm_asm_token_list;

mvm_asm_token *new_token(const char *text, mvm_asm_tk_kind kind);

mvm_asm_token_list *new_token_list(void);
void token_list_push(mvm_asm_token_list *list, const char *text,
                     mvm_asm_tk_kind kind);

void free_token(mvm_asm_token *token);
void free_token_list(mvm_asm_token_list *list);

mvm_asm_token_list *lex(const char *source);

#endif
