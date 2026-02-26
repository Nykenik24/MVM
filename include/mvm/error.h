#ifndef _MVM_ERROR_H
#define _MVM_ERROR_H

typedef enum {
  MVM_NOERR,
  MVM_UNRECOGNIZED_OPERATION,
  MVM_PCOFFSET_OUT_OF_BOUNDS,
  MVM_REGISTER_OUT_OF_BOUNDS,
  MVM_CODE_LEN_OUT_OF_BOUNDS
} MVM_ERRNO;

static MVM_ERRNO mvm_errno = MVM_NOERR;

static const char *mvm_errstr[] = {
    [MVM_NOERR] = "Used 'errprint', but there was no error",
    [MVM_UNRECOGNIZED_OPERATION] = "Operation not recognized",
    [MVM_PCOFFSET_OUT_OF_BOUNDS] = "PCOffset out of bounds",
    [MVM_REGISTER_OUT_OF_BOUNDS] = "Register out of bounds (max is 7)",
    [MVM_CODE_LEN_OUT_OF_BOUNDS] =
        "Loaded code surpasses the set code length limit."};

void errprint(const char *fmt, ...);

#endif
