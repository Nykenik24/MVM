#include "mvm/assembler/assembler.h"
#include "mvm/assembler/lexer.h"
#include "mvm/error.h"
#include "mvm/vm.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
const char *strop[] = {
    [OP_HALT] = "HALT",
    [OP_LD] = "LD",
    [OP_LDI] = "LDI",
    [OP_ST] = "ST",
    [OP_ADD] = "ADD",
    [OP_ADDI] = "ADDI",
    [OP_SUB] = "SUB",
    [OP_SUBI] = "SUBI",
    [OP_MUL] = "MUL",
    [OP_MULI] = "MULI",
    [OP_DIV] = "DIV",
    [OP_DIVI] = "DIVI",
    [OP_CGR] = "CGR",
    [OP_CLO] = "CLO",
    [OP_CEQ] = "CEQ",
    [OP_CNE] = "CNE",
    [OP_JMP] = "JMP",
    [OP_JT] = "JT",
    [OP_JF] = "JF",
    [OP_JZ] = "JZ",
    [OP_JNZ] = "JNZ",
    [OP_JN] = "JN",
    [OP_JNN] = "JNN",
    [OP_PUTN] = "PUTN",
    [OP_PUTS] = "PUTS",
    [OP_NEWL] = "NEWL",
    [OP_JPT] = "JPT"
};
// clang-format on

uint16_t *assemble(mvm_asm_token_list *tokens, size_t *code_len) {
  uint16_t *code = malloc(CODE_MAX);
  size_t codei = 0;
  for (size_t i = 0; i < tokens->tk_num; i++) {
    mvm_asm_token *token = tokens->tokens[i];
    switch (token->kind) {
    case T_OP: {
      int op_exists = 0;
      for (size_t j = 1; j < OP_COUNT; j++) {
        if (strcmp(strop[j], token->text) == 0) {
          code[codei] = j;
          codei++;
          op_exists = 1;
          break;
        }
      }

      if (!op_exists) {
        mvm_errno = MVM_UNRECOGNIZED_OPERATION;
        errprint("'%s'", token->text);
        exit(1);
      }

      break;
    }
    case T_NUM: {
      code[codei] = atoi(token->text);
      codei++;
      break;
    }
    case T_COMMA: {
      break;
    }
    case T_CHAR: {
      char c = token->text[0];
      code[codei] = (uint16_t)c;
      codei++;
      break;
    }
    }
  }

  *code_len = codei;
  return code;
}
