#include "console.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t*)0xB8000)

static size_t cursor_row = 0;
static size_t cursor_col = 0;
static uint8_t color = 0x07;   // light grey on black

static uint16_t vga_entry(unsigned char ch, uint8_t color_code) {
    return (uint16_t)ch | ((uint16_t)color_code << 8);
}

void console_set_color(uint8_t fg, uint8_t bg) {
    color = fg | (bg << 4);
}

static void console_scroll_if_needed(void) {
    if (cursor_row < VGA_HEIGHT) {
        return;
    }

    for (size_t row = 1; row < VGA_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            VGA_MEMORY[(row - 1) * VGA_WIDTH + col] =
                VGA_MEMORY[row * VGA_WIDTH + col];
        }
    }

    for (size_t col = 0; col < VGA_WIDTH; col++) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = vga_entry(' ', color);
    }

    cursor_row = VGA_HEIGHT - 1;
}

void console_clear(void) {
    for (size_t row = 0; row < VGA_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            VGA_MEMORY[row * VGA_WIDTH + col] = vga_entry(' ', color);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}

void console_put_char(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
        console_scroll_if_needed();
        return;
    }

    VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] = vga_entry(c, color);
    cursor_col++;

    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
        console_scroll_if_needed();
    }
}

void console_write(const char* str) {
    while (*str) {
        console_put_char(*str++);
    }
}

void console_write_line(const char* str) {
    console_write(str);
    console_put_char('\n');
}

void console_write_hex(uint32_t value) {
    const char* hex = "0123456789ABCDEF";
    console_write("0x");
    for (int i = 28; i >= 0; i -= 4) {
        console_put_char(hex[(value >> i) & 0xF]);
    }
}