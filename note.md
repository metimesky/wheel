## Compiling Bochs

```
./configure --enable-smp --enable-cpu-level=6 --enable-all-optimizations --enable-x86-64 --enable-pci --enable-vmx --enable-debugger --enable-disasm --enable-debugger-gui --enable-logging --enable-fpu --enable-3dnow --enable-cdrom --enable-x86-debugger --enable-iodebug --disable-plugins --disable-docbook --with-x --with-x11 --with-term
```

## Per-CPU Data

per-CPU Data Section中只能保存指针类型的数据，此外在per-CPU Section中还定义了内核栈，因此访问各自内核的内核栈，只能单独用__percpu_offset和自己的ID进行计算
