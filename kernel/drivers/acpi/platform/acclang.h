#ifndef __ACCLANG_H__
#define __ACCLANG_H__ 1

#define COMPILER_DEPENDENT_INT64    int64_t
#define COMPILER_DEPENDENT_UINT64   uint64_t

#define ACPI_INLINE inline

#define ACPI_EXPORT_SYMBOL
#define ACPI_EXTERNAL_XFACE extern
#define ACPI_INTERNAL_XFACE
#define ACPI_INTERNAL_VAR_XFACE
#define ACPI_SYSTEM_XFACE
#define ACPI_PRINTF_LIKE(c) __attribute__((format(printf, c, c+1)))
#define ACPI_UNUSED_VAR __attribute__((unused))

#define COMPILER_VA_MACRO 1

#endif // __ACCLANG_H__