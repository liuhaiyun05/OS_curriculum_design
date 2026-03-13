#include "mouse.h"
#include "io.h"
#include "../interrupt/interrupts.h"
#include "../include/types.h"

/* 初始把鼠标放在屏幕中间附近。 */
static MouseState state = {40, 12, 0, 0, 0, 1};
static uint8_t packet[3];
static int packet_index = 0;
static volatile int mouse_updated = 0;

static void mouse_wait_write(void) {
    /*
     * 0x64 是 PS/2 控制器状态/命令端口。
     * 其中 bit1 = 1 表示输入缓冲区满，当前还不能继续写命令。
     */
    while (inb(0x64) & 0x02) { }
}

static void mouse_wait_read(void) {
    /*
     * 0x64 的 bit0 = 1 表示输出缓冲区有数据可读，
     * 这时可以从 0x60 端口把数据取出来。
     */
    while (!(inb(0x64) & 0x01)) { }
}

static void mouse_write(uint8_t value) {
    /*
     * 要给鼠标设备本体发命令，不能直接 outb(0x60, value)。
     * 正确顺序是：
     * 1. 往 0x64 写 0xD4，告诉控制器“下一字节发给鼠标”
     * 2. 再往 0x60 写真正的命令字节
     */
    mouse_wait_write();
    outb(0x64, 0xD4);
    mouse_wait_write();
    outb(0x60, value);
}

static uint8_t mouse_read(void) {
    mouse_wait_read();
    return inb(0x60);
}

static void mouse_irq_handler(InterruptFrame* frame) {
    (void)frame;

    /* 鼠标 IRQ12 到来时，数据同样是从 0x60 读取。 */
    uint8_t data = inb(0x60);

    if (!state.enabled) {
        packet_index = 0;
        return;
    }

    /*
     * 标准 PS/2 鼠标一个完整数据包是 3 字节。
     * 第 1 字节的 bit3 固定为 1，可用于重新同步包边界。
     */
    if (packet_index == 0 && !(data & 0x08)) {
        return;
    }

    packet[packet_index++] = data;

    if (packet_index < 3) {
        return;
    }

    packet_index = 0;

    int x_move = (int8_t)packet[1];
    int y_move = (int8_t)packet[2];

    /*
     * 第 1 字节低 3 bit 表示按键状态：
     * - bit0: 左键
     * - bit1: 右键
     * - bit2: 中键
     */
    state.left = packet[0] & 0x01;
    state.right = (packet[0] >> 1) & 0x01;
    state.middle = (packet[0] >> 2) & 0x01;

    state.x += x_move;
    state.y -= y_move;

    if (state.x < 0) state.x = 0;
    if (state.x > 79) state.x = 79;
    if (state.y < 0) state.y = 0;
    if (state.y > 24) state.y = 24;

    mouse_updated = 1;
}

void mouse_init(void) {
    state.x = 40;
    state.y = 12;
    state.left = 0;
    state.right = 0;
    state.middle = 0;
    state.enabled = 1;
    packet_index = 0;
    mouse_updated = 0;

    /* 鼠标来自 IRQ12。 */
    irq_register_handler(12, mouse_irq_handler);

    /* 0xA8：打开第二个 PS/2 端口（通常就是鼠标）。 */
    mouse_wait_write();
    outb(0x64, 0xA8);

    /*
     * 0x20：读取控制器 command byte
     * 然后把 bit1 置 1，允许鼠标 IRQ12 上报中断
     */
    mouse_wait_write();
    outb(0x64, 0x20);
    mouse_wait_read();
    uint8_t status = inb(0x60);

    status |= 0x02;
    mouse_wait_write();
    outb(0x64, 0x60);
    mouse_wait_write();
    outb(0x60, status);

    /* 0xF6：恢复默认设置。鼠标通常会回一个 ACK。 */
    mouse_write(0xF6);
    mouse_read();

    /* 0xF4：开启数据上报。没有这步，鼠标一般不会开始送包。 */
    mouse_write(0xF4);
    mouse_read();
}

void mouse_set_enabled(int enabled) {
    state.enabled = enabled;
}

MouseState mouse_get_state(void) {
    return state;
}

int mouse_consume_update(void) {
    if (!mouse_updated) {
        return 0;
    }

    mouse_updated = 0;
    return 1;
}
