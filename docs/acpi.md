# ACPI

ACPI 是现代 PC 上用于管理电源的部分，程控关机/重启就是通过 ACPI 实现的。但是 ACPI 的功能不仅仅是管理供电，还能通过 ACPI 获取许多关键信息，例如多核规范中的 MADT，就可以通过 ACPI 得到。

## ACPICA

如果自己编写 ACPI 驱动，简单的功能还可以，一些复杂的功能就不那么方便了。因为 ACPI 的复杂性，已经出现了专门的平台无关实现——ACPICA，Linux 和很多商业 OS 都采用了它，因此说明 ACPICA 的质量很高。

但是使用 ACPICA 需要 OS 提供许多函数，因此在移植 ACPICA 之前最好将 OS 的基本功能实现完整。

ACPICA 的 Git 项目是开放的，但是获取开发版不推荐，因此在下载页面获取其 Unix 版本。将 ACPICA 包含到自己 OS 的最好做法就是直接复制源代码。但是需要 OS 提供一些函数，作为 OSL（OS Service Layer）。

下面的文字是从 OSDev 上摘下来的：

> I didn't find any good description of integrating the ACPICA source code into an operating system, and the released package is basically just a bundle of C files with little organization. This is what I ended up having to do:

> - I copied the C files from dispatcher/, events/, executer/, hardware/, parser/, namespace/, utilities/, tables/, and resources/ into a single acpi folder.
> - I copied the header files from include/
> - I created my own header file based on aclinux.h where I ripped out all of the userspace stuff, then I changed around the rest to be appropriate to my OS's definitions.
> - I edited the include/platform/acenv.h file to remove the inclusion of aclinux.h and included my header file instead.
> - I copied over acenv.h, acgcc.h, and my header file over to my include/platform/ folder.
> - This is in addition to writing an AcpiOs interface layer, and it is not well indicated by the reference manual that you have to actually edit header files. Many of the macros defined in the headers are documented, though.

## ACPICA 的架构

ACPICA 可以分为两个模块——ACPICA 子系统和 OSL（OS Service Layer），后者相当于 ACPICA 子系统和内核之间的桥梁，因此下面将 ACPICA 子系统简称为 ACPICA。

ACPICA 是与 OS 的实现独立的，OSL 是和特定 OS 相关的。内核使用 ACPI 功能时，直接调用 ACPICA 公开的函数。ACPICA 需要使用 OS 的服务时，会通过 OSL 进行调用。

ACPICA 分为许多子模块，例如 AML 解释器、ACPI 表管理器、命名空间管理、资源管理等，其中 AML 解释器是其他所有模块的基础。

- ACPI 表管理器。ACPI 规范定义了很多数据结构，称作 ACPI 表，例如 XSDT、FADT、MADT 等。由于 OS 可能在还没有完全启动的时候就需要相关表结构，因此这个模块可以独立于其他模块使用。
- AML 解释器。ACPI 的很多信息是以 AML 字节码的形式提供的，AML 解释器是其他模块能正常工作的前提。

### 使用

事实证明，下载 Unix 版本的 ACPICA，而不是 Github 上的源代码，能够避免很多不必要的麻烦。

使用的方法就是将源代码复制过来，但是 ACPICA 的目录结构已经固定，引用头文件的时候并没有使用相对路径，因此需要对源文件逐一进行修改。可以使用下面的命令批量处理：

``` bash
for f in components/*/*.h
do
perl -pi -e 's/^#include "/#include "\.\.\/\.\.\/include\//g' $f
done
```

或者写成函数的形式：

``` bash
function magic {
    for f in $1/*.c
    do
        perl -pi -e 's/^#include "/#include "\.\.\/\.\.\/include\//g' $f
    done
}
```

### 配置文件

`include` 目录下都是头文件，其中有几个属于配置文件，其中定义的宏会影响到其他组件的编译行为：
- 全局头文件 `include/acconfig.h`，包含一些通用的常量和选项
- 与特定目标平台相关的头文件，如 `include/platform/aclinux.h`
- 与使用的编译器相关的头文件，如 `include/platform/acgcc.h`

#### Component Selection

``` c
#undef ACPI_DISASSEMBLER
#undef ACPI_DEBUGGER
#define ACPI_REDUCED_HARDWARE FALSE

typedef ACPI_SPINLOCK (void*);
#define ACPI_SEMAPHORE (void*)
#define ACPI_MUTEX (void*)

#undef ACPI_USE_LOCAL_CACHE // since Wheel has SLAB
```

论坛上很多人说在`aclinux.h`的基础上进行修改，但是我发现这样做错误很多，还不如直接将整个文件重写一遍。OSL部分的函数需要全都提供，OSDevWiki上列出的那些太少了，可能是版本比较老，目前版本的ACPICA需要不少的OSL函数。

ACPICA是一个跨操作系统的实现，这一点没有错误，因此如果使用过程中发现了问题，十有八九是出在配置文件上。配置文件中主要的就是宏定义，如果定义了一个宏，就表示开启一个功能；要禁用一个功能，就取消定义（undef）。ACPICA的文档里对这些配置的参数宏进行了说明，最好参照文档逐个选项进行配置。

ACPICA的代码不用修改，最多只是改include/platform目录中的文件，把其中不需要的文件删除，只留下acenv.h、acgcc.h，再加上本OS的配置文件（参照文档写好其中的每一条配置宏）。

- - -

## ACPICA Table Management

这是 ACPICA 的一个组件，但是表管理组件相对特殊，因为它相对独立，可以在系统启动初期运行，即使没有内存管理也可以。这个特点非常方便系统开发，因此内核初始化的时候非常需要访问ACPI表，查询APIC相关信息。

- - -

## Precopiled Header

`acpi.h` 文件内容很多，编译非常耗时。尝试采用预编译头文件加速编译失败。

使用 PCH 有一些特殊的要求。假设预编译的是 `acpi.h`，那么对 `acpi.h` 的引用不能位于任何非预处理指令之后。如果某个文件中，首先定义了某个宏，然后引用这个 `acpi.h`，那么引用之前的宏定义不会影响这个预编译好的头文件。

预编译头文件的本质起始非常原始，只是把经过了预处理的头文件内部表示存储下来。如果 `acpi.h` 的引用之前有其他的宏定义，那么可能影响到 `acpi.h` 的表示，因此预编译的结果会不同。GCC 文档里有这样的内容：

> Any macros defined before the precompiled header is included must either be defined in the same way as when the precompiled header was generated, or must not affect the precompiled header, which usually means that they don't appear in the precompiled header at all.

然而，ACPICA 的代码中，存在首先定义某个宏然后引用 `acpi.h` 的例子，而且这些宏会影响到 `acpi.h` 的解析。

## Speed Up

现在的Makefile中，改动一个头文件就要重新编译全部的C文件，这是由Makefile的规则决定的：

``` make
$(dst_dir)/%.c.o: %.c $(headers)
```

自动生成以来文件。
