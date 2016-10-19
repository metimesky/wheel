#include <interrupt.h>
#include "entries.h"
#include <drivers/console.h>

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

// IDT是所有CPU共用的
int_handler_t int_handler_table[INTERRUPT_NUM];
static idt_entry_t idt[INTERRUPT_NUM];
static idt_ptr_t idtr;

// 默认的中断处理函数
static void default_int_handler(int vec, int_context_t *ctx) {
    static const char* sym[] = {    // 异常助记符
        "DE", "DB", "NMI", "BP", "OF", "BR", "UD", "NM",
        "DF", "><", "TS", "NP", "SS", "GP", "PF", "><",
        "MF", "AC", "MC", "XF", "><", "><", "><", "><",
        "><", "><", "><", "><", "><", "><", "SX", "><"
    };

    if (vec < IRQ_VEC_BASE) {
        console_print("Exception #%s, cs:rip=%x:%x, ss:rsp=%x:%x, rflags=%x, err=%x\n",
            sym[vec], ctx->cs, ctx->rip, ctx->ss, ctx->rsp, ctx->rflags, ctx->err_code);
        if (vec == 14) {
            // 如果是分页错误，则读取CR2并显示
            uint64_t virt;
            __asm__ __volatile__("mov %%cr2, %0" : "=r"(virt));
            console_print("Page Fault virtual address %x\n", virt);
        }
    } else {
        console_print("Interrupt #%d, cs:rip=%x:%x, ss:rsp=%x:%x, rflags=%x\n",
            vec, ctx->cs, ctx->rip, ctx->ss, ctx->rsp, ctx->rflags);
    }
    
    while (true) {}
}

// 填充IDT条目
static void fill_idt_entry(idt_entry_t *entry, void *handler, int dpl) __init {
    entry->selector = 8;
    entry->offset_low = ((uint64_t) handler) & 0xffff;
    entry->offset_mid = ((uint64_t) handler >> 16) & 0xffff;
    entry->offset_high = ((uint64_t) handler >> 32) & 0xffffffff;
    entry->attr = 0x8e00 | ((dpl & 3) << 13);   // Present, 64-bit Interrupt Gate, no IST
    entry->reserved = 0;
}

// fill IDT entries and load IDTR
void __init idt_init() {
    // initially, fill all interrupt handler as the default one
    for (int i = 0; i < INTERRUPT_NUM; ++i) {
        int_handler_table[i] = default_int_handler;
    }

    // internal exceptions, vector 0~31
    fill_idt_entry(&idt[0], isr0, 0);
    fill_idt_entry(&idt[1], isr1, 0);
    fill_idt_entry(&idt[2], isr2, 0);
    fill_idt_entry(&idt[3], isr3, 0);
    fill_idt_entry(&idt[4], isr4, 0);
    fill_idt_entry(&idt[5], isr5, 0);
    fill_idt_entry(&idt[6], isr6, 0);
    fill_idt_entry(&idt[7], isr7, 0);
    fill_idt_entry(&idt[8], isr8, 0);
    fill_idt_entry(&idt[9], isr9, 0);
    fill_idt_entry(&idt[10], isr10, 0);
    fill_idt_entry(&idt[11], isr11, 0);
    fill_idt_entry(&idt[12], isr12, 0);
    fill_idt_entry(&idt[13], isr13, 0);
    fill_idt_entry(&idt[14], isr14, 0);
    fill_idt_entry(&idt[15], isr15, 0);
    fill_idt_entry(&idt[16], isr16, 0);
    fill_idt_entry(&idt[17], isr17, 0);
    fill_idt_entry(&idt[18], isr18, 0);
    fill_idt_entry(&idt[19], isr19, 0);
    fill_idt_entry(&idt[20], isr20, 0);
    fill_idt_entry(&idt[21], isr21, 0);
    fill_idt_entry(&idt[22], isr22, 0);
    fill_idt_entry(&idt[23], isr23, 0);
    fill_idt_entry(&idt[24], isr24, 0);
    fill_idt_entry(&idt[25], isr25, 0);
    fill_idt_entry(&idt[26], isr26, 0);
    fill_idt_entry(&idt[27], isr27, 0);
    fill_idt_entry(&idt[28], isr28, 0);
    fill_idt_entry(&idt[29], isr29, 0);
    fill_idt_entry(&idt[30], isr30, 0);
    fill_idt_entry(&idt[31], isr31, 0);

    // other interrupts, for later use
    fill_idt_entry(&idt[32], isr32, 0);
    fill_idt_entry(&idt[33], isr33, 0);
    fill_idt_entry(&idt[34], isr34, 0);
    fill_idt_entry(&idt[35], isr35, 0);
    fill_idt_entry(&idt[36], isr36, 0);
    fill_idt_entry(&idt[37], isr37, 0);
    fill_idt_entry(&idt[38], isr38, 0);
    fill_idt_entry(&idt[39], isr39, 0);
    fill_idt_entry(&idt[40], isr40, 0);
    fill_idt_entry(&idt[41], isr41, 0);
    fill_idt_entry(&idt[42], isr42, 0);
    fill_idt_entry(&idt[43], isr43, 0);
    fill_idt_entry(&idt[44], isr44, 0);
    fill_idt_entry(&idt[45], isr45, 0);
    fill_idt_entry(&idt[46], isr46, 0);
    fill_idt_entry(&idt[47], isr47, 0);
    fill_idt_entry(&idt[48], isr48, 0);
    fill_idt_entry(&idt[49], isr49, 0);
    fill_idt_entry(&idt[50], isr50, 0);
    fill_idt_entry(&idt[51], isr51, 0);
    fill_idt_entry(&idt[52], isr52, 0);
    fill_idt_entry(&idt[53], isr53, 0);
    fill_idt_entry(&idt[54], isr54, 0);
    fill_idt_entry(&idt[55], isr55, 0);
    fill_idt_entry(&idt[56], isr56, 0);
    fill_idt_entry(&idt[57], isr57, 0);
    fill_idt_entry(&idt[58], isr58, 0);
    fill_idt_entry(&idt[59], isr59, 0);
    fill_idt_entry(&idt[60], isr60, 0);
    fill_idt_entry(&idt[61], isr61, 0);
    fill_idt_entry(&idt[62], isr62, 0);
    fill_idt_entry(&idt[63], isr63, 0);
    fill_idt_entry(&idt[64], isr64, 0);
    fill_idt_entry(&idt[65], isr65, 0);
    fill_idt_entry(&idt[66], isr66, 0);
    fill_idt_entry(&idt[67], isr67, 0);
    fill_idt_entry(&idt[68], isr68, 0);
    fill_idt_entry(&idt[69], isr69, 0);
    fill_idt_entry(&idt[70], isr70, 0);
    fill_idt_entry(&idt[71], isr71, 0);
    fill_idt_entry(&idt[72], isr72, 0);
    fill_idt_entry(&idt[73], isr73, 0);
    fill_idt_entry(&idt[74], isr74, 0);
    fill_idt_entry(&idt[75], isr75, 0);
    fill_idt_entry(&idt[76], isr76, 0);
    fill_idt_entry(&idt[77], isr77, 0);
    fill_idt_entry(&idt[78], isr78, 0);
    fill_idt_entry(&idt[79], isr79, 0);
    fill_idt_entry(&idt[80], isr80, 3);
    fill_idt_entry(&idt[81], isr81, 0);
    fill_idt_entry(&idt[82], isr82, 0);
    fill_idt_entry(&idt[83], isr83, 0);
    fill_idt_entry(&idt[84], isr84, 0);
    fill_idt_entry(&idt[85], isr85, 0);
    fill_idt_entry(&idt[86], isr86, 0);
    fill_idt_entry(&idt[87], isr87, 0);
    fill_idt_entry(&idt[88], isr88, 0);
    fill_idt_entry(&idt[89], isr89, 0);
    fill_idt_entry(&idt[90], isr90, 0);
    fill_idt_entry(&idt[91], isr91, 0);
    fill_idt_entry(&idt[92], isr92, 0);
    fill_idt_entry(&idt[93], isr93, 0);
    fill_idt_entry(&idt[94], isr94, 0);
    fill_idt_entry(&idt[95], isr95, 0);
    fill_idt_entry(&idt[96], isr96, 0);
    fill_idt_entry(&idt[97], isr97, 0);
    fill_idt_entry(&idt[98], isr98, 0);
    fill_idt_entry(&idt[99], isr99, 0);
    fill_idt_entry(&idt[100], isr100, 0);
    fill_idt_entry(&idt[101], isr101, 0);
    fill_idt_entry(&idt[102], isr102, 0);
    fill_idt_entry(&idt[103], isr103, 0);
    fill_idt_entry(&idt[104], isr104, 0);
    fill_idt_entry(&idt[105], isr105, 0);
    fill_idt_entry(&idt[106], isr106, 0);
    fill_idt_entry(&idt[107], isr107, 0);
    fill_idt_entry(&idt[108], isr108, 0);
    fill_idt_entry(&idt[109], isr109, 0);
    fill_idt_entry(&idt[110], isr110, 0);
    fill_idt_entry(&idt[111], isr111, 0);
    fill_idt_entry(&idt[112], isr112, 0);
    fill_idt_entry(&idt[113], isr113, 0);
    fill_idt_entry(&idt[114], isr114, 0);
    fill_idt_entry(&idt[115], isr115, 0);
    fill_idt_entry(&idt[116], isr116, 0);
    fill_idt_entry(&idt[117], isr117, 0);
    fill_idt_entry(&idt[118], isr118, 0);
    fill_idt_entry(&idt[119], isr119, 0);
    fill_idt_entry(&idt[120], isr120, 0);
    fill_idt_entry(&idt[121], isr121, 0);
    fill_idt_entry(&idt[122], isr122, 0);
    fill_idt_entry(&idt[123], isr123, 0);
    fill_idt_entry(&idt[124], isr124, 0);
    fill_idt_entry(&idt[125], isr125, 0);
    fill_idt_entry(&idt[126], isr126, 0);
    fill_idt_entry(&idt[127], isr127, 0);

    fill_idt_entry(&idt[128], isr128, 0);
    fill_idt_entry(&idt[129], isr129, 0);
    fill_idt_entry(&idt[130], isr130, 0);
    fill_idt_entry(&idt[131], isr131, 0);
    fill_idt_entry(&idt[132], isr132, 0);
    fill_idt_entry(&idt[133], isr133, 0);
    fill_idt_entry(&idt[134], isr134, 0);
    fill_idt_entry(&idt[135], isr135, 0);
    fill_idt_entry(&idt[136], isr136, 0);
    fill_idt_entry(&idt[137], isr137, 0);
    fill_idt_entry(&idt[138], isr138, 0);
    fill_idt_entry(&idt[139], isr139, 0);
    fill_idt_entry(&idt[140], isr140, 0);
    fill_idt_entry(&idt[141], isr141, 0);
    fill_idt_entry(&idt[142], isr142, 0);
    fill_idt_entry(&idt[143], isr143, 0);
    fill_idt_entry(&idt[144], isr144, 0);
    fill_idt_entry(&idt[145], isr145, 0);
    fill_idt_entry(&idt[146], isr146, 0);
    fill_idt_entry(&idt[147], isr147, 0);
    fill_idt_entry(&idt[148], isr148, 0);
    fill_idt_entry(&idt[149], isr149, 0);
    fill_idt_entry(&idt[150], isr150, 0);
    fill_idt_entry(&idt[151], isr151, 0);
    fill_idt_entry(&idt[152], isr152, 0);
    fill_idt_entry(&idt[153], isr153, 0);
    fill_idt_entry(&idt[154], isr154, 0);
    fill_idt_entry(&idt[155], isr155, 0);
    fill_idt_entry(&idt[156], isr156, 0);
    fill_idt_entry(&idt[157], isr157, 0);
    fill_idt_entry(&idt[158], isr158, 0);
    fill_idt_entry(&idt[159], isr159, 0);
    fill_idt_entry(&idt[160], isr160, 0);
    fill_idt_entry(&idt[161], isr161, 0);
    fill_idt_entry(&idt[162], isr162, 0);
    fill_idt_entry(&idt[163], isr163, 0);
    fill_idt_entry(&idt[164], isr164, 0);
    fill_idt_entry(&idt[165], isr165, 0);
    fill_idt_entry(&idt[166], isr166, 0);
    fill_idt_entry(&idt[167], isr167, 0);
    fill_idt_entry(&idt[168], isr168, 0);
    fill_idt_entry(&idt[169], isr169, 0);
    fill_idt_entry(&idt[170], isr170, 0);
    fill_idt_entry(&idt[171], isr171, 0);
    fill_idt_entry(&idt[172], isr172, 0);
    fill_idt_entry(&idt[173], isr173, 0);
    fill_idt_entry(&idt[174], isr174, 0);
    fill_idt_entry(&idt[175], isr175, 0);
    fill_idt_entry(&idt[176], isr176, 0);
    fill_idt_entry(&idt[177], isr177, 0);
    fill_idt_entry(&idt[178], isr178, 0);
    fill_idt_entry(&idt[179], isr179, 0);
    fill_idt_entry(&idt[180], isr180, 0);
    fill_idt_entry(&idt[181], isr181, 0);
    fill_idt_entry(&idt[182], isr182, 0);
    fill_idt_entry(&idt[183], isr183, 0);
    fill_idt_entry(&idt[184], isr184, 0);
    fill_idt_entry(&idt[185], isr185, 0);
    fill_idt_entry(&idt[186], isr186, 0);
    fill_idt_entry(&idt[187], isr187, 0);
    fill_idt_entry(&idt[188], isr188, 0);
    fill_idt_entry(&idt[189], isr189, 0);
    fill_idt_entry(&idt[190], isr190, 0);
    fill_idt_entry(&idt[191], isr191, 0);
    fill_idt_entry(&idt[192], isr192, 0);
    fill_idt_entry(&idt[193], isr193, 0);
    fill_idt_entry(&idt[194], isr194, 0);
    fill_idt_entry(&idt[195], isr195, 0);
    fill_idt_entry(&idt[196], isr196, 0);
    fill_idt_entry(&idt[197], isr197, 0);
    fill_idt_entry(&idt[198], isr198, 0);
    fill_idt_entry(&idt[199], isr199, 0);
    fill_idt_entry(&idt[200], isr200, 0);
    fill_idt_entry(&idt[201], isr201, 0);
    fill_idt_entry(&idt[202], isr202, 0);
    fill_idt_entry(&idt[203], isr203, 0);
    fill_idt_entry(&idt[204], isr204, 0);
    fill_idt_entry(&idt[205], isr205, 0);
    fill_idt_entry(&idt[206], isr206, 0);
    fill_idt_entry(&idt[207], isr207, 0);
    fill_idt_entry(&idt[208], isr208, 0);
    fill_idt_entry(&idt[209], isr209, 0);
    fill_idt_entry(&idt[210], isr210, 0);
    fill_idt_entry(&idt[211], isr211, 0);
    fill_idt_entry(&idt[212], isr212, 0);
    fill_idt_entry(&idt[213], isr213, 0);
    fill_idt_entry(&idt[214], isr214, 0);
    fill_idt_entry(&idt[215], isr215, 0);
    fill_idt_entry(&idt[216], isr216, 0);
    fill_idt_entry(&idt[217], isr217, 0);
    fill_idt_entry(&idt[218], isr218, 0);
    fill_idt_entry(&idt[219], isr219, 0);
    fill_idt_entry(&idt[220], isr220, 0);
    fill_idt_entry(&idt[221], isr221, 0);
    fill_idt_entry(&idt[222], isr222, 0);
    fill_idt_entry(&idt[223], isr223, 0);
    fill_idt_entry(&idt[224], isr224, 0);
    fill_idt_entry(&idt[225], isr225, 0);
    fill_idt_entry(&idt[226], isr226, 0);
    fill_idt_entry(&idt[227], isr227, 0);
    fill_idt_entry(&idt[228], isr228, 0);
    fill_idt_entry(&idt[229], isr229, 0);
    fill_idt_entry(&idt[230], isr230, 0);
    fill_idt_entry(&idt[231], isr231, 0);
    fill_idt_entry(&idt[232], isr232, 0);
    fill_idt_entry(&idt[233], isr233, 0);
    fill_idt_entry(&idt[234], isr234, 0);
    fill_idt_entry(&idt[235], isr235, 0);
    fill_idt_entry(&idt[236], isr236, 0);
    fill_idt_entry(&idt[237], isr237, 0);
    fill_idt_entry(&idt[238], isr238, 0);
    fill_idt_entry(&idt[239], isr239, 0);
    fill_idt_entry(&idt[240], isr240, 0);
    fill_idt_entry(&idt[241], isr241, 0);
    fill_idt_entry(&idt[242], isr242, 0);
    fill_idt_entry(&idt[243], isr243, 0);
    fill_idt_entry(&idt[244], isr244, 0);
    fill_idt_entry(&idt[245], isr245, 0);
    fill_idt_entry(&idt[246], isr246, 0);
    fill_idt_entry(&idt[247], isr247, 0);
    fill_idt_entry(&idt[248], isr248, 0);
    fill_idt_entry(&idt[249], isr249, 0);
    fill_idt_entry(&idt[250], isr250, 0);
    fill_idt_entry(&idt[251], isr251, 0);
    fill_idt_entry(&idt[252], isr252, 0);
    fill_idt_entry(&idt[253], isr253, 0);
    fill_idt_entry(&idt[254], isr254, 0);
    fill_idt_entry(&idt[255], isr255, 0);

    idt_load();
}

// 在AP中执行，用于加载已经初始化完成的IDT
void __init idt_load() {
    idtr.base = (uint64_t) idt;
    idtr.limit = INTERRUPT_NUM * sizeof(idt_entry_t) - 1;
    __asm__ __volatile__("lidt (%0)" :: "a"(&idtr));
}

// 获取中断处理函数指针
int_handler_t idt_get_int_handler(int vec) {
    if (0 <= vec && vec < INTERRUPT_NUM) {
        return int_handler_table[vec];
    } else {
        return 0;
    }
}

// 设置中断处理函数指针
void idt_set_int_handler(int vec, int_handler_t cb) {
    if (0 <= vec && vec < INTERRUPT_NUM) {
        if (cb == NULL) {
            int_handler_table[vec] = default_int_handler;
        } else {
            int_handler_table[vec] = cb;
        }
    }
}