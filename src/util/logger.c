#include "mvm/util/logger.h"
#include "mvm/vm.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#ifdef DEBUG
static const char *loglvl_str[4] = {[LOG_INFO] = "\x1b[32mINFO\x1b[0m",
                                    [LOG_DEBUG] = "\x1b[34mDEBUG\x1b[0m",
                                    [LOG_WARN] = "\x1b[33mWARN\x1b[0m",
                                    [LOG_ERROR] = "\x1b[31mERROR\x1b[0m"};

void mvm_log(log_level_t log_level, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);

  printf("\x1b[30m" __DATE__ " " __TIME__ "\x1b[0m [");
  printf("%s", loglvl_str[log_level]);
  printf("]: ");
  vprintf(fmt, list);
  printf("\n");
}
#else
void mvm_log(log_level_t log_level, const char *fmt, ...) {
  (void)log_level;
  (void)fmt;
};
#endif

void mvm_reg_dump(mvm_vm *vm) {
  for (size_t i = 0; i < REG_COUNT; i++) {
    switch (i) {

    case REG_PC:
      printf("\x1b[31mreg\x1b[0m[\x1b[32m'%s'\x1b[0m]: \x1b[33m%d "
             "\x1b[34m(%d)\x1b[0m\n",
             regstr[i], vm->reg[i] - CODE_START, vm->reg[i]);
      break;

    case REG_CND:
      printf("\x1b[31mreg\x1b[0m[\x1b[32m'%s'\x1b[0m]: \x1b[34m%s\x1b[0m\n",
             regstr[i], vm->reg[i] ? "true" : "false");
      break;

    case REG_FLAG:
      printf("\x1b[31mreg\x1b[0m[\x1b[32m'%s'\x1b[0m]: \x1b[34m%s\x1b[0m\n",
             regstr[i], vm->reg[i] == FLAG_ZERO ? "zero" : "negative");
      break;

    default:
      printf("\x1b[31mreg\x1b[0m[\x1b[32m'%s'\x1b[0m]: \x1b[33m%d\x1b[0m\n",
             regstr[i], vm->reg[i]);
      break;
    }
  }
}

void mvm_mem_dump(mvm_vm *vm, size_t start, size_t quantity) {
  printf("\x1b[31mmem\x1b[0m[\x1b[35m%lu..%lu\x1b[0m]: ", start, start + 16);
  for (size_t i = start; i < start + quantity && i < MEMORY_MAX; i++) {
    printf("\x1b[90m0x\x1b[0m%x ", vm->memory[i]);
    if (i > 0 && i % 16 == 0) {
      printf("\n");
      printf("\x1b[31mmem\x1b[0m[\x1b[35m%lu..%lu\x1b[0m]: ", i, i + 16);
    }
  }
  printf("\n");
}
