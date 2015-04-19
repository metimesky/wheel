#include <types.h>
#include <process.h>
#include <libk.h>

pcb_t ps[16];
pcb_t *process_to_go = NULL;
int current_pid;

extern goto_ring3();

int i = 240;

void sample_process_a() {
    static char *video = (char*)0xb8000;
    char ca = '0';
    //int i = 240;
    while (1) {
        video[2*i] = 'A';
        video[2*i+1] = 0x1f;
        video[2*i+2] = ca;
        video[2*i+3] = 0x1f;
        ++ca;
        i += 2;
        for (int a = 0; a < 100; ++a) {
            for (int b = 0; b < 200; ++b) {
                for (int c = 0; c < 300; ++c) {}
            }
        }
    }
}

void sample_process_b() {
    static char *video = (char*)0xb8000;
    char cb = '0';
    //int i = 240;
    while (1) {
        video[2*i] = 'B';
        video[2*i+1] = 0x0e;
        video[2*i+2] = cb;
        video[2*i+3] = 0x0e;
        ++cb;
        i += 2;
        for (int a = 0; a < 100; ++a) {
            for (int b = 0; b < 200; ++b) {
                for (int c = 0; c < 300; ++c) {}
            }
        }
    }
}

char stack_a[4096];
char stack_b[4096];

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

    ps[0].pid = 0;
    ps[0].context.cs = 0x18|3;
    ps[0].context.ds = 0x20|3;
    ps[0].context.es = 0x20|3;
    ps[0].context.fs = 0x20|3;
    ps[0].context.ss = 0x20|3;
    ps[0].context.gs = 0x20|3;
    ps[0].context.eip = (uint32_t)sample_process_a;
    ps[0].context.esp = (uint32_t)stack_a+4096;
    ps[0].context.eflags = 0x1202;
    //
    ps[1].pid = 1;
    ps[1].context.cs = 0x18|3;
    ps[1].context.ds = 0x20|3;
    ps[1].context.es = 0x20|3;
    ps[1].context.fs = 0x20|3;
    ps[1].context.ss = 0x20|3;
    ps[1].context.gs = 0x20|3;
    ps[1].context.eip = (uint32_t)sample_process_b;
    ps[1].context.esp = (uint32_t)stack_b+4096;
    ps[1].context.eflags = 0x1202;


    process_to_go = &ps[1];
    current_pid = 1;

    goto_ring3();
    while (1) {}
}

void schedule() {
    if (current_pid == 0) {
        current_pid = 1;
    } else {
        current_pid = 0;
    }
    process_to_go = &ps[current_pid];
}
