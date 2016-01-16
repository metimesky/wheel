APIC 并不是独立的，它和多核架构、ACPI、时钟等设备紧密相关。

## 获取 APIC 相关信息

Wheel 从 ACPI 获取 APIC 的信息，不使用 MP Tables。ACPI 中的 MADT 表就是我们所需的信息，里面有所有的 local APIC、IO APIC 等信息。

对于 IO APIC 来说，初始化步骤如下：

- 首先从 MADT 中找出 IO APIC 的总数，每个 IO APIC 的基地址，每个 IO APIC 有多少个输入端口。
- 之后，在 PIC 中屏蔽所有的中断，并且在 IO APIC 也屏蔽（起始默认状态就是已屏蔽的，但是再屏蔽一遍更可靠）。
- 如果机器没有 ACPI，那么可能存在 IMCR 这个寄存器，需要向其中写入一个特殊的值才能启用 APIC。如果 PC 有 ACPI，则没有这个必要。
- MADT 中还提供了 ISA 中断的相关信息，通过 ACPI AML，还能获取 PCI 中断的信息。在启动初期，只考虑 ISA，只需要开启相应的端口。
