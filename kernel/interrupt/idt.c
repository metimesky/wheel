#include "idt.h"
#include "interrupt.h"
#include "entries.h"

static idt_entry_t idt[INTERRUPT_NUM];
static idt_ptr_t idtr;

// fill IDT entry with given handler address
static void fill_idt_entry(idt_entry_t *entry, void *handler) {
    entry->selector = 8;
    entry->offset_low = ((uint64_t) handler) & 0xffff;
    entry->offset_mid = ((uint64_t) handler >> 16) & 0xffff;
    entry->offset_high = ((uint64_t) handler >> 32) & 0xffffffff;
    entry->attr = 0x8e00; // Present, 64-bit Interrupt Gate, no IST
    entry->reserved = 0;
}

// fill IDT entries and load IDTR
void idt_init() {
    // internal exceptions, vector 0~31
    fill_idt_entry(&idt[0], isr0);
    fill_idt_entry(&idt[1], isr1);
    fill_idt_entry(&idt[2], isr2);
    fill_idt_entry(&idt[3], isr3);
    fill_idt_entry(&idt[4], isr4);
    fill_idt_entry(&idt[5], isr5);
    fill_idt_entry(&idt[6], isr6);
    fill_idt_entry(&idt[7], isr7);
    fill_idt_entry(&idt[8], isr8);
    fill_idt_entry(&idt[9], isr9);
    fill_idt_entry(&idt[10], isr10);
    fill_idt_entry(&idt[11], isr11);
    fill_idt_entry(&idt[12], isr12);
    fill_idt_entry(&idt[13], isr13);
    fill_idt_entry(&idt[14], isr14);
    fill_idt_entry(&idt[15], isr15);
    fill_idt_entry(&idt[16], isr16);
    fill_idt_entry(&idt[17], isr17);
    fill_idt_entry(&idt[18], isr18);
    fill_idt_entry(&idt[19], isr19);
    fill_idt_entry(&idt[20], isr20);
    fill_idt_entry(&idt[21], isr21);
    fill_idt_entry(&idt[22], isr22);
    fill_idt_entry(&idt[23], isr23);
    fill_idt_entry(&idt[24], isr24);
    fill_idt_entry(&idt[25], isr25);
    fill_idt_entry(&idt[26], isr26);
    fill_idt_entry(&idt[27], isr27);
    fill_idt_entry(&idt[28], isr28);
    fill_idt_entry(&idt[29], isr29);
    fill_idt_entry(&idt[30], isr30);
    fill_idt_entry(&idt[31], isr31);

    // other interrupts, for later use
    fill_idt_entry(&idt[32], isr32);
    fill_idt_entry(&idt[33], isr33);
    fill_idt_entry(&idt[34], isr34);
    fill_idt_entry(&idt[35], isr35);
    fill_idt_entry(&idt[36], isr36);
    fill_idt_entry(&idt[37], isr37);
    fill_idt_entry(&idt[38], isr38);
    fill_idt_entry(&idt[39], isr39);
    fill_idt_entry(&idt[40], isr40);
    fill_idt_entry(&idt[41], isr41);
    fill_idt_entry(&idt[42], isr42);
    fill_idt_entry(&idt[43], isr43);
    fill_idt_entry(&idt[44], isr44);
    fill_idt_entry(&idt[45], isr45);
    fill_idt_entry(&idt[46], isr46);
    fill_idt_entry(&idt[47], isr47);
    fill_idt_entry(&idt[48], isr48);
    fill_idt_entry(&idt[49], isr49);
    fill_idt_entry(&idt[50], isr50);
    fill_idt_entry(&idt[51], isr51);
    fill_idt_entry(&idt[52], isr52);
    fill_idt_entry(&idt[53], isr53);
    fill_idt_entry(&idt[54], isr54);
    fill_idt_entry(&idt[55], isr55);
    fill_idt_entry(&idt[56], isr56);
    fill_idt_entry(&idt[57], isr57);
    fill_idt_entry(&idt[58], isr58);
    fill_idt_entry(&idt[59], isr59);
    fill_idt_entry(&idt[60], isr60);
    fill_idt_entry(&idt[61], isr61);
    fill_idt_entry(&idt[62], isr62);
    fill_idt_entry(&idt[63], isr63);
    fill_idt_entry(&idt[64], isr64);
    fill_idt_entry(&idt[65], isr65);
    fill_idt_entry(&idt[66], isr66);
    fill_idt_entry(&idt[67], isr67);
    fill_idt_entry(&idt[68], isr68);
    fill_idt_entry(&idt[69], isr69);
    fill_idt_entry(&idt[70], isr70);
    fill_idt_entry(&idt[71], isr71);
    fill_idt_entry(&idt[72], isr72);
    fill_idt_entry(&idt[73], isr73);
    fill_idt_entry(&idt[74], isr74);
    fill_idt_entry(&idt[75], isr75);
    fill_idt_entry(&idt[76], isr76);
    fill_idt_entry(&idt[77], isr77);
    fill_idt_entry(&idt[78], isr78);
    fill_idt_entry(&idt[79], isr79);
    fill_idt_entry(&idt[80], isr80);
    fill_idt_entry(&idt[81], isr81);
    fill_idt_entry(&idt[82], isr82);
    fill_idt_entry(&idt[83], isr83);
    fill_idt_entry(&idt[84], isr84);
    fill_idt_entry(&idt[85], isr85);
    fill_idt_entry(&idt[86], isr86);
    fill_idt_entry(&idt[87], isr87);
    fill_idt_entry(&idt[88], isr88);
    fill_idt_entry(&idt[89], isr89);
    fill_idt_entry(&idt[90], isr90);
    fill_idt_entry(&idt[91], isr91);
    fill_idt_entry(&idt[92], isr92);
    fill_idt_entry(&idt[93], isr93);
    fill_idt_entry(&idt[94], isr94);
    fill_idt_entry(&idt[95], isr95);
    fill_idt_entry(&idt[96], isr96);
    fill_idt_entry(&idt[97], isr97);
    fill_idt_entry(&idt[98], isr98);
    fill_idt_entry(&idt[99], isr99);
    fill_idt_entry(&idt[100], isr100);
    fill_idt_entry(&idt[101], isr101);
    fill_idt_entry(&idt[102], isr102);
    fill_idt_entry(&idt[103], isr103);
    fill_idt_entry(&idt[104], isr104);
    fill_idt_entry(&idt[105], isr105);
    fill_idt_entry(&idt[106], isr106);
    fill_idt_entry(&idt[107], isr107);
    fill_idt_entry(&idt[108], isr108);
    fill_idt_entry(&idt[109], isr109);
    fill_idt_entry(&idt[110], isr110);
    fill_idt_entry(&idt[111], isr111);
    fill_idt_entry(&idt[112], isr112);
    fill_idt_entry(&idt[113], isr113);
    fill_idt_entry(&idt[114], isr114);
    fill_idt_entry(&idt[115], isr115);
    fill_idt_entry(&idt[116], isr116);
    fill_idt_entry(&idt[117], isr117);
    fill_idt_entry(&idt[118], isr118);
    fill_idt_entry(&idt[119], isr119);
    fill_idt_entry(&idt[120], isr120);
    fill_idt_entry(&idt[121], isr121);
    fill_idt_entry(&idt[122], isr122);
    fill_idt_entry(&idt[123], isr123);
    fill_idt_entry(&idt[124], isr124);
    fill_idt_entry(&idt[125], isr125);
    fill_idt_entry(&idt[126], isr126);
    fill_idt_entry(&idt[127], isr127);

    idtr.base = (uint64_t) idt;
    idtr.limit = INTERRUPT_NUM * sizeof(idt_entry_t) - 1;
    __asm__ __volatile__("lidt (%0)" :: "a"(&idtr));
    // load_idtr(&idtr);   // defined in library/cpu.asm
}