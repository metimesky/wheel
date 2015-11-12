#include "console.h"
#include <util.h>

static char *const video = (char *) 0xb8000;

static int width    = -1;
static int height   = -1;
static int cursor_x = -1;
static int cursor_y = -1;

static bool cursor_visible = true;

// this function move the cursor to location specified by data member
static void console_move_cursor() {
    if (!cursor_visible) {
        return;
    }

    // calculate cursor position
    uint16_t loc = width * cursor_y + cursor_x;

    // write cursor position to port
    out_byte(0x3D4, 14);
    out_byte(0x3D5, loc >> 8);
    out_byte(0x3D4, 15);
    out_byte(0x3D5, loc);
}

void console_init() {
    // assign value to all member variables
    width = 80;
    height = 25;
    cursor_x = 0;
    cursor_y = 0;

    // clear screen
    memset(video, 0, 80*25*2);

    // place cursor in the right position
    console_move_cursor();
}

int console_get_width() {
    return 80;
}

int console_get_height() {
    return 25;
}

