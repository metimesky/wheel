#include "string.h"
#include <common/stdhdr.h>

// TODO: very naive implementation, must be refactored later.

void memcpy(void *dst, void *src, int n) {
    for (int i = 0; i < n; ++i) {
        ((char *) dst)[i] = ((char *) src)[i];
    }
}

void memmove(void *dst, void *src, int n) {
    if ((int) (dst - src) < n) {
        // copy backward
        for (int i = n - 1; i >= 0; --i) {
            ((char *) dst)[i] = ((char *) src)[i];
        }
    } else {
        for (int i = 0; i < n; ++i) {
            ((char *) dst)[i] = ((char *) src)[i];
        }
    }
}

void memset(void *dst, char c, int n) {
    for (int i = 0; i < n; ++i) {
        ((char *) dst)[i] = c;
    }
}

int memcmp(const void *s1, const void *s2, int n) {
    unsigned char *u1 = (const unsigned char *) s1;
    unsigned char *u2 = (const unsigned char *) s2;
    for (; n--; u1++, u2++) {
        if (*u1 != *u2) {
            return (*u1 - *u2);
        }
    }
    return 0;
}

// copy string from src to dst at most n bytes (excluding trailing zero)
// and return the actual number of bytes copied (excluding trailing zero)
// the return value should be same as strlen(dst)
char *strncpy(char *dst, const char *src, int n) {
    int i;
    for (i = 0; i < n && src[i]; ++i) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return dst;
}

char *strcpy (char *DstString, const char *SrcString) {
    char *String = DstString;

    /* Move bytes brute force */

    while (*SrcString) {
        *String = *SrcString;

        String++;
        SrcString++;
    }

    /* Null terminate */
    *String = 0;
    return (DstString);
}

// calculate the length of the string, excluding trailing zero.
int strlen(const char *str) {
    int len = 0;
    while (str[len]) { ++len; }
    return len;
}

int strcmp(const char *String1, const char *String2) {
    for (; (*String1 == *String2); String2++)  {
        if (!*String1++) {
            return (0);
        }
    }
    return ((unsigned char) *String1 - (unsigned char) *String2);
}

int strncmp(const char *String1, const char *String2, int Count) {
    for (; Count-- && (*String1 == *String2); String2++) {
        if (!*String1++) {
            return (0);
        }
    }
    return ((Count == ACPI_SIZE_MAX) ? 0 : ((unsigned char) *String1 -
        (unsigned char) *String2));
}

char *strchr(const char *String, int ch) {
    for (; (*String); String++) {
        if ((*String) == (char) ch) {
            return ((char *) String);
        }
    }
    return (NULL);
}

char *strcat(char *DstString, const char *SrcString) {
    char *String;

    /* Find end of the destination string */
    for (String = DstString; *String++; ) { ; }

    /* Concatenate the string */
    for (--String; (*String++ = *SrcString++); ) { ; }

    return (DstString);
}

char *strncat (char                    *DstString, const char              *SrcString,
    int               Count)
{
    char                    *String;


    if (Count)
    {
        /* Find end of the destination string */

        for (String = DstString; *String++; )
        { ; }

        /* Concatenate the string */

        for (--String; (*String++ = *SrcString++) && --Count; )
        { ; }

        /* Null terminate if necessary */

        if (!Count)
        {
            *String = 0;
        }
    }

    return (DstString);
}

char *
strstr (
    char                    *String1,
    char                    *String2)
{
    uint32_t                  Length;


    Length = strlen (String2);
    if (!Length)
    {
        return (String1);
    }

    while (strlen (String1) >= Length)
    {
        if (memcmp (String1, String2, Length) == 0)
        {
            return (String1);
        }
        String1++;
    }

    return (NULL);
}

uint32_t
strtoul (
    const char              *String,
    char                    **Terminator,
    uint32_t                  Base)
{
    uint32_t                  converted = 0;
    uint32_t                  index;
    uint32_t                  sign;
    const char              *StringStart;
    uint32_t                  ReturnValue = 0;
    ACPI_STATUS             Status = AE_OK;


    /*
     * Save the value of the pointer to the buffer's first
     * character, save the current errno value, and then
     * skip over any white space in the buffer:
     */
    StringStart = String;
    while (isspace (*String) || *String == '\t')
    {
        ++String;
    }

    /*
     * The buffer may contain an optional plus or minus sign.
     * If it does, then skip over it but remember what is was:
     */
    if (*String == '-')
    {
        sign = ACPI_SIGN_NEGATIVE;
        ++String;
    }
    else if (*String == '+')
    {
        ++String;
        sign = ACPI_SIGN_POSITIVE;
    }
    else
    {
        sign = ACPI_SIGN_POSITIVE;
    }

    /*
     * If the input parameter Base is zero, then we need to
     * determine if it is octal, decimal, or hexadecimal:
     */
    if (Base == 0)
    {
        if (*String == '0')
        {
            if (tolower (*(++String)) == 'x')
            {
                Base = 16;
                ++String;
            }
            else
            {
                Base = 8;
            }
        }
        else
        {
            Base = 10;
        }
    }
    else if (Base < 2 || Base > 36)
    {
        /*
         * The specified Base parameter is not in the domain of
         * this function:
         */
        goto done;
    }

    /*
     * For octal and hexadecimal bases, skip over the leading
     * 0 or 0x, if they are present.
     */
    if (Base == 8 && *String == '0')
    {
        String++;
    }

    if (Base == 16 &&
        *String == '0' &&
        tolower (*(++String)) == 'x')
    {
        String++;
    }

    /*
     * Main loop: convert the string to an unsigned long:
     */
    while (*String)
    {
        if (isdigit (*String))
        {
            index = (uint32_t) ((UINT8) *String - '0');
        }
        else
        {
            index = (uint32_t) toupper (*String);
            if (isupper (index))
            {
                index = index - 'A' + 10;
            }
            else
            {
                goto done;
            }
        }

        if (index >= Base)
        {
            goto done;
        }

        /*
         * Check to see if value is out of range:
         */

        if (ReturnValue > ((ACPI_uint32_t_MAX - (uint32_t) index) /
                            (uint32_t) Base))
        {
            Status = AE_ERROR;
            ReturnValue = 0;           /* reset */
        }
        else
        {
            ReturnValue *= Base;
            ReturnValue += index;
            converted = 1;
        }

        ++String;
    }

done:
    /*
     * If appropriate, update the caller's pointer to the next
     * unconverted character in the buffer.
     */
    if (Terminator)
    {
        if (converted == 0 && ReturnValue == 0 && String != NULL)
        {
            *Terminator = (char *) StringStart;
        }
        else
        {
            *Terminator = (char *) String;
        }
    }

    if (Status == AE_ERROR)
    {
        ReturnValue = ACPI_uint32_t_MAX;
    }

    /*
     * If a minus sign was present, then "the conversion is negated":
     */
    if (sign == ACPI_SIGN_NEGATIVE)
    {
        ReturnValue = (ACPI_uint32_t_MAX - ReturnValue) + 1;
    }

    return (ReturnValue);
}

#define islower(x) ('a' <= (x) && (x) <= 'z')
#define isupper(x) ('A' <= (x) && (x) <= 'Z')

int toupper (int c) {
    return (islower(c) ? ((c)-0x20) : (c));
}

int tolower (int c) {
    return (isupper(c) ? ((c)+0x20) : (c));
}

char *u32_to_str(unsigned int value, char *str, int base) {
    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

    /* Check whether base is valid. */
    if ((base < 2) || (base > 36)) {
        str[0] = '\0';
        return NULL;
    }

    /* Convert to string. Digits are in reverse order.  */
    int i = 0;
    unsigned int remainder = 0;
    do {
        remainder = value % base;
        str[i++] = digits[remainder];
        value = value / base;
    } while (value != 0);
    str[i] = '\0';

    /* Reverse string.  */
    char tmp;
    int j;
    for (j=0, --i; j<i; ++j, --i) {
        tmp = str[j];
        str[j] = str[i];
        str[i] = tmp;
    }

    return str;
}

char *u64_to_str(unsigned long long value, char *str, int base) {
    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  
    /* Check whether base is valid. */
    if ((base < 2) || (base > 36)) {
        str[0] = '\0';
        return NULL;
    }

    /* Convert to string. Digits are in reverse order.  */
    int i = 0;
    unsigned int remainder = 0;
    do {
        remainder = value % base;
        str[i++] = digits[remainder];
        value = value / base;
    } while (value != 0);
    str[i] = '\0';
  
    /* Reverse string.  */
    char tmp;
    int j;
    for (j=0, --i; j<i; ++j, --i) {
        tmp = str[j];
        str[j] = str[i];
        str[i] = tmp;
    }
  
    return str;
}

char *i32_to_str(int value, char *buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        return u32_to_str(-value, &buf[1], base);
    } else {
        return u32_to_str(value, buf, base);
    }
}

char *i64_to_str(long long value, char *buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        return u64_to_str(-value, &buf[1], base);
    } else {
        return u64_to_str(value, buf, base);
    }
}

/*
int vsprintf(char *buf, const char *fmt, va_list args) {
    const char *s;
    char *str;
    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }
        ++fmt;
        switch (*fmt) {
        case 'c':
            *str++ = (unsigned char)va_arg(args, int);
            break;
        case 's':
            s = va_arg(args, char *);
            for (int i = 0; s[i]; ++i) {
                *str++ = s[i];
            }
            break;
        case 'p':
            u64_to_str((uint64_t) va_arg(args, void *), str, 16);
            str += strlen(str);
            break;
        case '%':
            *str++ = '%';
            break;
        case 'o':
            u64_to_str(va_arg(args, uint64_t), str, 8);
            str += strlen(str);
            break;
        case 'x':
        case 'X':
            u64_to_str(va_arg(args, uint64_t), str, 16);
            str += strlen(str);
            break;
        case 'd':
        case 'i':
            i64_to_str(va_arg(args, int64_t), str, 10);
            str += strlen(str);
            break;
        case 'u':
            u64_to_str(va_arg(args, uint64_t), str, 10);
            str += strlen(str);
            break;
        default:
            *str++ = '%';
            if (*fmt) {
                *str++ = *fmt;
            } else {
                --fmt;
            }
            break;
        }
    }
    *str = '\0';
    return str - buf;
}
*/

// print to buf, at most n bytes (excluding trailing zero)
// return actual number of bytes written (excluding trailing zero)
int vsnprintf(char *buf, int n, const char *fmt, va_list args) {
    static char num[33];
    int i = 0;
    for (i = 0; i < n; ++i, ++fmt) {
        if (*fmt != '%') {
            buf[i] = *fmt;
        } else {
            ++fmt;
            switch (*fmt) {
            case 'c':
                buf[i] = (unsigned char) va_arg(args, int);
                break;
            case '%':
                buf[i] = '%';
                break;
            case 's':
                i += strncpy(&buf[i], va_arg(args, char*), n - i) - 1;
                break;
            case 'p':
            case 'x':
            case 'X':
                u64_to_str((uint64_t) va_arg(args, void *), num, 16);
                i += strncpy(&buf[i], num, n - i) - 1;
                // i += strlen(&buf[i]) - 1;
                break;
            case 'd':
            case 'i':
                i64_to_str((uint64_t) va_arg(args, void *), num, 10);
                i += strncpy(&buf[i], num, n - i) - 1;
                // i += strlen(&buf[i]) - 1;
                break;
            case 'u':
                u64_to_str((uint64_t) va_arg(args, void *), num, 10);
                i += strncpy(&buf[i], num, n - i) - 1;
                // i += strlen(&buf[i]) - 1;
                break;
            case 'o':
                u64_to_str((uint64_t) va_arg(args, void *), num, 8);
                i += strncpy(&buf[i], num, n - i) - 1;
                // i += strlen(&buf[i]) - 1;
                break;
            default:
                // *str++ = '%';
                // if (*fmt) {
                //     *str++ = *fmt;
                // } else {
                //     --fmt;
                // }
                break;
            }
        }
    }
    buf[i] = '\0';
    return i;
}

int snprintf(char *buf, int n, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, n, fmt, args);
    va_end(args);
}


////////////////////////////////////////////////////////////////////////////////

/*
 * Integer to string
 */
static void print_dec(unsigned int value, unsigned int width, char * buf, int * ptr ) {
    unsigned int n_width = 1;
    unsigned int i = 9;
    while (value > i && i < uint32_t_MAX) {
        n_width += 1;
        i *= 10;
        i += 9;
    }

    int printed = 0;
    while (n_width + printed < width) {
        buf[*ptr] = '0';
        *ptr += 1;
        printed += 1;
    }

    i = n_width;
    while (i > 0) {
        unsigned int n = value / 10;
        int r = value % 10;
        buf[*ptr + i - 1] = r + '0';
        i--;
        value = n;
    }
    *ptr += n_width;
}

/*
 * Hexadecimal to string
 */
static void print_hex(unsigned int value, unsigned int width, char * buf, int * ptr) {
    int i = width;

    if (i == 0) i = 8;

    unsigned int n_width = 1;
    unsigned int j = 0x0F;
    while (value > j && j < 0xffffffff) {
        n_width += 1;
        j *= 0x10;
        j += 0x0F;
    }

    while (i > (int)n_width) {
        buf[*ptr] = '0';
        *ptr += 1;
        i--;
    }

    i = (int)n_width;
    while (i-- > 0) {
        buf[*ptr] = "0123456789abcdef"[(value>>(i*4))&0xF];
        *ptr += + 1;
    }
}