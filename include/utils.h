#ifndef __UTILS_H__
#define __UTILS_H__ 1

static inline void panic(char *msg) {
    static char *video = (char*) 0xb8000;
    static char attr = 0x4e;
    for (int i = 0; msg[i]; ++i) {
        video[2*i] = msg[i];
        video[2*i+1] = attr;
    }
    while (1) {}
}

#define str(s) #s
#define mstr(s) str(s)

#define assert(cond) if (!(cond)) panic("ASSERT FAIL: " __FILE__ "(" mstr(__LINE__) "): \"" #cond "\"")

extern void load_idtr(uint64_t addr);
extern void load_tr(uint16_t sel);

extern uint8_t in_byte(uint16_t port);
extern uint16_t in_word(uint16_t port);
extern uint32_t in_dword(uint16_t port);
extern void out_byte(uint16_t port, uint8_t data);
extern void out_word(uint16_t port, uint8_t data);
extern void out_dword(uint16_t port, uint8_t data);

extern void cpu_id(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

#endif // __UTILS_H__
