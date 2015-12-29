#ifndef __LOGGING_H__
#define __LOGGING_H__ 1

/**
    This function is used to print kernel debug information.
    This usage is like printf or printk under Linux.
    Automatic append linebreak after it.
 */
void log(const char *fmt, ...);

#endif // __LOGGING_H__