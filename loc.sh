#! /bin/bash
# count the lines of code

find . \
         -not \( -path "./old/*" -prune \)          \
         -not \( -path "./build/*" -prune \)        \
         -name \*.asm -o -name \*.c -o -name \*.h   \
| xargs wc -l
