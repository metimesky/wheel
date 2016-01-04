/******************************************************************************
 *
 * Name: aclinux.h - OS specific defines, etc. for Linux
 *
 *****************************************************************************/

/*
 * Copyright (C) 2000 - 2015, Intel Corp.
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

#include <utilities/env.h>
#include <utilities/clib.h>

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

//#undef DEFINE_ACPI_GLOBALS
// #define ACPI_GLOBAL(t,a) t a
// #define ACPI_INIT_GLOBAL(t,a,b) t a = (b)

#define ACPI_MACHINE_WIDTH          64

// OSL functions
/*
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsInitialize
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsTerminate
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetRootPointer
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPredefinedOverride
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsTableOverride
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPhysicalTableOverride
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCreateLock
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsDeleteLock
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsAcquireLock
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReleaseLock
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCreateSemaphore
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsDeleteSemaphore
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWaitSemaphore
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsSignalSemaphore
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCreateMutex
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsDeleteMutex
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsAcquireMutex
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReleaseMutex
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsAllocate
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsAllocateZeroed
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsFree
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsMapMemory
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsUnmapMemory
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetPhysicalAddress
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCreateCache
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsDeleteCache
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPurgeCache
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsAcquireObject
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReleaseObject
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsInstallInterruptHandler
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsRemoveInterruptHandler
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetThreadId
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsExecute
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWaitEventsComplete
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsSleep
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsStall
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReadPort
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWritePort
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReadMemory
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWriteMemory
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReadPciConfiguration
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWritePciConfiguration
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReadable
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWritable
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTimer
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsSignal
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPrintf
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsVprintf
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsRedirectOutput
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetLine
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTableByName
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTableByIndex
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetTableByAddress
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsOpenDirectory
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetNextFilename
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCloseDirectory
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsOpenFile
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsCloseFile
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsReadFile
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsWriteFile
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsGetFileOffset
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsSetFileOffset
#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsTracePoint
*/
#include "acgcc.h"

#endif /* __ACWHEEL_H__ */
