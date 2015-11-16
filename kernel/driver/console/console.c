#include "console.h"
#include <common/util.h>

/**
    Future thoughts:
    Kernel use printf to print information to stdout. If stdout can be changed,
    that means the backend console_put_str function could have different
    implementation in different driver. So maybe we should define an interface
    and let each driver implement them, when activating each driver, the real
    implementation is associated with the kernel device interface.
 */

// vga register port definition
#define SEQUENCER_ADDR 0x03c4
#define SEQUENCER_DATA 0x03c5
#define CRT_CONTROLLER_ADDR 0x03d4
#define CRT_CONTROLLER_DATA 0x03d5
#define GRAPHICS_CONTROLLER_ADDR 0x03ce
#define GRAPHICS_CONTROLLER_DATA 0x03cf
#define ATTRIBUTE_CONTROLLER_ADDR 0x03c0

/**
    VGA video ram range 0xb8000~0xbffff, 32KB in total
    each line has 80 characters, each character has two bytes,
    so 32KB can have 204 lines.
    maximum `top_line` is 204 - 25 = 179
    in order to achieve infinite scroll, we have to map the last
    screen to the first screen to avoid flickering.
 */

static char *const video = (char *) 0xb8000;

static int line_num = -1;   // how many lines in total
static int width    = -1;
static int height   = -1;

static int top_line = -1;   // for scrolling
static int cursor_x = -1;
static int cursor_y = -1;

static uint8_t color = 0;

static bool cursor_visible = true;

// this function move the cursor to location specified by data member
static void console_move_cursor() {
    if (!cursor_visible) {
        return;
    }

    // calculate cursor position
    uint16_t data = width * cursor_y + cursor_x;

    // write cursor position to port
    out_byte(0x03d4, 0x0e);     // cursor location high
    out_byte(0x03d5, data >> 8);
    out_byte(0x03d4, 0x0f);     // cursor location low
    out_byte(0x03d5, data);
}

// this function scroll the screen by one line
// TODO: we have to clear the new lines that show up.
static void console_scroll_screen() {
    // the number 179 need to be computed
    if (top_line == 179) {
        // if we need to scroll back to the beginning of video buffer
        memcpy(video, video + top_line * 2 * width, 2 * width * height);
        top_line = 0;
        cursor_y = 0;
    } else {
        ++top_line;
    }

    // write data to port
    uint16_t data = top_line * width;
    out_byte(0x03d4, 0x0c);     // start address high
    out_byte(0x03d5, data >> 8);
    out_byte(0x03d4, 0x0d);     // start address low
    out_byte(0x03d5, data);
}

void console_init() {
    // console properties
    line_num = 204;  // 32K / (2*80)
    width = 80;
    height = 25;

    // initial state
    top_line = 0;
    cursor_x = 0;
    cursor_y = 0;

    // color
    color = 0x1f;   // white on blue

    // clear screen
    // memset(video, 0, width*height*2);
    for (int i = 0; i < width*line_num; ++i) {
        video[2 * i] = ' ';
        video[2 * i + 1] = color;
    }

    // place cursor in the right position
    console_move_cursor();
}

int console_get_width() {
    return width;
}

int console_get_height() {
    return height;
}

void console_set_cursor(int x, int y) {
    cursor_x = x;
    cursor_y = y;
    console_move_cursor();
}

// output a character and move cursor one position forward
void console_put_char(char c) {
    switch (c) {
    case '\t':
        cursor_x += 8 - cursor_x % 8;
        break;
    case '\n':
        cursor_x += 80 - cursor_x % 80;
        break;
    default:
        video[2 * (cursor_y * width + cursor_x)] = c;
        video[2 * (cursor_y * width + cursor_x) + 1] = color;
        ++cursor_x;
        break;
    }
    
    if (cursor_x >= width) {
        // if line is too long
        cursor_y += cursor_x / width;
        cursor_x = cursor_x % width;
        if (cursor_y - top_line >= height) {
            // if we need to scroll line
            // console_scroll_screen(cursor_y - top_line - height + 1);
            console_scroll_screen();
        }
    }

    console_move_cursor();
}

// output a string without and move cursor accordingly
void console_put_string(const char *str) {
    for (int i = 0; str[i]; ++i) {
        console_put_char(str[i]);
    }
}