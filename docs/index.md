## 关于 Wheel 操作系统

Wheel 是我个人开发的操作系统（WIP），开发这个系统没有任何目的，纯粹是兴趣使然。

Wheel 是一个运行在 64 位 Intel/AMD CPU 上的操作系统。我在开发过程中，尽量使用最新的硬件标准，例如 64 位工作方式、多核、ACPI、APIC 中断等。

This OS aim to be new, without any backward compatibility, and use the lastest hardware standard, with one exception -- UEFI.

Wheel does not UEFI, instead, it use old BIOS. Structures like EBDA, SMBIOS and ACPI table are searched rather than queried from UEFI. Main reason is I didn't find any document on UEFI and GRUB, and I don't want to write the whole bootloader just for use UEFI.

Besides UEFI, all component is the latest possible. Wheel uses ACPI to acquire multiprocessor and APIC info, uses APIC to handle hardware interrupt and timing.

## 关于本文档

本文档是 Wheel 操作系统开发的参考，包含许多与 OS 开发相关的资料。

## Generated using MkDocs

这个文档使用 MkDocs 生成，详细使用方法参考 [mkdocs.org](http://mkdocs.org)，基本命令如下：

- `mkdocs new [dir-name]` - Create a new project.
- `mkdocs serve` - Start the live-reloading docs server.
- `mkdocs build` - Build the documentation site.
- `mkdocs help` - Print this help message.

一个标准 MkDocs 文档的源文件组织结构如下：

```
mkdocs.yml    # The configuration file.
docs/
    index.md  # The documentation homepage.
    ...       # Other markdown pages, images and other files.
```

其中 `mkdocs.yml` 包含了项目页面的组织结构。
