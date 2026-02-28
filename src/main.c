#include "mvm.h"
#include "mvm/assembler/assembler.h"
#include "mvm/assembler/lexer.h"
#include "mvm/error.h"
#include "mvm/vm.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *get_filename_ext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}

int main(int argc, char *argv[]) {
#ifdef DEBUG
  printf(MVM_BUILD "\n");
  printf("By " MVM_AUTHORS "\n");
  printf("\n\n");
#endif

  if (argc < 2) {
    mvm_errno = MVM_NOT_ENOUGH_ARGS;
    errprint("got %d args", argc);
    return 1;
  }

  const char *path = argv[1];
  const char *ext = get_filename_ext(path);
  if (strcmp(ext, "mas")) {
    mvm_errno = MVM_LOAD_SOURCE_ERROR;
    errprint("incorrect extension, expected '.mas', got '.%s'", ext);
    return 1;
  }

  FILE *file = fopen(path, "rb");
  if (!file) {
    mvm_errno = MVM_LOAD_SOURCE_ERROR;
    errprint("could not open file %s", path);
    return 1;
  }

  fseek(file, 0L, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char *source = malloc(size + 1);
  if (!source) {
    mvm_errno = MVM_LOAD_SOURCE_ERROR;
    errprint("not enough memory to read file %s", path);
    fclose(file);
    return 1;
  }

  size_t read = fread(source, sizeof(char), size, file);
  if (read < size) {
    mvm_errno = MVM_LOAD_SOURCE_ERROR;
    errprint("could not read file %s", path);
    free(source);
    fclose(file);
    return 1;
  }

  source[read] = '\0';
  fclose(file);

  mvm_vm *vm = new_vm();

  mvm_asm_token_list *tokens = lex(source);
  // for (size_t i = 0; i < tokens->tk_num; i++) {
  //   printf("Token '%s', %s\n", tokens->tokens[i]->text,
  //          kindstr[tokens->tokens[i]->kind]);
  // }

  size_t code_len;
  uint16_t *code = assemble(tokens, &code_len);
  // for (size_t i = 0; i < code_len; i++) {
  //   printf("0x%x \n", code[i]);
  // }
  vm_load_code(vm, code, code_len);
  vm_loop(vm);

  free_token_list(tokens);
  free_vm(vm);
  return 0;
}
