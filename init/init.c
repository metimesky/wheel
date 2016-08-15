#include <wheel.h>
#include <drivers/console.h>

void init() {
    console_init();

    console_print("Welcome to Wheel OS!\n");

    while (1) { }
}