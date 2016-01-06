#ifndef __ENV_H__
#define __ENV_H__ 1

// freestanding C library headers
#include <stdint.h>     // integer type definition
#include <stddef.h>     // size_t, wchar_t and NULL
#include <stdbool.h>    // bool, true, false
#include <limits.h>     // macros about integer range
#include <stdarg.h>     // variable length arguments

// data accessing helpers
#define DATA_U8(x)  (* ((uint8_t *)  (x)))
#define DATA_U16(x) (* ((uint16_t *) (x)))
#define DATA_U32(x) (* ((uint32_t *) (x)))
#define DATA_U64(x) (* ((uint64_t *) (x)))
#define DATA_I8(x)  (* ((int8_t *)  (x)))
#define DATA_I16(x) (* ((int16_t *) (x)))
#define DATA_I32(x) (* ((int32_t *) (x)))
#define DATA_I64(x) (* ((int64_t *) (x)))

#endif // __ENV_H__