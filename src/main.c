#include "mvm.h"
#include "mvm/util/logger.h"
#include "mvm/vm.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
#ifdef DEBUG
  printf(MVM_BUILD "\n");
  printf("By " MVM_AUTHORS "\n");
  printf("\n\n");
#endif

  mvm_vm *vm = new_vm();
  uint16_t code[] = {OP_LD, REG_R0, 1, OP_HALT};
  size_t code_len = sizeof(code) / sizeof(code[0]);
  vm_load(vm, code, code_len);
  vm_loop(vm);

  mvm_reg_dump(vm);
  mvm_mem_dump(vm, code_len);

  free_vm(vm);
  return 0;
}
