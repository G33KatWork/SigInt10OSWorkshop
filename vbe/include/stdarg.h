#ifndef _STDARG_H_
#define _STDARG_H_

// Use GCC builtin va implementation

typedef __builtin_va_list va_list;

#define va_start(LIST, ARG)     __builtin_va_start (LIST, ARG)
#define va_end(LIST)            __builtin_va_end (LIST)
#define va_arg(LIST, TYPE)      __builtin_va_arg (LIST, TYPE)
#define va_copy(DST, SRC)       __builtin_va_copy (DST, SRC)

#endif
