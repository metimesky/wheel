# 多处理器

Wheel 支持 SMP 架构的多处理器，这种架构下，所有的 CPU 都是同构的，共享所有内存。

然而，Intel Core i7 既实现了 SMP，也实现了 NUMA 和超线程。

## 获取 MP 信息

PC 启动的时候，默认是单处理器模式，OS 需要检测并启用多核。检测的方法通常是 ACPI 表，尽管英特尔的文档中描述的是 MP Tables，但是多数现有 OS 都不使用。

从 ACPI 信息中找到 MADT，里面定义有本地 APIC。在多处理器规范中，APIC 是很重要的部分。

首先需要做的是初始化 BSP 的本地 APIC。Spurious-Interrupt Vector Register 中包含一个位，表示本地 APIC 是否启用。Spurious-Interrupt 是一类假的中断，OS 完全可以将其映射到一个没有任何功能的 ISR 上。

接下来需要向其他 CPU 发送 SIPI（Startup IPI）。AP 启动后从实模式开始运行，执行初始化程序，切换模式，当所有处理器都已经初始化完毕后，BSP 将 IO APIC 设置成对称 IO 模式，这样所有的 AP 也能够接受中断。

## 启动 AP

启动其他的处理器核心需要用到处理期间中断，除此之外，还需要等待固定的时间。因此，有必要首先实现一定的计时功能，例如使用传统的 PIT 或者新型的 HPET。或者比较方便的使用 TimeStampCounter 加忙等待。

TimeStampCounter 可以通过汇编指令 rdtsc 实现。计数器存在于 MSR 寄存器中，增长的时间间隔和 CPU 主频绑定。

一个比 PIT 更加原始的方案是RealTimeClock，这是存储在 CMOS 中的一片电路，即使系统断电也能继续工作。

定时功能配合处理器间中断，可以唤醒其他的处理器。其中一个中断是 SIPI，这种中断的向量号表示页编号，即乘以 4K 后得到的地址就是新处理器开始执行的地方。OS 应该首先将一段汇编的程序复制到 1M 以下的内存中，让 AP 从那个地方开始执行，开启 A20，设置 GDT，进入保护模式，接下来的工作就可以重用 GRUB 的入口了。不过应该适当处理 BSP 和 AP 的区别，在启动代码中检查当前是否为 BSP，如果不是 BSP，则没有必要初始化内存管理器等关键资源。

每个 CPU 核心的运行都需要自己特有的栈，这需要 OS 提前分配好。

有一种 IPI 类型是多播，即向所有的核心发送信号。但是，OS 不应使用这种方式，因为用户可能通过 BIOS 禁用超线程技术，或者禁用了某些核心。启动 AP 的伪代码：

``` c
for(each AP mentioned by BIOS) {
    AP_status = NOT_STARTED;
    send_INIT_to_AP();
    wait(10ms);
    send_SIPI_to_AP();
    timeout_remaining = 5ms;
    while( timeout_remaining > 0) {
        if(AP_status == STARTED) goto started;
    }
    send_SIPI_to_AP();
    timeout_remaining = 10ms;
    while( timeout_remaining > 0) {
        if(AP_status == STARTED) goto started;
    }
    printf("AP CPU failed to start\n");
    continue;

started:
    AP_status = ACKNOWLEDGED;
}
```

AP 上执行的代码应该是：

``` c
AP_init () {
    AP_status = STARTED;
    while(AP_status != ACKNOWLEDGED) { /* Do nothing */ }

    /* Start CPU initialisation here */
}
```

## 时钟

下面要做的就是设置时钟。传统 OS 使用 PIT，但是 PIT 已经过时，Wheel 使用本地 APIC 计时器。这种计时器是集成在本地 APIC 中的，因此是每个 CPU 核心特有的。但是这种计时器的时钟周期并不可知，而是和 CPU 主频相关的，在某些情况下，CPU 的主频也是会变的。因此，通常的做法是使用一个外部时钟同步本地 APIC 计时器。

编程 APIC 计时器的方法和 APIC 一样，都是通过内存映射设备实现。APIC 的寄存器就是内存单元，均为 32 位，但是分布在 16 字节对齐的位置上。

- Timer LVT register: `* (uint32_t *) (local_apic_base + 0x320)`
- Divide Configuration register: `* (uint32_t *) (local_apic_base + 0x3e0)`
- Initial Count register: `* (uint32_t *) (local_apic_base + 0x380)`
- Current Count register: `* (uint32_t *) (local_apic_base + 0x390)`

启动 APIC 计时器的步骤是，设置 LVT 寄存器，其中包含向量号、模式（one-shot/periodic），设置除数，设置初始计数器，清除 LVT 寄存器中的标志为。

## HPET

HPET 是 ACPI 标准中的内容，专门有一张表描述 HPET 的信息（与 MADT 类似）。但是 Bochs 不支持 HPET，不过在 QEMU 中没有问题。

一个 HPET 有多个独立的计时器通道（3~32个），而且一个主板上可以有很多个 HPET，因此一个系统的计时器通道数量是无限的，但是通常是三个。

所谓通道，可以这样理解。系统中只有一个计数器，该计数器每隔固定的时间就会增加 1。通道则是一个比较器，当发现计数器的值和通道设置的值相等，就会触发一次中断。

HPET 的计数器增长频率是可调的，单位是飞秒（即 10^{-15} 秒）
