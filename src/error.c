#include "mvm/error.h"
#include <stdarg.h>
#include <stdio.h>

void errprint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  printf("(MVM) Error: %s: ", mvm_errstr[mvm_errno]);
  vprintf(fmt, args);
  printf("\n");
}
