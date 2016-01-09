#include <utilities/env.h>
#include <utilities/logging.h>

void unwind() {
    uint64_t rbp;

    // first get current rbp
    __asm__ __volatile__("movq %%rbp, %0" : "=r"(rbp));
    log("current rbp is %x", rbp);

    while (rbp != 0) {
        log("[rbp], [rbp+8]: 0x%x, 0x%x", DATA_U64(rbp), DATA_U64(rbp+8));
        rbp = DATA_U64(rbp);
    }
}

void unwind_from(uint64_t rbp) {
    while (0x100000 < rbp && rbp < 0x100000000) {
        log("unwinding: 0x%x", DATA_U64(rbp+8));
        rbp = DATA_U64(rbp);
    }
}