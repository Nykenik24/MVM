#include "mvm/error.h"
#include <stdarg.h>
#include <stdio.h>

MVM_ERRNO mvm_errno = MVM_NOERR;

const char *mvm_errstr[] = {
    [MVM_NOERR] = "Used 'errprint', but there was no error",
    [MVM_UNRECOGNIZED_OPERATION] = "Operation not recognized",
    [MVM_OFFSET_OUT_OF_BOUNDS] = "Offset out of bounds",
    [MVM_REGISTER_OUT_OF_BOUNDS] = "Register out of bounds (max is 7)",
    [MVM_CODE_LEN_OUT_OF_BOUNDS] =
        "Loaded code surpasses the set code length limit",
    [MVM_OUT_OF_USEMEM_BOUNDS] = "Index is out of usable memory bounds",
    [MVM_DIVIDE_BY_0] = "Can't divide by 0",
    [MVM_JPT_DOESNT_EXIST] = "Tried to jump to a jump point that doesn't exist",
    [MVM_ERROR_WHEN_CREATING] = "(INTERNAL) Error when creating/allocating",
    [MVM_LEX_UNKNOWN_CHAR] = "Unknown character",
    [MVM_LOAD_SOURCE_ERROR] = "Couldn't load source",
    [MVM_NOT_ENOUGH_ARGS] = "Not enough arguments",
    [MVM_REACHED_JPT_LIMIT] = "Reached jump point limit"};

void errprint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  printf("(MVM) Error: %s: ", mvm_errstr[mvm_errno]);
  vprintf(fmt, args);
  printf("\n");
}
