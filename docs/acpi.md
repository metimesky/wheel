# ACPI

ACPI 是现代 PC 上用于管理电源的部分，程控关机/重启就是通过 ACPI 实现的。但是 ACPI 的功能不仅仅是管理供电，还能通过 ACPI 获取许多关键信息，例如多核规范中的 MADT，就可以通过 ACPI 得到。

### ACPICA

如果自己编写 ACPI 驱动，简单的功能还可以，一些复杂的功能就不那么方便了。因为 ACPI 的复杂性，已经出现了专门的平台无关实现——ACPICA，Linux 和很多商业 OS 都采用了它，因此说明 ACPICA 的质量很高。

但是使用 ACPICA 需要 OS 提供许多函数，因此在移植 ACPICA 之前最好将 OS 的基本功能实现完整。

ACPICA 的 Git 项目是开放的，但是获取开发版不推荐，因此在下载页面获取其 Unix 版本。将 ACPICA 包含到自己 OS 的最好做法就是直接复制源代码。但是需要 OS 提供一些函数，作为 OSL（OS Service Layer）。

I didn't find any good description of integrating the ACPICA source code into an operating system, and the released package is basically just a bundle of C files with little organization. This is what I ended up having to do:

I copied the C files from dispatcher/, events/, executer/, hardware/, parser/, namespace/, utilities/, tables/, and resources/ into a single acpi folder.
I copied the header files from include/
I created my own header file based on aclinux.h where I ripped out all of the userspace stuff, then I changed around the rest to be appropriate to my OS's definitions.
I edited the include/platform/acenv.h file to remove the inclusion of aclinux.h and included my header file instead.
I copied over acenv.h, acgcc.h, and my header file over to my include/platform/ folder.
This is in addition to writing an AcpiOs interface layer, and it is not well indicated by the reference manual that you have to actually edit header files. Many of the macros defined in the headers are documented, though.
