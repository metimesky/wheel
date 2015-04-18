#include <types.h>
#include <process.h>

pcb_t ps[16];
pcb_t *process_to_go = NULL;

extern goto_ring3();

void sample_process() {
    static char *video = (char*)0xb8000;
    int i = 240;
    while (1) {
        video[2*i] = 'U';
        video[2*i+1] = 0x1f;
        for (int a = 0; a < 1000; ++a) {
            for (int b = 0; b < 200; ++b) {
                for (int c = 0; c < 100; ++c) {}
            }
        }
        ++i;
    }
}

char stack[4096];

void main() {
    char *video = (char*)0xb8000;
    video[0] = 'K';
    video[1] = 0x1f;
    video[2] = 'e';
    video[3] = 0x1f;
    video[4] = 'r';
    video[5] = 0x1f;
    video[6] = 'n';
    video[7] = 0x1f;
    video[8] = 'e';
    video[9] = 0x1f;
    video[10] = 'l';
    video[11] = 0x1f;

    process_to_go = &ps[0];
    process_to_go->context.cs = 0x18|3;
    process_to_go->context.ds = 0x20|3;
    process_to_go->context.es = 0x20|3;
    process_to_go->context.fs = 0x20|3;
    process_to_go->context.ss = 0x20|3;
    process_to_go->context.gs = 0x20|3;
    process_to_go->context.eip = (uint32_t)sample_process;
    process_to_go->context.esp = (uint32_t)stack+4096;
    process_to_go->context.eflags = 0x1202;

    goto_ring3();
    while (1) {}
}

