#include "mvm/vm.h"
#include "mvm/error.h"
#include "mvm/util/logger.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define PC_INC(vm) vm->reg[REG_PC]++
#define AT_PC(vm) (vm->memory[vm->reg[REG_PC]])
#define CHECK_REG(reg)                                                         \
  if (reg > REG_R7) {                                                          \
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
  vm->label_num = 0;
  vm->usemem_counter = USEMEM_START;
  return vm;
}

void free_vm(mvm_vm *vm) {
  if (vm) {
    for (size_t i = 0; i < (size_t)vm->label_num; i++) {
      if (vm->labels[i])
        free_label(vm->labels[i]);
    }
    free(vm);
  }
}

mvm_label *new_label(int pc, const char *name) {
  mvm_label *label = malloc(sizeof(mvm_label));
  label->pc = pc;
  size_t len = strlen(name);
  label->name = malloc(len + 1);
  if (!label->name) {
    free(label);
    return NULL;
  }
  memcpy(label->name, name, len + 1);
  return label;
}

void free_label(mvm_label *label) {
  if (label) {
    if (label->name)
      free(label->name);
    free(label);
  }
}

uint32_t vm_read_mem(mvm_vm *vm, int idx) {
  if ((uint32_t)idx > MEMORY_MAX) {
    mvm_errno = MVM_OUT_OF_USEMEM_BOUNDS;
    errprint("got index %d", idx);
    exit(1);
  }
  return vm->memory[idx];
}

void vm_write_mem(mvm_vm *vm, uint32_t data) {
  if (vm->usemem_counter == USEMEM_MAX) {
    vm->memory[vm->usemem_counter] = data;
  } else {
    vm->memory[vm->usemem_counter] = data;
    vm->usemem_counter++;
  }
}

void vm_load_code(mvm_vm *vm, uint32_t code[], const size_t len) {
  if (len >= MEMORY_MAX) {
    mvm_errno = MVM_CODE_LEN_OUT_OF_BOUNDS;
    errprint("length is %lu", len);
    exit(1);
  }
  for (size_t i = 0; i < len; i++) {
    vm->memory[CODE_START + i] = code[i];
  }
  vm->memory[CODE_START + len] = OP_HALT;
}

void skip_label_operand(mvm_vm *vm) {
  PC_INC(vm);
  while ((char)AT_PC(vm) != '\0') {
    PC_INC(vm);
  }
  PC_INC(vm);
}

void jump(mvm_vm *vm) {
  char dest_label[256];
  int dest_label_i = 0;

  PC_INC(vm);
  while ((char)AT_PC(vm) != '\0') {
    dest_label[dest_label_i++] = (char)AT_PC(vm);
    PC_INC(vm);
  }
  dest_label[dest_label_i] = '\0';
  PC_INC(vm);

  for (size_t i = 0; i < (size_t)vm->label_num; i++) {
    if (strcmp(vm->labels[i]->name, dest_label) == 0) {
      vm->reg[REG_PC] = vm->labels[i]->pc;
      return;
    }
  }

  mvm_errno = MVM_LABEL_DOESNT_EXIST;
  errprint("%s", dest_label);
  exit(1);
}

int run_curr_op(mvm_vm *vm) {
  int op = vm->memory[vm->reg[REG_PC]];

  switch ((MVM_OP)op) {
  case OP_LD: {
    PC_INC(vm);
    int reg = AT_PC(vm);
    PC_INC(vm);
    int offset = AT_PC(vm);
    CHECK_REG(reg);
    if (vm->reg[USEMEM_START] + offset > USEMEM_MAX) {
      mvm_errno = MVM_OFFSET_OUT_OF_BOUNDS;
      exit(1);
    }
    vm->reg[reg] = vm->memory[vm->reg[USEMEM_START] + offset];
    break;
  }
  case OP_LDI: {
    PC_INC(vm);
    int reg = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    CHECK_REG(reg);
    vm->reg[reg] = imm;
    break;
  }
  case OP_ST: {
    PC_INC(vm);
    int reg = AT_PC(vm);
    CHECK_REG(reg);
    vm_write_mem(vm, vm->reg[reg]);
    break;
  }
  case OP_ADD: {
    PC_INC(vm);
    int r0 = AT_PC(vm);
    PC_INC(vm);
    int r1 = AT_PC(vm);
    PC_INC(vm);
    int r2 = AT_PC(vm);
    CHECK_REG(r0);
    CHECK_REG(r1);
    CHECK_REG(r2);
    int result = vm->reg[r0] + vm->reg[r1];
    vm->reg[r2] = result;
    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_ADDI: {
    PC_INC(vm);
    int r0 = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    PC_INC(vm);
    int r1 = AT_PC(vm);
    CHECK_REG(r0);
    CHECK_REG(r1);
    int result = vm->reg[r0] + imm;
    vm->reg[r1] = result;
    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_SUB: {
    PC_INC(vm);
    int r0 = AT_PC(vm);
    PC_INC(vm);
    int r1 = AT_PC(vm);
    PC_INC(vm);
    int r2 = AT_PC(vm);
    CHECK_REG(r0);
    CHECK_REG(r1);
    CHECK_REG(r2);
    int result = vm->reg[r0] - vm->reg[r1];
    vm->reg[r2] = result;
    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_SUBI: {
    PC_INC(vm);
    int r0 = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    PC_INC(vm);
    int r1 = AT_PC(vm);
    CHECK_REG(r0);
    CHECK_REG(r1);
    int result = vm->reg[r0] - imm;
    vm->reg[r1] = result;
    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_MUL: {
    PC_INC(vm);
    int r0 = AT_PC(vm);
    PC_INC(vm);
    int r1 = AT_PC(vm);
    PC_INC(vm);
    int r2 = AT_PC(vm);
    CHECK_REG(r0);
    CHECK_REG(r1);
    CHECK_REG(r2);
    int result = vm->reg[r0] * vm->reg[r1];
    vm->reg[r2] = result;
    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_MULI: {
    PC_INC(vm);
    int r0 = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    PC_INC(vm);
    int r1 = AT_PC(vm);
    CHECK_REG(r0);
    CHECK_REG(r1);
    int result = vm->reg[r0] * imm;
    vm->reg[r1] = result;
    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_DIV: {
    PC_INC(vm);
    int r0 = AT_PC(vm);
    PC_INC(vm);
    int r1 = AT_PC(vm);
    PC_INC(vm);
    int r2 = AT_PC(vm);
    CHECK_REG(r0);
    CHECK_REG(r1);
    CHECK_REG(r2);
    if (vm->reg[r1] == 0)
      exit(1);
    int result = vm->reg[r0] / vm->reg[r1];
    vm->reg[r2] = result;
    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_DIVI: {
    PC_INC(vm);
    int r0 = AT_PC(vm);
    PC_INC(vm);
    int imm = AT_PC(vm);
    PC_INC(vm);
    int r1 = AT_PC(vm);
    CHECK_REG(r0);
    CHECK_REG(r1);
    if (imm == 0)
      exit(1);
    int result = vm->reg[r0] / imm;
    vm->reg[r1] = result;
    FLAG_Z(vm, result);
    FLAG_N(vm, result);
    break;
  }
  case OP_CEQ: {
    PC_INC(vm);
    int r0 = AT_PC(vm);
    PC_INC(vm);
    int r1 = AT_PC(vm);
    CHECK_REG(r0);
    CHECK_REG(r1);
    vm->reg[REG_CND] = vm->reg[r0] == vm->reg[r1];
    break;
  }
  case OP_JMP: {
    jump(vm);
    goto skip_inc;
  }
  case OP_JT: {
    if (vm->reg[REG_CND]) {
      jump(vm);
      goto skip_inc;
    }
    skip_label_operand(vm);
    break;
  }
  case OP_JF: {
    if (!vm->reg[REG_CND]) {
      jump(vm);
      goto skip_inc;
    }
    skip_label_operand(vm);
    break;
  }
  case OP_PUTN: {
    PC_INC(vm);
    int reg = AT_PC(vm);
    CHECK_REG(reg);
    printf("%d", vm->reg[reg]);
    break;
  }
  case OP_PUTS: {
    PC_INC(vm);
    int len = AT_PC(vm);
    char str[256];
    for (int i = 0; i < len; i++) {
      PC_INC(vm);
      str[i] = vm->memory[vm->reg[REG_PC]];
    }
    str[len] = '\0';
    printf("%s", str);
    break;
  }
  case OP_FLUSH: {
    printf("\n");
    break;
  }
  case OP_LBL: {
    PC_INC(vm);
    char buf[256];
    int i = 0;
    while ((char)AT_PC(vm) != '\0') {
      buf[i++] = (char)AT_PC(vm);
      PC_INC(vm);
    }
    buf[i] = '\0';
    PC_INC(vm);
    vm->labels[vm->label_num++] = new_label(vm->reg[REG_PC], buf);
    goto skip_inc;
  }
  default:
    exit(1);
  }

  PC_INC(vm);

skip_inc:
  return 0;
}

int vm_loop(mvm_vm *vm) {
  while (vm->memory[vm->reg[REG_PC]] != OP_HALT) {
    run_curr_op(vm);
  }
  return 0;
}
