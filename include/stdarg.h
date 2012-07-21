/*
 * stdarg.h -- defines interface for GCC built-in variadic functions support
 */
#ifndef STDARG_H
#define STDARG_H

/*
 * See man 3 stdarg for description of these macros and type.
 */

#define va_start(ap, last)  __builtin_va_start(ap, last)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)
#define va_end(ap)          __builtin_va_end(ap)
#define va_copy(dest, src)  __builtin_va_end(dest, src)

typedef __builtin_va_list va_list;

#endif /* end of include guard: STDARG_H */
