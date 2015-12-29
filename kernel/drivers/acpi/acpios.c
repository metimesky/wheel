// this file defines functions that are required by ACPICA
// AcpiOs*

#include <utilities/clib.h>
#include "acpi.h"
#include "acpica/acpi.h"
#include "acpica/acpiosxf.h"
#include "acpica/actbl.h"

////////////////////////////////////////////////////////////////////////////////
/// environmental and ACPI tables
////////////////////////////////////////////////////////////////////////////////

// called during initialization of the ACPICA subsystem
ACPI_STATUS AcpiOsInitialize(void) {
	// return AE_OK;
}

ACPI_STATUS AcpiOsTerminate(void) {
	// return AE_OK;
}

// return the physical address of RSDP table
ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void) {
    // ACPI_SIZE addr;
    // if (AE_OK == AcpiFindRootPointer(&addr)) {
    //     return addr;
    // }
    // return 0;
}

// allow the host os to override a predefined ACPI object
ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *PredefinedObject, ACPI_STRING *NewValue) {
    return 0;
}

ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *ExistingTable, ACPI_TABLE_HEADER **NewTable) {}

ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER *ExistingTable,
ACPI_PHYSICAL_ADDRESS *NewAddress,
UINT32 *NewTableLength) {}

////////////////////////////////////////////////////////////////////////////////
/// memory
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsCreateCache(char *cacheName, UINT16 size, UINT16 depth, ACPI_CACHE_T **ret) {}
ACPI_STATUS AcpiOsDeleteCache(ACPI_CACHE_T *cache) {}
ACPI_STATUS AcpiOsPurgeCache(ACPI_CACHE_T *cache) {}
void *AcpiOsAcquireObject(ACPI_CACHE_T *cache) {}
ACPI_STATUS AcpiOsReleaseObject(ACPI_CACHE_T *cache, void *object) {}

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS PhysicalAddress, ACPI_SIZE Length) {}
void AcpiOsUnmapMemory(void *where, ACPI_SIZE length) {}
ACPI_STATUS AcpiOsGetPhysicalAddress(void *LogicalAddress, ACPI_PHYSICAL_ADDRESS *PhysicalAddress) {}

void *AcpiOsAllocate(ACPI_SIZE Size) {}
void AcpiOsFree(void *Memory) {}

BOOLEAN AcpiOsReadable(void *Memory, ACPI_SIZE Length) {}
BOOLEAN AcpiOsWritable(void *Memory, ACPI_SIZE Length) {}

// #define ACPI_CACHE_T                ACPI_MEMORY_LIST
// #define ACPI_USE_LOCAL_CACHE        1

////////////////////////////////////////////////////////////////////////////////
/// Multithreading and Scheduling Services
////////////////////////////////////////////////////////////////////////////////

ACPI_THREAD_ID AcpiOsGetThreadId() {}
ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE Type, ACPI_OSD_EXEC_CALLBACK Function, void *Context) {}

void AcpiOsSleep(UINT64 Milliseconds) {}
void AcpiOsStall(UINT32 Microseconds) {}
void AcpiOsWaitEventsComplete(void) {}

////////////////////////////////////////////////////////////////////////////////
/// Mutual Exclusion and Synchronization
////////////////////////////////////////////////////////////////////////////////

// ACPI_STATUS AcpiOsCreateMutex(ACPI_MUTEX *OutHandle) {}
// void AcpiOsDeleteMutex(ACPI_MUTEX Handle) {}
// ACPI_STATUS AcpiOsAcquireMutex(ACPI_MUTEX Handle, UINT16 Timeout) {}
// void AcpiOsReleaseMutex(ACPI_MUTEX Handle) {}

ACPI_STATUS AcpiOsCreateSemaphore(UINT32 MaxUnits, UINT32 InitialUnits, ACPI_SEMAPHORE *OutHandle) {}
ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle) {}
ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units, UINT16 Timeout) {}
ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units) {}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *OutHandle) {}
void AcpiOsDeleteLock(ACPI_HANDLE Handle) {}
ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle) {}
void AcpiOsReleaseLock(ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags) {}

////////////////////////////////////////////////////////////////////////////////
/// Interrupt Handling
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 InterruptLevel, ACPI_OSD_HANDLER Handler, void *Context) {}
ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 InterruptNumber, ACPI_OSD_HANDLER Handler) {}

////////////////////////////////////////////////////////////////////////////////
/// Memory Access and Memory Mapped I/O
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS addr, UINT64 *val, UINT32 width) {}
ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS addr, UINT64 val, UINT32 width) {}

////////////////////////////////////////////////////////////////////////////////
/// Port Input/Output
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS addr, UINT32 *val, UINT32 width) {}
ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS addr, UINT32 val, UINT32 width) {}

////////////////////////////////////////////////////////////////////////////////
/// PCI Configuration Space Access
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID pciId, UINT32 reg, UINT64 *val, UINT32 width) {}
ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID pciId, UINT32 reg, UINT64 val, UINT32 width) {}

////////////////////////////////////////////////////////////////////////////////
/// Formatted Output
////////////////////////////////////////////////////////////////////////////////

void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf(const char *Format, ...) {}
void AcpiOsVprintf(const char *fmt, va_list args) {}
void AcpiOsRedirectOutput(void *dst) {}

////////////////////////////////////////////////////////////////////////////////
/// System ACPI Table Access
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsGetTableByAddress(ACPI_PHYSICAL_ADDRESS addr, ACPI_TABLE_HEADER **out) {}
ACPI_STATUS AcpiOsGetTableByIndex(UINT32 idx, ACPI_TABLE_HEADER **out, ACPI_PHYSICAL_ADDRESS **addr) {}
ACPI_STATUS AcpiOsGetTableByName(char *sig, UINT32 ins, ACPI_TABLE_HEADER **out, ACPI_PHYSICAL_ADDRESS **addr) {}

////////////////////////////////////////////////////////////////////////////////
/// Miscellaneous
////////////////////////////////////////////////////////////////////////////////

UINT64 AcpiOsGetTimer(void) {}  // return the current timer in 100-nanosecond units
ACPI_STATUS AcpiOsSignal(UINT32 func, void *info) {}
ACPI_STATUS AcpiOsGetLine(char *buf, UINT32 len, UINT32 *read) {}

