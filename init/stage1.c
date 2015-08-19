#include <stdint.h>

// low level printing function
static void write(const char *str, char attr) {
    static char* const video = (char*) 0xb8000;
    static int cursor = 0;
    for (int i = 0; str[i]; ++i) {
        if (str[i] == '\n') {
            cursor += 79;
            cursor -= cursor % 80;
        } else {
            video[2 * cursor] = str[i];
            video[2 * cursor + 1] = attr;
            ++cursor;
        }
    }
}

#define SEG_TYPE(x)  ((x) << 0x04)          // descriptor type
#define SEG_PRES(x)  ((x) << 0x07)          // present
#define SEG_SAVL(x)  ((x) << 0x0c)          // available for system use
#define SEG_LONG(x)  ((x) << 0x0d)          // long mode
#define SEG_SIZE(x)  ((x) << 0x0e)          // size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)  ((x) << 0x0f)          // granularity
#define SEG_PRIV(x) (((x) &  0x03) << 0x05) // privilege level (0 - 3)

#define CODE_PL0 \
    SEG_TYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
    SEG_LONG(1) | SEG_SIZE(0) | SEG_GRAN(1) | \
    SEG_PRIV(0) | 0x0a  // execute/read code segment, non-confirming

#define DATA_PL0 \
    SEG_TYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
    SEG_LONG(1) | SEG_SIZE(0) | SEG_GRAN(1) | \
    SEG_PRIV(0) | 0x02  // read/write data segment

#define CODE_PL3 \
    SEG_TYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
    SEG_LONG(1) | SEG_SIZE(0) | SEG_GRAN(1) | \
    SEG_PRIV(3) | 0x0a  // execute/read code segment, non-confirming

#define DATA_PL3 \
    SEG_TYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
    SEG_LONG(1) | SEG_SIZE(0) | SEG_GRAN(1) | \
    SEG_PRIV(3) | 0x02  // read/write data segment

static uint64_t gdt[7];

// defined in lib/load_gdtr.asm
extern void load_gdtr(uint64_t);

void init_gdt() {
    gdt[0] = 0UL;
    gdt[1] = 0UL | CODE_PL0;    // kernel code segment
    gdt[2] = 0UL | DATA_PL0;    // kernel data segment
    gdt[3] = 0UL | CODE_PL3;    // user code segment
    gdt[4] = 0UL | DATA_PL3;    // user data segment

    struct {
        uint16_t size;
        uint64_t base;
    } __attribute__((packed)) gdtr;
    gdtr.base = (uint64_t) gdt;
    gdtr.size = 5 * sizeof(uint64_t) - 1;
    __asm__ ("lgdt (%%rax)" :: "a"(&gdtr));
    // load_gdtr((uint64_t) &gdtr);
}

void init_idt() {
    ;
}

void stage1(uint32_t eax, uint32_t ebx) {
    write("[0] Initializing new GDT ... ", 0x0f);
    init_gdt();
    write("done\n", 0x0a);

    write("[1] Initializing IDT ... ", 0x0f);
    init_idt();
    write("done\n", 0x0a);
}
