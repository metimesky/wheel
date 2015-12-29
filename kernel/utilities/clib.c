#include "clib.h"

/* Currently all functions are naive implementation.
 */

// check if address is size_t aligned, 0 if aligned
#define UNALIGNED(x) (((size_t) (x)) & (sizeof(size_t) - 1))

// check if x contains an all-zero byte
#define haszero(x) (((x) - 0x0101010101010101UL) & ~(x) & 0x8080808080808080UL)

// check if x contains a byte of value n
#define hasvalue(x,n) (haszero((x) ^ (~0UL/255 * (n))))

////////////////////////////////////////////////////////////////////////////////
/// ctype functions
////////////////////////////////////////////////////////////////////////////////
char tolower(char c) {
    return isupper(c) ? c+0x20 : c;
}

char toupper(char c) {
    return islower(c) ? c-0x20 : c;
}

////////////////////////////////////////////////////////////////////////////////
/// memory functions
////////////////////////////////////////////////////////////////////////////////

void *memcpy(void *dst, const void *src, size_t n) {
    unsigned char *vdst = (unsigned char *) dst;
    const unsigned char *vsrc = (const unsigned char *) src;
    for (; n; --n, ++vdst, ++vsrc) {
        *vdst = *vsrc;
    }
}

void *memmove(void *dst, const void *src, size_t n) {
    unsigned char *vdst = (unsigned char *) dst;
    const unsigned char *vsrc = (const unsigned char *) src;
    if (vsrc < vdst && vdst < vsrc + n) {
        // copy region overlaps, have to move backwards
        vdst += n;
        vsrc += n;
        for (; n; --n, --vdst, --vsrc) {
            *vdst = *vsrc;
        }
    } else {
        // else, just use normal memcpy
        return memcpy(dst, src, n);
    }
    return dst;
}

int memcmp(const void *ptr1, const void *ptr2, size_t n) {
    const unsigned char *vptr1 = (const unsigned char *) ptr1;
    const unsigned char *vptr2 = (const unsigned char *) ptr2;
    for (; n; --n, ++vptr1, ++vptr2) {
        if (*vptr1 > *vptr2) {
            return 1;
        } else if (*vptr1 < *vptr2) {
            return -1;
        }
    }
    return 0;
}

void *memchr(const void *ptr, int value, size_t n) {
    const unsigned char *vptr = (const unsigned char *) ptr;
    unsigned char ch = (unsigned char) value & 0xff;
    for (; n; --n, ++vptr) {
        if (*vptr == ch) {
            return (void *) vptr;
        }
    }
    return NULL;
}

void *memset(void *ptr, int value, size_t n) {
    unsigned char* vptr = (unsigned char *) ptr;
    unsigned char ch = (unsigned char) value & 0xff;
    for (; n; --n, ++vptr) {
        *vptr = ch;
    }
    return ptr;
}

////////////////////////////////////////////////////////////////////////////////
/// string functions
////////////////////////////////////////////////////////////////////////////////

size_t strlen(const char *str) {
    size_t len = 0;
    for (; *str; ++str, ++len) {}
    return len;
}

char *strcpy(char *dst, const char *src) {
    return memcpy(dst, src, strlen(src) + 1);
}

char *strncpy(char *dst, const char *src, size_t n) {
    size_t len = strlen(src) + 1;
    if (len > n) {
        len = n;
    }
    memcpy(dst, src, len);
    if (len < n) {
        memset(dst + len, '\0', n - len);
    }
    return dst;
}

char *strcat(char *s1, const char *s2) {
    strcpy(s1 + strlen(s1), s2);
    return s1;
}

char *strncat(char *s1, const char *s2, size_t n) {
    size_t n1 = strlen(s1);
    size_t n2 = strlen(s2);
    if (n2 > n) {
        n2 = n;
    }
    strncpy(s1 + n1, s2, n2);
    s1[n1 + n2] = '\0';
    return s1;
}

int strcmp(const char *s1, const char *s2) {
    size_t n1 = strlen(s1);
    size_t n2 = strlen(s2);
    
    int res = memcmp(s1, s2, (n1 < n2) ? n1 : n2);
    if (res != 0){
        return res;
    }
    if (n1 > n2){
        return 1;
    } else if (n1 < n2) {
        return -1;
    }
    return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    return memcmp(s1, s2, n);
}

char *strchr(const char *str, int value) {
    return memchr(str, value, strlen(str) + 1);
}

// find the first occurence of s2 in s1
char *strstr(const char *s1, const char *s2) {
    size_t n2 = strlen(s2);
    for (; *s1; ++s1) {
        for (int i = 0; i < n2; ++i) {
            if (s2[i] == '\0') {
                return (char *) s1;
            }
            if (s2[i] == *s1) {
                continue;
            }
            break;
        }
    }
    return NULL;
}

// return the literal value of digit, range 0~36, return -1 when error
static int digit_value(char c) {
    switch (c) {
    case '0':   case '1':   case '2':   case '3':   case '4':
    case '5':   case '6':   case '7':   case '8':   case '9':
        return c - '0';
    default:
        if (isalpha(c)) {
            return tolower(c) - 'a' + 10;
        } else {
            return -1;
        }
    }
}

// convert string to integer, if base is zero, guess the radix
unsigned int strtoul(const char *str, char **end, int base) {
    const char *p = str;

    // skip leading blanks
    while (isspace(*p)) { ++p; }

    // sign
    if (*p == '-') {
        ++p;
    } else if (*p == '+') {
        ++p;
    }

    if (base == 0) {
        // if base not supplied, then guess one from 8, 10, 16
        if (*p == '0') {
            ++p;
            if (*p == 'x' || *p == 'X') {
                ++p;
                base = 16;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        // skip the leading '0x'
        if (p[0] == '0' && tolower(p[1]) == 'x') {
            p += 2;
        }
    }

    unsigned int num = 0;

    for (; *p; ++p) {
        int val = digit_value(*p);
        if (val == -1 || val >= base) {
            break;
        }
        num *= base;
        num += val;
    }

    if (end != 0) {
        *end = (char *) p;
    }

    return num;
}

unsigned long long strtoull(const char *str, char **end, int base) {
    const char *p = str;

    // skip leading blanks
    while (isspace(*p)) { ++p; }

    // sign
    if (*p == '-') {
        ++p;
    } else if (*p == '+') {
        ++p;
    }

    if (base == 0) {
        // if base not supplied, then guess one from 8, 10, 16
        if (*p == '0') {
            ++p;
            if (*p == 'x' || *p == 'X') {
                ++p;
                base = 16;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        // skip the leading '0x'
        if (p[0] == '0' && tolower(p[1]) == 'x') {
            p += 2;
        }
    }

    unsigned long long num = 0;

    for (; *p; ++p) {
        int val = digit_value(*p);
        if (val == -1 || val >= base) {
            break;
        }
        num *= base;
        num += val;
    }

    if (end != 0) {
        *end = (char *) p;
    }

    return num;
}

////////////////////////////////////////////////////////////////////////////////
/// stdio functions
////////////////////////////////////////////////////////////////////////////////

static int snprintf(char* buf, size_t n, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        n = vsnprintf(buf, n, fmt, args);
        va_end(args);
        return n;
}

// n is the maximun capacity of buf, return how many bytes are put into buf
int vsnprintf(char *buf, size_t n, const char *fmt, va_list args) {
    // TODO: 'n' is not handled
    // TODO: 'e', 'f', 'g', 'n' specifiers to do
    // TODO: function doesn't return the number of printed characters
    
    if (n == 0) {
        return 0;
    }

    char *original_buf = buf;
    
    // we loop through each character of the fmt
    while (*fmt != '\0' && buf - original_buf < n) {
        // first we handle the most common case: a normal character
        if (*fmt != '%') {
            *buf++ = *fmt++;
            continue;
        }
            
        // then we check if fmt is "%%"
        ++fmt;
        if (*fmt == '%') {
            *buf++ = '%';
            ++fmt;
            continue;
        }

        // now we are sure that we are in the real trouble
        // what we do is that we store flags, width, precision, length in variables
        bool sharpFlag = false;
        bool alignLeft = false;
        bool alwaysSign = false;
        bool noSign = false;
        bool padding = ' ';
        int minimumWidth = 0;
        int precision = 1;
        bool numberMustBeShort = false;
        bool numberMustBeLong = false;
        bool unsignedNumber = false;
        bool capitalLetters = false;
        bool octal = false;
        bool hexadecimal = false;
        bool pointer = false;
        bool tagFinished = false;

        // then we loop (and we modify variables) until we find a specifier
        do {
            switch (*fmt) {
            /// flags
            case '-':   alignLeft   = true; ++fmt;  break;
            case '+':   alwaysSign  = true; ++fmt;  break;
            case ' ':   noSign      = true; ++fmt;  break;
            case '0':   padding     = '0';  ++fmt;  break;
            case '#':   sharpFlag   = true; ++fmt;  break;

            case '1':   case '2':   case '3':   case '4':   // width cannot start with 0 or it would be a flag
            case '5':   case '6':   case '7':   case '8':   case '9':
                // minimumWidth = atoi(fmt);
                for (; '0' <= *fmt && *fmt <= '9'; ++fmt) {
                    minimumWidth *= 10;
                    minimumWidth += *fmt - '0';
                }
                break;
            case '*':
                minimumWidth = va_arg(args, int);
                ++fmt;
                break;

            /// precision
            case '.':
                ++fmt;
                if (*fmt == '*') {
                    precision = va_arg(args, int);
                    ++fmt;
                } else if ('0' <= *fmt && *fmt <= '9') {
                    // precision = atoi(fmt);
                    for (; '0' <= *fmt && *fmt <= '9'; ++fmt) {
                        precision *= 10;
                        precision += *fmt - '0';
                    }
                } else {
                    precision = 0;          // this behavior is standardized
                }
                break;
                    
            /// length
            case 'h': numberMustBeShort = true; ++fmt;  break;
            case 'l':
            case 'L': numberMustBeLong  = true; ++fmt;  break;
            
            /// specifiers
            
            /// bufings
            case 's': {
                char* nStr = va_arg(args, char*);
                size_t len = strlen(nStr);
                
                if (!alignLeft && len < minimumWidth) {
                        while (len++ < minimumWidth)
                                *buf++ = padding;
                }
                
                while (*nStr)
                        *buf++ = *nStr++;
                
                if (alignLeft && len < minimumWidth) {
                        while (len++ < minimumWidth)
                                *buf++ = padding;
                }
                
                fmt++;
                tagFinished = true;
                break;
            }

            /// characters
            case 'c': {
                char toWrite = (char) va_arg(args, int);
                if (!alignLeft) {
                    for (; minimumWidth > 1; --minimumWidth) {
                        *buf++ = padding;
                    }
                }
                *buf++ = toWrite;
                if (alignLeft) {
                    for (; minimumWidth > 1; --minimumWidth) {
                        *buf++ = padding;
                    }
                }
                ++fmt;
                tagFinished = true;
                break;
            }
            
            /// numbers
            case 'o':   octal           = true;
            case 'p':   pointer         = true;
            case 'X':   capitalLetters  = true;
            case 'x':   hexadecimal     = true;
            case 'u':   unsignedNumber  = true;
            case 'd':
            case 'i': {
                // first we handle problems with our switch-case
                if (octal) {
                    pointer = false;
                    hexadecimal = false;
                    unsignedNumber = false;
                }
                
                // then we retreive the value to write
                unsigned long int toWrite;
                if (numberMustBeLong) {
                    toWrite = va_arg(args, long int);
                } else if (numberMustBeShort) {
                    toWrite = (short int) va_arg(args, int);
                } else if (pointer) {
                    toWrite = (unsigned long int) va_arg(args, void*);
                } else {
                    toWrite = va_arg(args, int);
                }
                
                // handling sign
                if (!noSign) {
                    bool positive = (unsignedNumber || (((signed)toWrite) > 0));
                    if (alwaysSign || !positive) {
                        *buf++ = (positive ? '+' : '-');
                    }
                    if (!unsignedNumber && (((signed)toWrite) < 0)) {
                        toWrite = -((signed)toWrite);
                    }
                }
                
                if (sharpFlag && toWrite != 0) {
                    if (octal || hexadecimal) {
                        *buf++ = '0';
                    }
                    if (hexadecimal) {
                        *buf++ = capitalLetters ? 'X' : 'x';
                    }
                }
                
                // writing number
                int digitSwitch = 10;
                if (hexadecimal) {
                    digitSwitch = 16;
                } else if (octal) {
                    digitSwitch = 8;
                }
                
                // this variable will be usefull
                char* baseStr = buf;
                
                int numDigits = 0;
                do {
                    if (numDigits) {
                        memmove(baseStr + 1, baseStr, numDigits * sizeof(char));
                    }
                    int modResult = toWrite % digitSwitch;
                    if (modResult < 10) {
                        *baseStr = '0' + modResult;
                        ++buf;
                    } else if (capitalLetters) {
                        *baseStr = 'A' + (modResult - 10);
                        ++buf;
                    } else {
                        *baseStr = 'a' + (modResult - 10);
                        ++buf;
                    }
                    toWrite /= digitSwitch;
                    numDigits++;
                } while (toWrite != 0);
                
                if (numDigits < minimumWidth) {
                    minimumWidth -= numDigits;
                    if (alignLeft) {
                        for (; minimumWidth > 0; --minimumWidth) {
                            *buf++ = padding;
                        }
                    } else {
                        memmove(baseStr + minimumWidth * sizeof(char), baseStr, numDigits * sizeof(char));
                        memset(baseStr, padding, minimumWidth * sizeof(char));
                        buf += minimumWidth;
                    }
                }
                
                // finished
                ++fmt;
                tagFinished = true;
                break;
            }
            
            default:
                ++fmt;
                tagFinished = true;
                break;
            }
        } while (!tagFinished);
    }
    *buf = '\0';
    return 1;
}

/*
void *memcpy_o(void *dst, const void *src, size_t n) {
    uint8_t *bdst = (uint8_t *) dst;
    uint8_t *bsrc = (uint8_t *) src;
    if (n >= 4 * sizeof(uint64_t) && !UNALIGNED(src) && !UNALIGNED(dst)) {
        uint64_t *qdst = (uint64_t *) dst;
        uint64_t *qsrc = (uint64_t *) src;
        for (; n >= 4 * sizeof(uint64_t); n -= 4 * sizeof(uint64_t)) {
            *qdst++ = *qsrc++;
            *qdst++ = *qsrc++;
            *qdst++ = *qsrc++;
            *qdst++ = *qsrc++;
        }
        bdst = qdst;
        bsrc = qsrc;
    }
    for (; n; --n) {
        *bdst++ = *bsrc++;
    }
    return dst;
}
*/
