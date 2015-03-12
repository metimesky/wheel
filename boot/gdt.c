#include <types.h>

// GDT pointer type definition
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

// global GDT pointer
gdt_ptr_t gdt_ptr;

// actual GDT structure
uint64_t gdt[5];

#define SEG_USER(x)     ((x) << 0x04)   // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)     ((x) << 0x07)   // Present
#define SEG_SAVL(x)     ((x) << 0x0C)   // Available for system use
#define SEG_LONG(x)     ((x) << 0x0D)   // Long mode
#define SEG_SIZE(x)     ((x) << 0x0E)   // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)     ((x) << 0x0F)   // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)    (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
 
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
#define SEG_CODE_EXRD       0x0A    // Execute/Read
#define SEG_CODE_EXRDA      0x0B    // Execute/Read, accessed
#define SEG_CODE_EXC        0x0C    // Execute-Only, conforming
#define SEG_CODE_EXCA       0x0D    // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC      0x0E    // Execute/Read, conforming
#define SEG_CODE_EXRDCA     0x0F    // Execute/Read, conforming, accessed
 
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

// only low 20 bit of limit is valid
static uint64_t create_gdt_descriptor(uint32_t base, uint32_t limit, uint16_t flag) {
    uint64_t descriptor;
 
    // create the high 32 bit segment
    descriptor  =  limit       & 0x000f0000;    // set limit bits 19:16
    descriptor |= (flag <<  8) & 0x00f0ff00;    // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000ff;    // set base bits 23:16
    descriptor |=  base        & 0xff000000;    // set base bits 31:24
 
    // shift by 32 to allow for low part of segment
    descriptor <<= 32;
 
    // create the low 32 bit segment
    descriptor |= base  << 16;                  // set base bits 15:0
    descriptor |= limit  & 0x0000ffff;          // set limit bits 15:0

    return descriptor;
}

void setup_gdt() {
    gdt[0] = create_gdt_descriptor(0, 0, 0);                    // null descriptor
    gdt[1] = create_gdt_descriptor(0, 0xfffff, GDT_CODE_PL0);   // kernel code segment
    gdt[2] = create_gdt_descriptor(0, 0xfffff, GDT_DATA_PL0);   // kernel data segment
    gdt[3] = create_gdt_descriptor(0, 0xfffff, GDT_CODE_PL3);   // user code segment
    gdt[4] = create_gdt_descriptor(0, 0xfffff, GDT_DATA_PL3);   // user data segment

    gdt_ptr.base  = (uint32_t)gdt;
    gdt_ptr.limit = 40 - 1;
}
