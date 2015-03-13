#ifndef IA32_PROTECT_H_
#define IA32_PROTECT_H_ 1

#include <types.h>

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) idt_ptr_t;

#define SEG_USER(x)     ((x) << 0x04)   // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)     ((x) << 0x07)   // Present
#define SEG_SAVL(x)     ((x) << 0x0c)   // Available for system use
#define SEG_LONG(x)     ((x) << 0x0d)   // Long mode
#define SEG_SIZE(x)     ((x) << 0x0e)   // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)     ((x) << 0x0f)   // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)    (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)

// descriptor type for code/data segment
#define SEG_DATA_RD         0x00    // Read-Only
#define SEG_DATA_RDA        0x01    // Read-Only, accessed
#define SEG_DATA_RDWR       0x02    // Read/Write
#define SEG_DATA_RDWRA      0x03    // Read/Write, accessed
#define SEG_DATA_RDEXPD     0x04    // Read-Only, expand-down
#define SEG_DATA_RDEXPDA    0x05    // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD   0x06    // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA  0x07    // Read/Write, expand-down, accessed
#define SEG_CODE_EX         0x08    // Execute-Only
#define SEG_CODE_EXA        0x09    // Execute-Only, accessed
#define SEG_CODE_EXRD       0x0a    // Execute/Read
#define SEG_CODE_EXRDA      0x0b    // Execute/Read, accessed
#define SEG_CODE_EXC        0x0c    // Execute-Only, conforming
#define SEG_CODE_EXCA       0x0d    // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC      0x0e    // Execute/Read, conforming
#define SEG_CODE_EXRDCA     0x0f    // Execute/Read, conforming, accessed

// descriptor type for system segment
#define SEG_286_TSS         0x01    // 16-bit TSS (Available)
#define SEG_LDT             0x02    // LDT
#define SEG_286_TSS_BUSY    0x03    // 16-bit TSS (Busy)
#define SEG_286_CALL_GATE   0x04    // 16-bit Call Gate
#define SEG_TASK_GATE       0x05    // Task Gate
#define SEG_286_INT_GATE    0x06    // 16-bit Interrupt Gate
#define SEG_286_TRAP_GATE   0x07    // 16-bit Trap Gate
#define SEG_386_TSS         0x09    // 32-bit TSS (Available)
#define SEG_386_TSS_BUSY    0x0b    // 32-bit TSS (Busy)
#define SEG_386_CALL_GATE   0x0c    // 32-bit Call Gate
#define SEG_386_INT_GATE    0x0e    // 32-bit Interrupt Gate
#define SEG_386_TRAP_GATE   0x0f    // 32-bit Trap Gate

#define GDT_CODE_PL0    SEG_USER(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                        SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                        SEG_PRIV(0) | SEG_CODE_EXRD
#define GDT_DATA_PL0    SEG_USER(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                        SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                        SEG_PRIV(0) | SEG_DATA_RDWR
#define GDT_CODE_PL3    SEG_USER(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                        SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                        SEG_PRIV(3) | SEG_CODE_EXRD
#define GDT_DATA_PL3    SEG_USER(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                        SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                        SEG_PRIV(3) | SEG_DATA_RDWR
#define GDT_TSS         SEG_USER(0) | SEG_PRES(1) | SEG_SAVL(0) | \
                        SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(0) | \
                        SEG_PRIV(3) | SEG_386_TSS

#define IDT_386_TASK
#define IDT_386_INT
#define IDT_386_TRAP

// TSS structure
typedef struct {
    uint16_t prev_task;
    uint16_t resv_1;
    uint32_t esp0;
    uint16_t ss0;
    uint16_t resv_2;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t resv_3;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t resv_4;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t resv_5;
    uint16_t cs;
    uint16_t resv_6;
    uint16_t ss;
    uint16_t resv_7;
    uint16_t ds;
    uint16_t resv_8;
    uint16_t fs;
    uint16_t resv_9;
    uint16_t gs;
    uint16_t resv_a;
    uint16_t ldt;
    uint16_t resv_b;
    uint16_t T;
    uint16_t io_map;
} __attribute__((packed)) tss_t;

// typedef struct {
//     uint16_t prev_task;
//     uint16_t;
//     uint32_t esp0;
//     uint16_t ss0;
//     uint16_t;
//     uint32_t esp1;
//     uint16_t ss1;
//     uint16_t;
//     uint32_t esp2;
//     uint16_t ss2;
//     uint16_t;
//     uint32_t cr3;
//     uint32_t eip;
//     uint32_t eflags;
//     uint32_t eax;
//     uint32_t ecx;
//     uint32_t edx;
//     uint32_t ebx;
//     uint32_t esp;
//     uint32_t ebp;
//     uint32_t esi;
//     uint32_t edi;
//     uint16_t es;
//     uint16_t;
//     uint16_t cs;
//     uint16_t;
//     uint16_t ss;
//     uint16_t;
//     uint16_t ds;
//     uint16_t;
//     uint16_t fs;
//     uint16_t;
//     uint16_t gs;
//     uint16_t;
//     uint16_t ldt;
//     uint16_t;
//     uint16_t T;
//     uint16_t io_map;
// } __attribute__((packed)) tss_t;


// global data definition
#define GDT_SIZE 6
#define IDT_SIZE 256
extern gdt_ptr_t gdt_ptr;
extern idt_ptr_t idt_ptr;
extern uint64_t gdt[GDT_SIZE];
extern uint64_t idt[IDT_SIZE];
extern tss_t tss;

#endif // IA32_PROTECT_H_