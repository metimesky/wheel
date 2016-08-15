#ifndef __ACPI_H__
#define __ACPI_H__

#include <utilities/kernel.h>

// signatures
#define ACPI_SIG_RSDP   "RSD PTR "  // root system description pointer
#define ACPI_SIG_RSDT   "RSDT"      // root system description table
#define ACPI_SIG_XSDT   "XSDT"      // extended system description table
#define ACPI_SIG_FADT   "FACP"      // fixed acpi description table
#define ACPI_SIG_SSDT   "SSDT"      // secondary system description table
#define ACPI_SIG_MADT   "APIC"      // multiple APIC description table
#define ACPI_SIG_HPET   "HPET"      // high precision event timer table

// RSDP -- Root System Description Pointer
struct acpi_rsdp {
    char        signature[8];           // "RSD PTR "
    uint8_t     checksum;               // ACPI 1.0 checksum
    char        oem_id[6];              // OEM identification
    uint8_t     revision;               // 0 for ACPI 1.0, 2 for ACPI 2.0+
    uint32_t    rsdt_physical_addr;     // 32-bit physical address of the RSDT
    uint32_t    length;                 // length in bytes, including header (ACPI 2.0+)
    uint64_t    xsdt_physical_addr;     // physical address of the XSDT
    uint8_t     extended_checksum;      // Checksum of entire table (ACPI 2.0+)
    uint8_t     reserved[3];            // Reserved, must be zero
} __attribute__((packed));
typedef struct acpi_rsdp acpi_rsdp_t;

// 通用表头
struct acpi_table_header {
    char        signature[4];       // ASCII table signature
    uint32_t    length;             // table total length in bytes
    uint8_t     revision;           // ACPI spec minor version number
    uint8_t     checksum;           // makeing sum of entire table == 0
    char        oem_id[6];          // ASCII OEM identification
    char        oem_table_id[8];    // ASCII OEM table identification
    uint32_t    oem_revision;       // OEM revision number
    char        creator_id[4];      // ASCII ASL compiler vendor ID
    uint32_t    creator_revision;   // ASL compiler version
} __attribute__((packed));
typedef struct acpi_table_header acpi_table_header_t;

// 通用子表表头
struct acpi_subtable_header {
    uint8_t type;
    uint8_t length;
} __attribute__((packed));
typedef struct acpi_subtable_header acpi_subtable_header_t;

// RSDT
struct acpi_table_rsdt {
    acpi_table_header_t header;     // signature == "RSDT"
    uint32_t            entries[0]; // physical addresses to other tables
} __attribute__((packed));
typedef struct acpi_table_rsdt acpi_table_rsdt_t;

// XSDT
struct acpi_table_xsdt {
    acpi_table_header_t header;     // signature == "XSDT"
    uint64_t            entries[0]; // physical addresses to other tables
} __attribute__((packed));
typedef struct acpi_table_xsdt acpi_table_xsdt_t;

////////////////////////////////////////////////////////////////////////////////
// MADT -- Multiple APIC Description Table
struct acpi_table_madt {
    acpi_table_header_t header;
    uint32_t            local_apic_addr;
    uint32_t            flags;
} __attribute__((packed));
typedef struct acpi_table_madt acpi_table_madt_t;

// MADT子表类型
#define ACPI_MADT_LOCAL_APIC            0
#define ACPI_MADT_IO_APIC               1
#define ACPI_MADT_INTERRUPT_OVERRIDE    2
#define ACPI_MADT_NMI_SOURCE            3
#define ACPI_MADT_LOCAL_APIC_NMI        4
#define ACPI_MADT_LOCAL_APIC_OVERRIDE   5
#define ACPI_MADT_IO_SAPIC              6
#define ACPI_MADT_LOCAL_SAPIC           7
#define ACPI_MADT_INTERRUPT_SOURCE      8
#define ACPI_MADT_LOCAL_X2APIC          9
#define ACPI_MADT_LOCAL_X2APIC_NMI      10
#define ACPI_MADT_GENERIC_INTERRUPT     11
#define ACPI_MADT_GENERIC_DISTRIBUTOR   12
#define ACPI_MADT_GENERIC_MSI_FRAME     13
#define ACPI_MADT_GENERIC_REDISTRIBUTOR 14
#define ACPI_MADT_GENERIC_TRANSLATOR    15
#define ACPI_MADT_RESERVED              16

// MADT local APIC structure
struct acpi_madt_local_apic {
    acpi_subtable_header_t  header;
    uint8_t                 processor_id;
    uint8_t                 local_apic_id;
    uint32_t                flags;
} __attribute__((packed));
typedef struct acpi_madt_local_apic acpi_madt_local_apic_t;

// MADT IO APIC structure
struct acpi_madt_io_apic {
    acpi_subtable_header_t  header;
    uint8_t                 io_apic_id;
    uint8_t                 reserved;
    uint32_t                io_apic_addr;
    uint32_t                global_irq_base;
} __attribute__((packed));
typedef struct acpi_madt_io_apic acpi_madt_io_apic_t;

// MADT interrupt override
struct acpi_madt_interrupt_override {
    acpi_subtable_header_t  header;
    uint8_t                 bus;        // 0 means ISA
    uint8_t                 source_irq; // interrupt source (IRQ)
    uint32_t                global_irq; // global system interrupt
    uint16_t                inti_flags;
} __attribute__((packed));
typedef struct acpi_madt_interrupt_override acpi_madt_interrupt_override_t;

// MADT NMI source
struct acpi_madt_nmi_source {
    acpi_subtable_header_t  header;
    uint16_t                inti_flags;
    uint32_t                global_irq; // global system interrupt
} __attribute__((packed));
typedef struct acpi_madt_nmi_source acpi_madt_nmi_source_t;

// Local APIC NMI
struct acpi_madt_local_apic_nmi {
    acpi_subtable_header_t  header;
    uint8_t                 processor_id;   // ACPI processor id
    uint16_t                inti_flags;
    uint8_t                 lint;           // LINTn to which NMI is connected
} __attribute__((packed));
typedef struct acpi_madt_local_apic_nmi acpi_madt_local_apic_nmi_t;

// address override
struct acpi_madt_local_apic_override {
    acpi_subtable_header_t  header;
    uint16_t                reserved;
    uint64_t                address;    // APIC physical address
} __attribute__((packed));
typedef struct acpi_madt_local_apic_override acpi_madt_local_apic_override_t;

extern acpi_table_header_t *acpi_get_fadt();
extern acpi_table_header_t *acpi_get_ssdt();
extern acpi_table_madt_t *acpi_get_madt();
extern acpi_table_header_t *acpi_get_fadt();
extern void acpi_init();

#endif
