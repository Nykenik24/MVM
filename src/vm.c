#include "mvm/vm.h"
#include "mvm/error.h"
#include "mvm/util/logger.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define PC_INC(vm) vm->reg[REG_PC]++
#define AT_PC(vm) (vm->memory[vm->reg[REG_PC]])
#define CHECK_REG(reg)                                                         \
  if (reg >= REG_R7) {                                                         \
    mvm_errno = MVM_REGISTER_OUT_OF_BOUNDS;                                    \
    errprint("got register %d", reg);                                          \
    exit(1);                                                                   \
  }
#define SET_FLAG(vm, f) vm->reg[REG_FLAG] = f
#define FLAG_Z(vm, res)                                                        \
  if (res == 0) {                                                              \
    SET_FLAG(vm, FLAG_ZERO);                                                   \
  }
#define FLAG_N(vm, res)                                                        \
  if (res < 0) {                                                               \
    SET_FLAG(vm, FLAG_NEG);                                                    \
  }

const char *regstr[] = {
    [REG_R0] = "R0",   [REG_R1] = "R1",     [REG_R2] = "R2",
    [REG_R3] = "R3",   [REG_R4] = "R4",     [REG_R5] = "R5",
    [REG_R6] = "R6",   [REG_R7] = "R7",     [REG_PC] = "PC",
    [REG_CND] = "CND", [REG_FLAG] = "FLAG", [REG_COUNT] = "Register count"};

mvm_vm *new_vm() {
  mvm_vm *vm = malloc(sizeof(mvm_vm));
  vm->reg[REG_PC] = CODE_START;
  vm->jptn = 1;
  return vm;
}

void free_vm(mvm_vm *vm) {
  if (vm) {
    free(vm);
  }
}

uint16_t vm_read_mem(mvm_vm *vm, int idx) {
  if (idx > USEMEM_MAX) {
    mvm_errno = MVM_OUT_OF_USEMEM_BOUNDS;
    errprint("got index %d", idx);
    exit(1);
  }

  return vm->memory[idx];
}

void vm_write_mem(mvm_vm *vm, uint16_t data) {
  if (vm->usemem_counter == USEMEM_MAX) {
    mvm_log(LOG_WARN,
            "Out of usable memory (in VM), overwriting last value (%d at %d).",
            vm->memory[vm->usemem_counter], vm->usemem_counter);
    vm->memory[vm->usemem_counter] = data;
  } else {
    vm->memory[vm->usemem_counter] = data;
    vm->usemem_counter++;
  }
}

void vm_load_code(mvm_vm *vm, uint16_t code[], const size_t len) {
  if (len >= CODE_MAX) {
    mvm_errno = MVM_CODE_LEN_OUT_OF_BOUNDS;
    errprint("length is %lu", len);
    exit(1);
  }

  for (size_t i = 0; i < len; i++) {
    vm->memory[CODE_START + i] = code[i];
  }
  vm->memory[CODE_START + len] = OP_HALT;
}

int run_curr_op(mvm_vm *vm) {
  int op = vm->memory[vm->reg[REG_PC]];
  mvm_log(LOG_INFO, "Found OP %d, running...", op);

  switch ((MVM_OP)op) {
  case OP_LD: {
    PC_INC(vm);
    int reg = AT_PC(vm);
    PC_INC(vm);
    int offset = AT_PC(vm);
    mvm_log(LOG_INFO, "Loading. Register '%d', offset '%d'", reg, offset);

    CHECK_REG(reg);

    if (vm->reg[USEMEM_START] + offset > USEMEM_MAX) {
      mvm_errno = MVM_OFFSET_OUT_OF_BOUNDS;
      errprint("max is %d", USEMEM_MAX);
      exit(1);
    }

    mvm_log(LOG_INFO,
            "Setting register '%d' to value '%u' (memory[%d + %d = %d])", reg,
            vm->memory[vm->reg[USEMEM_START] + offset], vm->reg[USEMEM_START],
            offset, vm->reg[USEMEM_START] + offset);
    vm->reg[reg] = vm->memory[vm->reg[USEMEM_START] + offset];
    break;
  }
  case OP_LDI: {
    PC_INC(vm);
    int reg = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    mvm_log(LOG_INFO, "Loading immediate. Register %d, IMM '%d'", reg, imm);

    CHECK_REG(reg);

    mvm_log(LOG_INFO, "Setting register '%d' to immediate value '%u'", reg,
            imm);
    vm->reg[reg] = imm;
    break;
  }
  case OP_ST: {
    PC_INC(vm);
    int reg = AT_PC(vm);
    mvm_log(LOG_INFO, "Storing register %d (value '%u') at memory[%d]", reg,
            vm->reg[reg], vm->usemem_counter);

    CHECK_REG(reg);

    vm_write_mem(vm, vm->reg[reg]);
    break;
  }
  case OP_ADD: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);
    PC_INC(vm);
    int reg2 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);
    CHECK_REG(reg2);

    int result = vm->reg[reg0] + vm->reg[reg1];
    mvm_log(LOG_INFO, "Storing R%d + R%d = %d at R%d", reg0, reg1, result,
            reg2);
    vm->reg[reg2] = result;

    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_ADDI: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);

    int result = vm->reg[reg0] + imm;
    mvm_log(LOG_INFO, "Storing R%d + %d = %d at R%d", reg0, imm, result, reg1);
    vm->reg[reg1] = result;

    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    if (result < 0) {
      vm->reg[reg1] = -result;
    }
    break;
  }
  case OP_SUB: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);
    PC_INC(vm);
    int reg2 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);
    CHECK_REG(reg2);

    int result = vm->reg[reg0] - vm->reg[reg1];
    mvm_log(LOG_INFO, "Storing R%d - R%d = %d at R%d", reg0, reg1, result,
            reg2);
    vm->reg[reg2] = result;

    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    if (result < 0) {
      vm->reg[reg2] = -result;
    }
    break;
  }
  case OP_SUBI: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);

    int result = vm->reg[reg0] - imm;
    mvm_log(LOG_INFO, "Storing R%d - %d = %d at R%d", reg0, imm, result, reg1);
    vm->reg[reg1] = result;

    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    if (result < 0) {
      vm->reg[reg1] = -result;
    }
    break;
  }
  case OP_MUL: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);
    PC_INC(vm);
    int reg2 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);
    CHECK_REG(reg2);

    int result = vm->reg[reg0] * vm->reg[reg1];
    mvm_log(LOG_INFO, "Storing R%d * R%d = %d at R%d", reg0, reg1, result,
            reg2);
    vm->reg[reg2] = result;

    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_MULI: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);

    int result = vm->reg[reg0] * imm;
    mvm_log(LOG_INFO, "Storing R%d * %d = %d at R%d", reg0, imm, result, reg1);
    vm->reg[reg1] = result;

    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_DIV: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);
    PC_INC(vm);
    int reg2 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);
    CHECK_REG(reg2);

    if (vm->reg[reg1] == 0) {
      mvm_errno = MVM_DIVIDE_BY_0;
      errprint("tried to divide %d by 0", vm->reg[reg0]);
      exit(1);
    }

    int result = vm->reg[reg0] / vm->reg[reg1];
    mvm_log(LOG_INFO, "Storing R%d / R%d = %d at R%d", reg0, reg1, result,
            reg2);
    vm->reg[reg2] = result;

    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_DIVI: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);

    int result = vm->reg[reg0] / imm;
    mvm_log(LOG_INFO, "Storing R%d / %d = %d at R%d", reg0, imm, result, reg1);
    vm->reg[reg1] = result;

    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_CGR: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);

    int result = vm->reg[reg0] > vm->reg[reg1];
    mvm_log(LOG_INFO, "Storing R%d > R%d = %s at CND", reg0, reg1,
            result ? "true" : "false");
    vm->reg[REG_CND] = result;
    break;
  }
  case OP_CLO: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);

    int result = vm->reg[reg0] < vm->reg[reg1];
    mvm_log(LOG_INFO, "Storing R%d < R%d = %s at CND", reg0, reg1,
            result ? "true" : "false");
    vm->reg[REG_CND] = result;
    break;
  }
  case OP_CEQ: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);

    int result = vm->reg[reg0] == vm->reg[reg1];
    mvm_log(LOG_INFO, "Storing R%d == R%d = %s at CND", reg0, reg1,
            result ? "true" : "false");
    vm->reg[REG_CND] = result;
    break;
  }
  case OP_CNE: {
    PC_INC(vm);
    int reg0 = AT_PC(vm);
    PC_INC(vm);
    int reg1 = AT_PC(vm);

    CHECK_REG(reg0);
    CHECK_REG(reg1);

    int result = vm->reg[reg0] != vm->reg[reg1];
    mvm_log(LOG_INFO, "Storing R%d != R%d = %s at CND", reg0, reg1,
            result ? "true" : "false");
    vm->reg[REG_CND] = result;
    break;
  }
  case OP_JMP: {
    PC_INC(vm);
    int dest = AT_PC(vm);

    if (dest < 0 || dest > vm->jptn) {
      mvm_errno = MVM_JPT_DOESNT_EXIST;
      errprint("tried to jump to %d", dest);
      exit(1);
    }

    mvm_log(LOG_INFO, "Jumping to jump point %d (at PC %d)", dest,
            vm->jpts[dest]);
    vm->reg[REG_PC] = vm->jpts[dest];
    break;
  }
  case OP_JT: {
    PC_INC(vm);
    int dest = AT_PC(vm);

    if (dest < 0 || dest > vm->jptn) {
      mvm_errno = MVM_JPT_DOESNT_EXIST;
      errprint("tried to jump to %d", dest);
      exit(1);
    }

    if (vm->reg[REG_CND] == 1) {
      mvm_log(LOG_INFO, "Jumping to jump point %d (at PC %d)", dest,
              vm->jpts[dest]);
      vm->reg[REG_PC] = vm->jpts[dest];
    } else {
      mvm_log(LOG_INFO, "Ran JT, but CND wasn't true");
    }
    break;
  }
  case OP_JF: {
    PC_INC(vm);
    int dest = AT_PC(vm);

    if (dest < 0 || dest > vm->jptn) {
      mvm_errno = MVM_JPT_DOESNT_EXIST;
      errprint("tried to jump to %d", dest);
      exit(1);
    }

    if (vm->reg[REG_CND] == 0) {
      mvm_log(LOG_INFO, "Jumping to jump point %d (at PC %d)", dest,
              vm->jpts[dest]);
      vm->reg[REG_PC] = vm->jpts[dest];
    } else {
      mvm_log(LOG_INFO, "Ran JF, but CND wasn't false");
    }
    break;
  }
  case OP_JZ: {
    PC_INC(vm);
    int dest = AT_PC(vm);

    if (dest < 0 || dest > vm->jptn) {
      mvm_errno = MVM_JPT_DOESNT_EXIST;
      errprint("tried to jump to %d", dest);
      exit(1);
    }

    if (vm->reg[REG_FLAG] == FLAG_ZERO) {
      mvm_log(LOG_INFO, "Jumping to jump point %d (at PC %d)", dest,
              vm->jpts[dest]);
      vm->reg[REG_PC] = vm->jpts[dest];
    } else {
      mvm_log(LOG_INFO, "Ran JZ, but FLAG wasn't zero");
    }
    break;
  }
  case OP_JNZ: {
    PC_INC(vm);
    int dest = AT_PC(vm);

    if (dest < 0 || dest > vm->jptn) {
      mvm_errno = MVM_JPT_DOESNT_EXIST;
      errprint("tried to jump to %d", dest);
      exit(1);
    }

    if (vm->reg[REG_FLAG] != FLAG_ZERO) {
      mvm_log(LOG_INFO, "Jumping to jump point %d (at PC %d)", dest,
              vm->jpts[dest]);
      vm->reg[REG_PC] = vm->jpts[dest];
    } else {
      mvm_log(LOG_INFO, "Ran JNZ, but FLAG was zero");
    }
    break;
  }
  case OP_JN: {
    PC_INC(vm);
    int dest = AT_PC(vm);

    if (dest < 0 || dest > vm->jptn) {
      mvm_errno = MVM_JPT_DOESNT_EXIST;
      errprint("tried to jump to %d", dest);
      exit(1);
    }

    if (vm->reg[REG_FLAG] == FLAG_NEG) {
      mvm_log(LOG_INFO, "Jumping to jump point %d (at PC %d)", dest,
              vm->jpts[dest]);
      vm->reg[REG_PC] = vm->jpts[dest];
    } else {
      mvm_log(LOG_INFO, "Ran JN, but FLAG wasn't negative");
    }
    break;
  }
  case OP_JNN: {
    PC_INC(vm);
    int dest = AT_PC(vm);

    if (dest < 0 || dest > vm->jptn) {
      mvm_errno = MVM_JPT_DOESNT_EXIST;
      errprint("tried to jump to %d", dest);
      exit(1);
    }

    if (vm->reg[REG_FLAG] != FLAG_NEG) {
      mvm_log(LOG_INFO, "Jumping to jump point %d (at PC %d)", dest,
              vm->jpts[dest]);
      vm->reg[REG_PC] = vm->jpts[dest];
    } else {
      mvm_log(LOG_INFO, "Ran JNN, but FLAG wasn negative");
    }
    break;
  }
  case OP_PUTN: {
    PC_INC(vm);
    int reg = AT_PC(vm);

    CHECK_REG(reg);

    mvm_log(LOG_INFO, "Output number %d", vm->reg[reg]);
    printf("%d", vm->reg[reg]);
    break;
  }
  case OP_PUTS: {
    PC_INC(vm);
    int len = AT_PC(vm);

    char str[256];
    for (size_t i = 0; i < (size_t)len; i++) {
      PC_INC(vm);
      str[i] = vm->memory[vm->reg[REG_PC]];
    }
    str[len + 1] = '\0';
    mvm_log(LOG_INFO, "Output string \"%s\" of length %d");
    printf("%s", str);
    break;
  }
  case OP_NEWL: {
    printf("\n");
    break;
  }
  case OP_JPT: {
    if (vm->jptn >= MAX_JPTS) {
      mvm_errno = MVM_REACHED_JPT_LIMIT;
      errprint("%d", vm->jptn);
      exit(1);
    }

    mvm_log(LOG_INFO, "Creating jump point %d at PC %d", vm->jptn,
            vm->reg[REG_PC]);
    vm->jpts[vm->jptn] = vm->reg[REG_PC];
    vm->jptn++;
    break;
  }
  default: {
    mvm_errno = MVM_UNRECOGNIZED_OPERATION;
    errprint("operation %d", op);
    exit(1);
    break;
  }
  }

  PC_INC(vm);
  return 0;
}

int vm_loop(mvm_vm *vm) {
  while (vm->memory[vm->reg[REG_PC]] != OP_HALT) {
    run_curr_op(vm);
  }
  return 0;
}
