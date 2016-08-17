/******************************************************************************
 *
 * Name: aclinux.h - OS specific defines, etc. for Linux
 *
 *****************************************************************************/

/*
 * Copyright (C) 2000 - 2016, Intel Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    substantially similar to the "NO WARRANTY" disclaimer below
 *    ("Disclaimer") and any redistribution must be conditioned upon
 *    including a substantially similar Disclaimer requirement for further
 *    binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef __ACWHEEL_H__
#define __ACWHEEL_H__

/* Common (in-kernel/user-space) ACPICA configuration */
#ifdef ACPI_USE_STANDARD_HEADERS
#undef ACPI_USE_STANDARD_HEADERS
#endif
#ifdef ACPI_USE_DO_WHILE_0
#undef ACPI_USE_DO_WHILE_0
#endif
#define ACPI_USE_SYSTEM_CLIBRARY
#define ACPI_USE_SYSTEM_INTTYPES

// Component Selection
#ifdef ACPI_DISASSEMBLER
#undef ACPI_DISASSEMBLER
#endif
#ifdef ACPI_DEBUGGER
#undef ACPI_DEBUGGER
#endif
#ifdef ACPI_DEBUG_OUTPUT
#undef ACPI_DEBUG_OUTPUT
#endif
#ifdef ACPI_REDUCED_HARDWARE
#undef ACPI_REDUCED_HARDWARE
#endif

/* Kernel specific ACPICA configuration */

#include <wheel.h>
#include <lib/ctype.h>
#include <lib/string.h>

typedef int BOOLEAN;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;

#define ACPI_INIT_FUNCTION __init

#define ACPI_MACHINE_WIDTH          64
#define ACPI_EXPORT_SYMBOL(symbol)

#define ACPI_CACHE_T                int
#define ACPI_SPINLOCK               int
#define ACPI_CPU_FLAGS              unsigned long

#define ACPI_NO_ERROR_MESSAGES

#endif /* __ACWHEEL_H__ */
