#include "keyboard.h"
#include "io.h"

static int shift_pressed = 0;

static const char normal_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0
};

static const char shift_map[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0
};

void keyboard_init(void) {
    shift_pressed = 0;
}

int keyboard_read_char(char* out) {
    uint8_t status = inb(0x64);
    if (!(status & 0x01)) return 0;

    uint8_t scancode = inb(0x60);

    if (status & 0x20) return 0;

    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return 0;
    }

    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return 0;
    }

    if (scancode & 0x80) return 0;

    char c = shift_pressed ? shift_map[scancode] : normal_map[scancode];
    if (!c) return 0;

    *out = c;
    return 1;
}