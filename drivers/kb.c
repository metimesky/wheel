// 8042 PS/2 Keyboard driver

#include <drivers/kb.h>
#include <interrupt.h>
#include <lib/cpu.h>
#include <drivers/console.h>

/*
目前，键盘所有的输入都会通过console打印在屏幕上。
最好的设计，应该是将键盘设备看作一个文件，其他进程可以从这个文件读取用户输入的内容。
设备驱动维护两个缓冲区，进行热切换。将扫描码直接存进缓冲区。

不同的进程可能会通过不同的方法使用键盘，普通程序接收字符输入，而游戏这类程序接收纯粹的按键事件。
*/

#define KB_DATA 0x60
#define KB_STAT 0x64    // read only
#define KB_CMD  0x64    // write only

static bool stat_caps_lock;
static bool stat_nums_lock;
static bool stat_scroll_lock;

static bool stat_lctrl_down;
static bool stat_rctrl_down;
static bool stat_lalt_down;
static bool stat_ralt_down;
static bool stat_lshift_down;
static bool stat_rshift_down;

static inline bool is_shift() {
    return stat_lshift_down || stat_lshift_down;
}

static inline bool is_upcase() {
    return (stat_caps_lock ^ (stat_lshift_down || stat_rshift_down));
}

static inline void real_handler() {
    uint8_t scan = in_byte(0x60);
    if (scan & 0x80) {
        // key release event
        // console_print("^");
        switch (scan & 0x7f) {
        case 0x1d:  // left control
            stat_lctrl_down = false;
            break;
        case 0x2a:  // left shift
            stat_lshift_down = false;
            break;
        case 0x36:  // right shift
            stat_rshift_down = false;
            break;
        case 0x38:  // left alt
            stat_lalt_down = false;
            break;
        default:
            break;
        }
    } else {
        // key press event
        // console_print("v");
        switch (scan & 0x7f) {
        case 0x01:  // escape
            break;
        case 0x02:  // 1
            console_print(is_shift()?"!":"1");
            break;
        case 0x03:  // 2
            console_print(is_shift()?"@":"2");
            break;
        case 0x04:  // 3
            console_print(is_shift()?"#":"3");
            break;
        case 0x05:  // 4
            console_print(is_shift()?"$":"4");
            break;
        case 0x06:  // 5
            console_print(is_shift()?"%":"5");
            break;
        case 0x07:  // 6
            console_print(is_shift()?"^":"6");
            break;
        case 0x08:  // 7
            console_print(is_shift()?"&":"7");
            break;
        case 0x09:  // 8
            console_print(is_shift()?"*":"8");
            break;
        case 0x0a:  // 9
            console_print(is_shift()?"(":"9");
            break;
        case 0x0b:  // 0
            console_print(is_shift()?")":"0");
            break;
        case 0x0c:  // -
            console_print(is_shift()?"_":"-");
            break;
        case 0x0d:  // =
            console_print(is_shift()?"+":"=");
            break;
        case 0x0e:  // backspace
            break;
        case 0x0f:  // tab
            console_print("\t");
            break;
        case 0x10:  // Q
            console_print(is_upcase()?"Q":"q");
            break;
        case 0x11:  // W
            console_print(is_upcase()?"W":"w");
            break;
        case 0x12:  // E
            console_print(is_upcase()?"E":"e");
            break;
        case 0x13:  // R
            console_print(is_upcase()?"R":"r");
            break;
        case 0x14:  // T
            console_print(is_upcase()?"T":"t");
            break;
        case 0x15:  // Y
            console_print(is_upcase()?"Y":"y");
            break;
        case 0x16:  // U
            console_print(is_upcase()?"U":"u");
            break;
        case 0x17:  // I
            console_print(is_upcase()?"I":"i");
            break;
        case 0x18:  // O
            console_print(is_upcase()?"O":"o");
            break;
        case 0x19:  // P
            console_print(is_upcase()?"P":"p");
            break;
        case 0x1a:  // [
            console_print(is_shift()?"{":"[");
            break;
        case 0x1b:  // ]
            console_print(is_shift()?"}":"]");
            break;
        case 0x1c:  // ENTER
            console_print("\n");
            break;
        case 0x1d:  // left control
            stat_lctrl_down = true;
            break;
        case 0x1e:  // A
            console_print(is_upcase()?"A":"a");
            break;
        case 0x1f:  // S
            console_print(is_upcase()?"S":"s");
            break;
        case 0x20:  // D
            console_print(is_upcase()?"D":"d");
            break;
        case 0x21:  // F
            console_print(is_upcase()?"F":"f");
            break;
        case 0x22:  // G
            console_print(is_upcase()?"G":"g");
            break;
        case 0x23:  // H
            console_print(is_upcase()?"H":"h");
            break;
        case 0x24:  // J
            console_print(is_upcase()?"J":"j");
            break;
        case 0x25:  // K
            console_print(is_upcase()?"K":"k");
            break;
        case 0x26:  // L
            console_print(is_upcase()?"L":"l");
            break;
        case 0x27:  // ;
            console_print(is_shift()?":":";");
            break;
        case 0x28:  // '
            console_print(is_shift()?"\"":"'");
            break;
        case 0x29:  // `
            console_print(is_shift()?"~":"`");
            break;
        case 0x2a:  // left shift
            stat_lshift_down = true;
            break;
        case 0x2b:  // `\`
            console_print(is_shift()?"|":"\\");
            break;
        case 0x2c:  // Z
            console_print(is_upcase()?"Z":"z");
            break;
        case 0x2d:  // X
            console_print(is_upcase()?"X":"x");
            break;
        case 0x2e:  // C
            console_print(is_upcase()?"C":"c");
            break;
        case 0x2f:  // V
            console_print(is_upcase()?"V":"v");
            break;
        case 0x30:  // B
            console_print(is_upcase()?"B":"b");
            break;
        case 0x31:  // N
            console_print(is_upcase()?"N":"n");
            break;
        case 0x32:  // M
            console_print(is_upcase()?"M":"m");
            break;
        case 0x33:  // ,
            console_print(is_shift()?"<":",");
            break;
        case 0x34:  // .
            console_print(is_shift()?">":".");
            break;
        case 0x35:  // /
            console_print(is_shift()?"?":"/");
            break;
        case 0x36:  // right shift
            stat_rshift_down = true;
            break;
        case 0x37:  // keypad *
            console_print("*");
            break;
        case 0x38:  // left alt
            stat_lalt_down = true;
            break;
        case 0x39:  // space
            console_print(" ");
            break;
        case 0x3a:  // caps lock
            stat_caps_lock ^= 1;
            break;
        case 0x3b:  // F1
        case 0x3c:  // F2
        case 0x3d:  // F3
        case 0x3e:  // F4
        case 0x3f:  // F5
        case 0x40:  // F6
        case 0x41:  // F7
        case 0x42:  // F8
        case 0x43:  // F9
        case 0x44:  // F10
            break;
        case 0x45:  // num lock
            stat_nums_lock ^= 1;
            break;
        case 0x46:  // scroll lock
            stat_scroll_lock ^= 1;
            break;
        case 0x47:  // keypad 7
            if (stat_nums_lock) {
                console_print("7");
            }
            break;
        case 0x48:  // keypad 8
            if (stat_nums_lock) {
                console_print("8");
            }
            break;
        case 0x49:  // keypad 9
            if (stat_nums_lock) {
                console_print("9");
            }
            break;
        case 0x4a:  // keypad -
            console_print("-");
            break;
        case 0x4b:  // keypad 4
            if (stat_nums_lock) {
                console_print("4");
            }
            break;
        case 0x4c:  // keypad 5
            if (stat_nums_lock) {
                console_print("5");
            }
            break;
        case 0x4d:  // keypad 6
            if (stat_nums_lock) {
                console_print("6");
            }
            break;
        case 0x4e:  // keypad +
            console_print("+");
            break;
        case 0x4f:  // keypad 1
            if (stat_nums_lock) {
                console_print("1");
            }
            break;
        case 0x50:  // keypad 2
            if (stat_nums_lock) {
                console_print("2");
            }
            break;
        case 0x51:  // keypad 3
            if (stat_nums_lock) {
                console_print("3");
            }
            break;
        case 0x52:  // keypad 0
            if (stat_nums_lock) {
                console_print("0");
            }
            break;
        case 0x53:  // keypad .
            console_print(".");
            break;
        // case 0x54:  //
        // case 0x55:  //
        // case 0x56:  //
        case 0x57:  // F11
        case 0x58:  // F12
            break;
        default:
            break;
        }
    }
}

static void kb_pic_handler() {
    real_handler();
    pic_send_eoi(1);
}

static void kb_apic_handler() {
    real_handler();
    local_apic_send_eoi();
}

void __init kb_init() {
    console_print("initializing keyboard");
    idt_set_int_handler(IRQ_VEC_BASE + 1, kb_pic_handler);
    idt_set_int_handler(GSI_VEC_BASE + io_apic_irq_to_gsi(1), kb_apic_handler);

    // 检查ACPI::FADT->IA_PC_Boot_Architecture_Flags bit 1，判断键盘设备是否存在
    // 如果这个位是1，则存在键盘
if (0) {
    out_byte(KB_CMD, 0xad);
    while (in_byte(KB_STAT) & 2) { }

    out_byte(KB_CMD, 0xa7);
    while (in_byte(KB_STAT) & 2) { }
}

    // controller configuration byte
    out_byte(KB_CMD, 0x20);
    while (in_byte(KB_STAT) & 2) { }
    uint8_t conf = in_byte(KB_DATA);

    conf |= (1U << 0);
    conf |= (1U << 1);
    conf |= (1U << 6);

    out_byte(KB_CMD, 0x60);
    while (in_byte(KB_STAT) & 2) { }
    out_byte(KB_DATA, conf);

    // flush output buffer
    while (in_byte(KB_STAT) & 1) {
        console_print(".");
        in_byte(KB_DATA);
    }
    console_print("\n");

if (0) {
    // reset LED lights
    out_byte(KB_CMD, 0xed);
    out_byte(KB_DATA, 0);
}
    
    stat_nums_lock = false;
    stat_caps_lock = false;
    stat_scroll_lock = false;

    stat_lctrl_down = false;
    stat_rctrl_down = false;
    stat_lalt_down = false;
    stat_ralt_down = false;
    stat_lshift_down = false;
    stat_rshift_down = false;
}