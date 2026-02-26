#ifndef _LOGGER_H
#define _LOGGER_H

#include "mvm/vm.h"
#include <stddef.h>

typedef enum { LOG_INFO, LOG_DEBUG, LOG_WARN, LOG_ERROR } log_level_t;

void mvm_log(log_level_t log_level, const char *fmt, ...);

void mvm_reg_dump(mvm_vm *vm);
void mvm_mem_dump(mvm_vm *vm, size_t quantity);

#endif
