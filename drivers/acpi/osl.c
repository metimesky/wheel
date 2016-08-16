// this file defines functions that are required by ACPICA
// ACPICA OS Layer

#include "acpi.h"    // we have to include this header first to avoid some wierd problems
#include <wheel.h>
// #include <memory/memory.h>
// #include <utilities/logging.h>

typedef int spinlock_t;

#define TRACE while(1){}

////////////////////////////////////////////////////////////////////////////////
/// Environmental and ACPI tables
////////////////////////////////////////////////////////////////////////////////

// called during initialization of the ACPICA subsystem
ACPI_STATUS AcpiOsInitialize() {
    //TRACE
    // do nothing, just return ok
	return AE_OK;
}

// called during termination of the ACPICA subsystem
ACPI_STATUS AcpiOsTerminate() {
    TRACE
    // do nothing, just return ok
	return AE_OK;
}

// returns the physical address of the ACPI RSDP table (only works on x86)
ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer() {
    ACPI_SIZE ret;
    if (ACPI_SUCCESS(AcpiFindRootPointer(&ret))) {
        return ret;
    } else {
        return 0;
    }
}

// allow the host os to override a predefined ACPI object
ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *PredefinedObject, ACPI_STRING *NewValue) {
    TRACE
    return 0;
}

// allow the host OS to override a firmware ACPI table via a logical address
ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *old_table, ACPI_TABLE_HEADER **new_table) {
    *new_table = NULL;  // no replacement is provided
    return AE_OK;
}

// allow the host OS to override a firmware ACPI table via a physical address
ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER *old_table, ACPI_PHYSICAL_ADDRESS *new_table, UINT32 *new_table_len) {
    *new_table = 0;     // no replacement will be given
    *new_table_len = 0;
    return AE_OK;
}

////////////////////////////////////////////////////////////////////////////////
/// memory
////////////////////////////////////////////////////////////////////////////////

// only needed when use Wheel's own cache
ACPI_STATUS AcpiOsCreateCache(char *cacheName, UINT16 size, UINT16 depth, ACPI_CACHE_T **ret) {
    TRACE
}
ACPI_STATUS AcpiOsDeleteCache(ACPI_CACHE_T *cache) {
    TRACE
}
ACPI_STATUS AcpiOsPurgeCache(ACPI_CACHE_T *cache) {
    TRACE
}
void *AcpiOsAcquireObject(ACPI_CACHE_T *cache) {
    TRACE
}
ACPI_STATUS AcpiOsReleaseObject(ACPI_CACHE_T *cache, void *object) {
    TRACE
}


// for memories below 4GB, we can simply return physical address

// map the physical address to virtual space and return the virtual address
void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS phy_addr, ACPI_SIZE len) {
    int page_num = (len + 4095) >> 12;  // calculate the number of pages, round upwards
    if (phy_addr > 0xffffffffUL) {
        // log("mapping %x", phy_addr);
    }
    return (void *) phy_addr;
}
// deletes a mapping that was created by AcpiOsMapMemory
void AcpiOsUnmapMemory(void *virt_addr, ACPI_SIZE len) {
    int page_num = (len + 4095) >> 12;  // calculate the number of pages, round upwards
    if (virt_addr > 0xffffffffUL) {
        // log("unmapping %x", virt_addr);
    }
}
ACPI_STATUS AcpiOsGetPhysicalAddress(void *LogicalAddress, ACPI_PHYSICAL_ADDRESS *PhysicalAddress) {
    TRACE
}

void *AcpiOsAllocate(ACPI_SIZE n) {
    // void *addr = slab_alloc(n);
    // log("osl::malloc %d at %x", n, addr);
    // return addr;
    return 0;
}
void AcpiOsFree(void *addr) {
    // log("osl::freeing at %x", addr);
    // slab_free(addr);
}

BOOLEAN AcpiOsReadable(void *addr, ACPI_SIZE n) {
    TRACE
}
BOOLEAN AcpiOsWritable(void *addr, ACPI_SIZE n) {
    TRACE
}

// #define ACPI_CACHE_T                ACPI_MEMORY_LIST
// #define ACPI_USE_LOCAL_CACHE        1

////////////////////////////////////////////////////////////////////////////////
/// Multithreading and Scheduling Services
////////////////////////////////////////////////////////////////////////////////

ACPI_THREAD_ID AcpiOsGetThreadId() {
    return 0;
}
ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE Type, ACPI_OSD_EXEC_CALLBACK Function, void *Context) {
    TRACE
}

void AcpiOsSleep(UINT64 Milliseconds) {
    TRACE
}
void AcpiOsStall(UINT32 Microseconds) {
    TRACE
}
void AcpiOsWaitEventsComplete(void) {
    TRACE
}

////////////////////////////////////////////////////////////////////////////////
/// Mutual Exclusion and Synchronization
////////////////////////////////////////////////////////////////////////////////

// ACPI_STATUS AcpiOsCreateMutex(ACPI_MUTEX *OutHandle) {
//     TRACE
// }
// void AcpiOsDeleteMutex(ACPI_MUTEX Handle) {
//     TRACE
// }
// ACPI_STATUS AcpiOsAcquireMutex(ACPI_MUTEX Handle, UINT16 Timeout) {
//     TRACE
// }
// void AcpiOsReleaseMutex(ACPI_MUTEX Handle) {
//     TRACE
// }

ACPI_STATUS AcpiOsCreateSemaphore(UINT32 MaxUnits, UINT32 InitialUnits, ACPI_SEMAPHORE *OutHandle) {
    TRACE
}
ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle) {
    TRACE
}
static uint32_t val;
ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units, UINT16 Timeout) {
    // log("semaphore wait %d, %d, %d", Handle, Units, Timeout);
    --val;
    return AE_OK;
}
ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units) {
    // log("semaphore signal %d, %d", Handle, Units);
    ++val;
    return AE_OK;
}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *OutHandle) {
    TRACE
}
void AcpiOsDeleteLock(ACPI_SPINLOCK Handle) {
    TRACE
}
ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle) {
    TRACE
}
void AcpiOsReleaseLock(ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags) {
    TRACE
}

////////////////////////////////////////////////////////////////////////////////
/// Interrupt Handling
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 InterruptLevel, ACPI_OSD_HANDLER Handler, void *Context) {
    TRACE
}
ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 InterruptNumber, ACPI_OSD_HANDLER Handler) {
    TRACE
}

////////////////////////////////////////////////////////////////////////////////
/// Memory Access and Memory Mapped I/O
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS addr, UINT64 *val, UINT32 width) {
    TRACE
}
ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS addr, UINT64 val, UINT32 width) {
    TRACE
}

////////////////////////////////////////////////////////////////////////////////
/// Port Input/Output
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS addr, UINT32 *val, UINT32 width) {
    TRACE
}
ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS addr, UINT32 val, UINT32 width) {
    TRACE
}

////////////////////////////////////////////////////////////////////////////////
/// PCI Configuration Space Access
////////////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID *pciId, UINT32 reg, UINT64 *val, UINT32 width) {
    TRACE
}
ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *pciId, UINT32 reg, UINT64 val, UINT32 width) {
    TRACE
}

////////////////////////////////////////////////////////////////////////////////
/// Formatted Output
////////////////////////////////////////////////////////////////////////////////

void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf(const char *Format, ...) {
    TRACE
}
void AcpiOsVprintf(const char *fmt, va_list args) {
    TRACE
}
void AcpiOsRedirectOutput(void *dst) {
    TRACE
}

////////////////////////////////////////////////////////////////////////////////
/// System ACPI Table Access
////////////////////////////////////////////////////////////////////////////////

// ACPI_STATUS AcpiOsGetTableByAddress(ACPI_PHYSICAL_ADDRESS addr, ACPI_TABLE_HEADER **out) {
//     TRACE
// }
// ACPI_STATUS AcpiOsGetTableByIndex(UINT32 idx, ACPI_TABLE_HEADER **out, ACPI_PHYSICAL_ADDRESS **addr) {
//     TRACE
// }
// ACPI_STATUS AcpiOsGetTableByName(char *sig, UINT32 ins, ACPI_TABLE_HEADER **out, ACPI_PHYSICAL_ADDRESS *addr) {
//     TRACE
// }
// ACPI_STATUS
// AcpiOsGetTableByName (
//     char                    *Signature,
//     UINT32                  Instance,
//     ACPI_TABLE_HEADER       **Table,
//     ACPI_PHYSICAL_ADDRESS   *Address) {
//     TRACE
// }

////////////////////////////////////////////////////////////////////////////////
/// Miscellaneous
////////////////////////////////////////////////////////////////////////////////

UINT64 AcpiOsGetTimer(void) {  // return the current timer in 100-nanosecond units
    TRACE
}
ACPI_STATUS AcpiOsSignal(UINT32 func, void *info) {
    TRACE
}
ACPI_STATUS AcpiOsGetLine(char *buf, UINT32 len, UINT32 *read) {
    TRACE
}

