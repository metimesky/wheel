#ifndef __WHEEL_H__
#define __WHEEL_H__

// 独立头文件
#include <stdint.h>     // integer type definition
#include <stddef.h>     // size_t, wchar_t and NULL
#include <stdbool.h>    // bool, true, false
#include <limits.h>     // macros about integer range
#include <stdarg.h>     // variable length arguments

// 内核空间起始虚拟地址
#define KERNEL_VMA 0xffff800000000000UL

// 将启动相关代码/数据放在单独的section，启动完成后可回收这部分内存
// TODO: 查看GCC手册，了解__init在函数声明中的正确位置
#define __init __attribute__((section(".init")))

#endif
