#ifndef PROCESS_H_
#define PROCESS_H_ 1

#include "../i386/i386.h"

struct pcb {
    process_context_t context;
    uint32_t pid;
} __attribute__((packed));
typedef struct pcb pcb_t;

#endif // PROCESS_H_