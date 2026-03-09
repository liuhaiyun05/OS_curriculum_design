#ifndef CONSOLE_H
#define CONSOLE_H

#include "../include/types.h"

void console_clear(void);
void console_put_char(char c);
void console_write(const char* str);
void console_write_line(const char* str);
void console_write_hex(uint32_t value);
void console_set_color(uint8_t fg, uint8_t bg);

#endif