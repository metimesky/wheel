#ifndef __WHEEL_H__
#define __WHEEL_H__

// 独立头文件
#include <stdint.h>     // integer type definition
#include <stddef.h>     // size_t, wchar_t and NULL
#include <stdbool.h>    // bool, true, false
#include <limits.h>     // macros about integer range
#include <stdarg.h>     // variable length arguments

// 全局宏定义
#define KERNEL_VMA 0xffff800000000000UL
#define __init __attribute__((section(".init")))

#endif
