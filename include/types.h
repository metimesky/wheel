#ifndef __TYPES_H__
#define __TYPES_H__ 1

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;

typedef uint32_t            size_t;

#define NULL ((void*)0)

#define INT32_MIN -2147483648
#define INT32_MAX 2147483647

#define UINT32_MIN 0
#define UINT32_MAX 4294967296

#define INT64_MIN -9223372036854775808L
#define INT64_MAX 9223372036854775807L

#define UINT64_MIN 0L
#define UINT64_MAX 18446744073709551616L

#endif // __TYPES_H__