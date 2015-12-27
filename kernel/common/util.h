#ifndef __UTIL_H__
#define __UTIL_H__ 1

#include <library/cpu.h>
#include <library/string.h>
#include <library/bitmap.h>
#include <library/logging.h>

// casting
#define U32(x) (* ((uint32_t *) (x)))
#define U64(x) (* ((uint64_t *) (x)))

#endif // __UTIL_H__