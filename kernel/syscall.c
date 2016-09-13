#include <wheel.h>
#include <interrupt.h>

#include <drivers/console.h>

typedef void (*syscall_t)(int num, int_context_t *ctx);

syscall_t syscall_list[128];

// 底层的syscall分配器
static void syscall_dispatcher(int vec, int_context_t *ctx) {
    if (ctx->rax < 128) {
        syscall_list[ctx->rax](ctx->rax, ctx);
    }
}

// syscall的默认实现
static void default_syscall_handler(int num, int_context_t *ctx) {
    console_print("unsupported syscall #%d.\n", num);
}

static void syscall_print(int num, int_context_t *ctx) {
    console_print(ctx->rdi);
}

void syscall_init() {
    for (int i = 0; i < 128; ++i) {
        syscall_list[i] = default_syscall_handler;
    }

    syscall_list[0] = syscall_print;

    idt_set_int_handler(80, syscall_dispatcher);
}