#! /bin/bash
# count the lines of code

find .                                                  \
    -not \( -path "./build" -prune \)                   \
    -not \( -path "./kernel/drivers/acpi" -prune \)     \
    -name \*.asm -o -name \*.c -o -name \*.h            \
| xargs wc -l                                   \
    ./kernel/drivers/acpi/platform/osl.c        \
    ./kernel/drivers/acpi/platform/wheelex.c
