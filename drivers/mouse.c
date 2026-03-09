#include "mouse.h"
#include "io.h"
#include "../include/types.h"

static MouseState state = {40, 12, 0, 0, 0, 1};
static uint8_t packet[3];
static int packet_index = 0;

static void mouse_wait_write(void) {
    while (inb(0x64) & 0x02) { }
}

static void mouse_wait_read(void) {
    while (!(inb(0x64) & 0x01)) { }
}

static void mouse_write(uint8_t value) {
    mouse_wait_write();
    outb(0x64, 0xD4);
    mouse_wait_write();
    outb(0x60, value);
}

static uint8_t mouse_read(void) {
    mouse_wait_read();
    return inb(0x60);
}

void mouse_init(void) {
    state.x = 40;
    state.y = 12;
    state.left = 0;
    state.right = 0;
    state.middle = 0;
    state.enabled = 1;
    packet_index = 0;

    mouse_wait_write();
    outb(0x64, 0xA8);

    mouse_wait_write();
    outb(0x64, 0x20);
    mouse_wait_read();
    uint8_t status = inb(0x60);

    status |= 0x02;
    mouse_wait_write();
    outb(0x64, 0x60);
    mouse_wait_write();
    outb(0x60, status);

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();
}

void mouse_set_enabled(int enabled) {
    state.enabled = enabled;
}

MouseState mouse_get_state(void) {
    return state;
}

void mouse_poll(void) {
    if (!state.enabled) return;

    while (1) {
        uint8_t status = inb(0x64);
        if (!(status & 0x01)) return;
        if (!(status & 0x20)) return;

        packet[packet_index++] = inb(0x60);

        if (packet_index < 3) return;
        packet_index = 0;

        int x_move = (int8_t)packet[1];
        int y_move = (int8_t)packet[2];

        state.left   = packet[0] & 0x01;
        state.right  = (packet[0] >> 1) & 0x01;
        state.middle = (packet[0] >> 2) & 0x01;

        state.x += x_move;
        state.y -= y_move;

        if (state.x < 0) state.x = 0;
        if (state.x > 79) state.x = 79;
        if (state.y < 0) state.y = 0;
        if (state.y > 24) state.y = 24;
    }
}