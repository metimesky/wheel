/******************************************************************************
 *
 * Name: acwheel.h - OS specific defines, etc. for Wheel
 *
 *****************************************************************************/

#ifndef __ACWHEEL_H__
#define __ACWHEEL_H__

// component selection

#undef ACPI_DISASSEMBLER
#undef ACPI_DEBUGGER
#undef ACPI_REDUCED_HARDWARE

// data types

// typedef uint32_t* ACPI_SPINLOCK;
// typedef uint32_t* ACPI_MUTEX;

#include <utilities/env.h>
#include <utilities/clib.h>

typedef uint32_t ACPI_SEMAPHORE;

// use C library functions provided by Wheel
#define ACPI_USE_SYSTEM_CLIBRARY

// standard C library headers should be included via env.h
#undef ACPI_USE_STANDARD_HEADERS

// we provide int types
#define ACPI_USE_SYSTEM_INTTYPES

typedef int BOOLEAN;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;

// diable debugging
#undef ACPI_DISASSEMBLER
#undef ACPI_DEBUGGER
#undef ACPI_DEBUG_OUTPUT    // !!
#undef ACPI_DBG_TRACK_ALLOCATIONS

#undef ACPI_MUTEX_DEBUG

#undef ACPI_USE_DO_WHILE_0

#define ACPI_NO_ERROR_MESSAGES  // get rid of error messages
// #define ACPI_NO_MEM_ALLOCATIONS

// we have SLAB, but use ACPICA's internal cache anyway
#define ACPI_CACHE_T                ACPI_MEMORY_LIST
#define ACPI_USE_LOCAL_CACHE        1

// #define ACPI_REDUCED_HARDWARE 1

#define ACPI_MACHINE_WIDTH          64

#include "acgcc.h"
// #include "acclang.h"

#endif /* __ACWHEEL_H__ */
