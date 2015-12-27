// this file defines functions that are required by ACPICA

#include <common/stdhdr.h>
#include "acpi.h"
#include "acpica/acpi.h"

// #define ACPI_STATUS int
// #define ACPI_PHYSICAL_ADDRESS int
// #define ACPI_PREDEFINED_NAMES char*
// #define ACPI_STRING char*

ACPI_STATUS AcpiOsInitialize(void) {
	return 0;
}

ACPI_STATUS AcpiOsTerminate(void) {
	return 0;
}

// return the physical address of RSDP table
ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void) {
    return 0;
}

// allow the host os to override a predefined ACPI object
ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *PredefinedObject, ACPI_STRING *NewValue) {
    return 0;
}

ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *ExistingTable, ACPI_TABLE_HEADER **NewTable) {}

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS PhysicalAddress, ACPI_SIZE Length) {}

void AcpiOsUnmapMemory(void *where, ACPI_SIZE length) {}

ACPI_STATUS AcpiOsGetPhysicalAddress(void *LogicalAddress, ACPI_PHYSICAL_ADDRESS *PhysicalAddress) {}

void *AcpiOsAllocate(ACPI_SIZE Size) {}

void AcpiOsFree(void *Memory) {}

BOOLEAN AcpiOsReadable(void *Memory, ACPI_SIZE Length) {}

BOOLEAN AcpiOsWritable(void *Memory, ACPI_SIZE Length) {}

#define ACPI_CACHE_T                ACPI_MEMORY_LIST
#define ACPI_USE_LOCAL_CACHE        1

ACPI_THREAD_ID AcpiOsGetThreadId() {}

ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE Type, ACPI_OSD_EXEC_CALLBACK Function, void *Context) {}

void AcpiOsSleep(UINT64 Milliseconds) {}

void AcpiOsStall(UINT32 Microseconds) {}

// ACPI_STATUS AcpiOsCreateMutex(ACPI_MUTEX *OutHandle) {}

// void AcpiOsDeleteMutex(ACPI_MUTEX Handle) {}

// ACPI_STATUS AcpiOsAcquireMutex(ACPI_MUTEX Handle, UINT16 Timeout) {}

// ACPI_STATUS AcpiOsCreateSemaphore(UINT32 MaxUnits, UINT32 InitialUnits, ACPI_SEMAPHORE *OutHandle) {}

// ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle) {}

// ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units, UINT16 Timeout) {}

// ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units) {}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *OutHandle) {}

void AcpiOsDeleteLock(ACPI_HANDLE Handle) {}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle) {}

void AcpiOsReleaseLock(ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags) {}

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 InterruptLevel, ACPI_OSD_HANDLER Handler, void *Context) {}

ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 InterruptNumber, ACPI_OSD_HANDLER Handler) {}