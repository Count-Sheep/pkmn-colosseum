/**
 * @file stdarg.h
 * @brief MSL <stdarg.h> for PowerPC EABI (variadic argument access).
 *
 * Layout and macros follow the MSL header as reproduced by the Melee
 * decompilation (doldecomp/melee, src/MSL/stdarg.h); __va_arg lives in the
 * runtime.
 */
#ifndef CRT_STDARG_H
#define CRT_STDARG_H

typedef struct {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} __va_list[1];
typedef __va_list va_list;

extern void __builtin_va_info(void*);

void* __va_arg(va_list v_list, unsigned char type);

#define va_start(ap, fmt) ((void) fmt, __builtin_va_info(&ap))
#define va_arg(ap, t) (*((t*) __va_arg(ap, _var_arg_typeof(t))))
#define va_end(ap) (void) 0

#endif /* CRT_STDARG_H */
