#ifndef __ACPI_H__
#define __ACPI_H__ 1

/* This file deals with Root System Description Table */

#include <type.h>

/*******************************************************************************
 * Pointer and General Structures
 ******************************************************************************/

struct rsdp_1 {
    char signature[8];  // "RSD PTR "
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_addr;
} __attribute__((packed));
typedef struct rsdp_1 rsdp_1_t;

struct rsdp_2 {
    rsdp_1_t rsdp_1;
    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t reserved[3];
} __attribute__((packed));
typedef struct rsdp_2 rsdp_2_t;

// the header is shared in RSDT and XSDT
// the standard ACPI table header
struct sdt_header {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));
typedef struct sdt_header sdt_header_t;

struct rsdt {   // signature = "RSDT"
    sdt_header_t header;
    uint32_t sdt_entries[0];
} __attribute__((packed));
typedef struct rsdt rsdt_t;

struct xsdt {   // signature = "XSDT"
    sdt_header_t header;
    uint64_t sdt_entries[0];
} __attribute__((packed));
typedef struct xsdt xsdt_t;

/*******************************************************************************
 * MADT - Multiple APIC Description Table
 ******************************************************************************/

struct madt {   // signature = "APIC"
    sdt_header_t header;
    uint32_t local_controller_addr;
    uint32_t flags;
    uint8_t entries[0];
} __attribute__((packed));
typedef struct madt madt_t;

struct madt_local_apic_entry {
    uint8_t type;   // 0
    uint8_t length;
    uint8_t processor_id;
    uint8_t local_apic_id;
    uint32_t flags;
} __attribute__((packed));
typedef struct madt_local_apic_entry madt_local_apic_entry_t;

struct madt_io_apic_entry {
    uint8_t type;   // 1
    uint8_t length;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_addr;
    uint32_t global_system_interrupt_base;
} __attribute__((packed));
typedef struct madt_io_apic_entry madt_io_apic_entry_t;

struct madt_interrupt_src_override {
    uint8_t type;   // 2
    uint8_t length;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} __attribute__((packed));
typedef struct madt_interrupt_src_override madt_interrupt_src_override_t;

struct madt_nmi_source {
    uint8_t type;   // 3
    uint8_t length;
} __attribute__((packed));
typedef struct madt_nmi_source madt_nmi_source_t;

struct madt_local_apic_nmi {
    uint8_t type;   // 4
    uint8_t length;
} __attribute__((packed));
typedef struct madt_local_apic_nmi madt_local_apic_nmi_t;

struct madt_local_apic_addr_override {
    uint8_t type;   // 5
    uint8_t length;
} __attribute__((packed));
typedef struct madt_local_apic_addr_override madt_local_apic_addr_override_t;

struct madt_io_sapic {
    uint8_t type;   // 6
    uint8_t length;
} __attribute__((packed));
typedef struct madt_io_sapic madt_io_sapic_t;

struct madt_local_sapic {
    uint8_t type;   // 7
    uint8_t length;
} __attribute__((packed));
typedef struct madt_local_sapic madt_local_sapic_t;

struct madt_platform_interrupt_source {
    uint8_t type;   // 8
    uint8_t length;
} __attribute__((packed));
typedef struct madt_platform_interrupt_source madt_platform_interrupt_source_t;

/*******************************************************************************
 * FADT - Fixed ACPI Description Table
 ******************************************************************************/

// structure used by ACPI to descibe the position of registers
struct reg {
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
} __attribute__((packed));
typedef struct reg reg_t;

struct fadt {
    sdt_header_t header;

    uint32_t firmware_control;
    uint32_t Dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  Reserved;
 
    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;
 
    uint8_t  Reserved2;
    uint32_t Flags;
 
    // 12 byte structure; see below for details
    reg_t ResetReg;
 
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                X_FirmwareControl;
    uint64_t                X_Dsdt;
 
    reg_t X_PM1aEventBlock;
    reg_t X_PM1bEventBlock;
    reg_t X_PM1aControlBlock;
    reg_t X_PM1bControlBlock;
    reg_t X_PM2ControlBlock;
    reg_t X_PMTimerBlock;
    reg_t X_GPE0Block;
    reg_t X_GPE1Block;
} __attribute__((packed));

#endif // __ACPI_H__