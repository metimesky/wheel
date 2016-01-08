/******************************************************************************
 *
 * Name: acgcc.h - GCC and Clang specific defines, etc.
 *
 *****************************************************************************/

#ifndef __ACGCC_H__
#define __ACGCC_H__

#define ACPI_INLINE             __inline__

/* Function name is used for debug output. Non-ANSI, compiler-dependent */

#define ACPI_GET_FUNCTION_NAME          __func__

/*
 * This macro is used to tag functions as "printf-like" because
 * some compilers (like GCC) can catch printf format string problems.
 */
#define ACPI_PRINTF_LIKE(c) __attribute__ ((__format__ (__printf__, c, c+1)))

/*
 * Some compilers complain about unused variables. Sometimes we don't want to
 * use all the variables (for example, _AcpiModuleName). This allows us
 * to tell the compiler warning in a per-variable manner that a variable
 * is unused.
 */
#define ACPI_UNUSED_VAR __attribute__((unused))

/*
 * Some versions of gcc implement strchr() with a buggy macro. So,
 * undef it here. Prevents error messages of this form (usually from the
 * file getopt.c):
 *
 * error: logical '&&' with non-zero constant will always evaluate as true
 */
#ifdef strchr
#undef strchr
#endif

/* GCC supports __VA_ARGS__ in macros */

#define COMPILER_VA_MACRO               1

#endif /* __ACGCC_H__ */
