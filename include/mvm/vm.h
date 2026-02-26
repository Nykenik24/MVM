#ifndef _VM_H
#define _VM_H

#include <stddef.h>
#include <stdint.h>
#define MEMORY_MAX (1 << 16)

typedef enum {
  REG_R0,
  REG_R1,
  REG_R2,
  REG_R3,
  REG_R4,
  REG_R5,
  REG_R6,
  REG_R7,
  REG_PC,   /* Program counter */
  REG_CND,  /* Condition */
  REG_COUNT /* Total number of registers */
} MVM_REG;

static const char *regstr[] = {[REG_R0] = "R0",
                               [REG_R1] = "R1",
                               [REG_R2] = "R2",
                               [REG_R3] = "R3",
                               [REG_R4] = "R4",
                               [REG_R5] = "R5",
                               [REG_R6] = "R6",
                               [REG_R7] = "R7",
                               [REG_PC] = "PC",
                               [REG_CND] = "CND",
                               [REG_COUNT] = "Register count"};

typedef enum { OP_HALT = 0x1, OP_LD, OP_ST, OP_COUNT } MVM_OP;

typedef struct {
  uint16_t memory[MEMORY_MAX];
  uint16_t reg[REG_COUNT];
} mvm_vm;

mvm_vm *new_vm();
void free_vm(mvm_vm *vm);

void vm_load(mvm_vm *vm, uint16_t code[], const size_t len);
int run_curr_op(mvm_vm *vm);
int vm_loop(mvm_vm *vm);

#endif
