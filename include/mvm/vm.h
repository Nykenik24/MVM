#ifndef _VM_H
#define _VM_H

#include <stddef.h>
#include <stdint.h>
#define MEMORY_MAX UINT16_MAX
#define CODE_START (MEMORY_MAX / 2 + 1)
#define CODE_MAX MEMORY_MAX - CODE_START
#define USEMEM_START 0
#define USEMEM_MAX MEMORY_MAX / 2
#define MAX_LABELS 256

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
  REG_FLAG, /* Flag */
  REG_COUNT /* Total number of registers */
} MVM_REG;

typedef enum { FLAG_ZERO = 1 << 1, FLAG_NEG = 1 << 2 } MVM_FLAG;

extern const char *regstr[];

typedef enum {
  OP_HALT = 0x1,
  OP_LD,    /* Load */
  OP_LDI,   /* Load immediate */
  OP_ST,    /* Store */
  OP_ADD,   /* Add */
  OP_ADDI,  /* Add immediate */
  OP_SUB,   /* Subtract */
  OP_SUBI,  /* Subtract immediate */
  OP_MUL,   /* Multiply */
  OP_MULI,  /* Multiply immediate */
  OP_DIV,   /* Divide */
  OP_DIVI,  /* Divide immediate */
  OP_CGR,   /* Compare greater */
  OP_CLO,   /* Compare lower */
  OP_CGE,   /* Compare greater or equal */
  OP_CLE,   /* Compare lower or equal */
  OP_CEQ,   /* Compare equal */
  OP_CNE,   /* Compare NOT equal */
  OP_JMP,   /* Jump */
  OP_JT,    /* Jump if true*/
  OP_JF,    /* Jump if false */
  OP_JZ,    /* Jump if zero */
  OP_JNZ,   /* Jump if NOT zero */
  OP_JN,    /* Jump if negative */
  OP_JNN,   /* Jump if NOT negative */
  OP_PUTN,  /* Put number */
  OP_PUTS,  /* Put string */
  OP_FLUSH, /* Put newline */
  OP_LBL,   /* Define a new label */
  OP_COUNT
} MVM_OP;

typedef struct {
  int pc;
  char *name;
} mvm_label;

typedef struct {
  uint32_t memory[MEMORY_MAX];
  uint32_t reg[REG_COUNT];
  int usemem_counter;

  mvm_label *labels[MAX_LABELS];
  // Last label's index
  int label_num;
} mvm_vm;

mvm_vm *new_vm();
void free_vm(mvm_vm *vm);

mvm_label *new_label(int pc, const char *name);
void free_label(mvm_label *label);

uint32_t vm_read_mem(mvm_vm *vm, int idx);
void vm_write_mem(mvm_vm *vm, uint32_t data);

void vm_load_code(mvm_vm *vm, uint32_t code[], const size_t len);
int run_curr_op(mvm_vm *vm);
int vm_loop(mvm_vm *vm);

#endif
