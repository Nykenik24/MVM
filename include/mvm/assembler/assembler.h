#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include "mvm/assembler/lexer.h"
#include "mvm/vm.h"

extern const char *strop[];

uint32_t *assemble(mvm_asm_token_list *tokens, size_t *code_len);

#endif
