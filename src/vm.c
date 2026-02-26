#include "mvm/vm.h"
#include "mvm/error.h"
#include "mvm/util/logger.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

mvm_vm *new_vm() {
  mvm_vm *vm = malloc(sizeof(mvm_vm));
  vm->reg[REG_PC] = 0;
  return vm;
}

void free_vm(mvm_vm *vm) {
  if (vm) {
    free(vm);
  }
}

void vm_load(mvm_vm *vm, uint16_t code[], const size_t len) {
  if (len >= MEMORY_MAX) {
    mvm_errno = MVM_CODE_LEN_OUT_OF_BOUNDS;
    errprint("length is %lu", len);
    exit(1);
  }

  for (size_t i = 0; i < len; i++) {
    vm->memory[i] = code[i];
  }
  vm->memory[len + 1] = OP_HALT;
}

int run_curr_op(mvm_vm *vm) {
  int op = vm->memory[vm->reg[REG_PC]];
  mvm_log(LOG_INFO, "Found OP %d, running...", op);

  switch ((MVM_OP)op) {
  case OP_LD: {
    vm->reg[REG_PC]++;
    int reg = vm->memory[vm->reg[REG_PC]];
    vm->reg[REG_PC]++;
    int pcoffset = vm->memory[vm->reg[REG_PC]];
    mvm_log(LOG_DEBUG, "Loading register '%d', pcoffset '%d'", reg, pcoffset);

    if (reg >= REG_R7) {
      mvm_errno = MVM_REGISTER_OUT_OF_BOUNDS;
      errprint("register %d", reg);
      exit(1);
    }

    if (vm->reg[REG_PC] + pcoffset > MEMORY_MAX) {
      mvm_errno = MVM_PCOFFSET_OUT_OF_BOUNDS;
      errprint("");
      exit(1);
    }

    mvm_log(LOG_DEBUG,
            "Setting register '%d' to value '%u' (memory[%d + %d = %d])", reg,
            vm->memory[vm->reg[REG_PC] + pcoffset], vm->reg[REG_PC], pcoffset,
            vm->reg[REG_PC] + pcoffset);
    vm->reg[reg] = vm->memory[vm->reg[REG_PC] + pcoffset];
    break;
  }
  default: {
    mvm_errno = MVM_UNRECOGNIZED_OPERATION;
    errprint("operation %d", op);
    exit(1);
    break;
  }
  }

  return 0;
}

int vm_loop(mvm_vm *vm) {
  while (vm->memory[vm->reg[REG_PC]] != OP_HALT) {
    run_curr_op(vm);
  }
  return 0;
}
