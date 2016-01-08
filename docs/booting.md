# 引导

Wheel 的引导使用 GRUB 引导器，只要遵循 Multiboot 规范第一版就可以，GRUB Legacy 和 GRUB2 都满足这个要求。

使用引导器，能让 Wheel 直接从保护模式开始执行，而且能够直接获取机器重要信息。

其中一个重要信息就是内存布局（Memory Map），这个信息由 GRUB 直接给出。内存布局描述了哪部分内存可以使用，哪部分不能使用。Multiboot2 规范中，对于不能使用的内存有了更细的分类，区分了可回收的内存，例如被 ACPI 用来存放表的内存，就可以被 OS 回收再利用。

但是，Wheel 不去回收这些内存。因为 Wheel 目前使用的是 Multiboot1，而且回收内存涉及资源的复制，比较复杂，而且经过试验，发现这样的内存并不大，因此不去回收并不浪费。

- - -

## 安装 GRUB2 到 U 盘

首先插入 U 盘，解除挂载，然后格式化为 FAT 文件系统:

``` bash
sudo umount /dev/sdb1
sudo mkfs.vfat -F 32 -n WHEEL -I /dev/sdb
```

注意没有用 cfdisk 进行分区，这样创建的 U 盘是无分区的文件系统。

之后拔除 U 盘，然后重新插上，OS 应该能够自动检测并连接。

然后安装 Grub2 到 USB：

``` bash
sudo grub-install --root-directory=/media/szm/Wheel/ --no-floppy --recheck --force /dev/sdb
```

其中 `--root-directory` 的值就是 U 盘的挂载点。

这样 U 盘就能够自动引导了。但是还需要复制内核镜像、创建 GRUB 配置文件才可以。
