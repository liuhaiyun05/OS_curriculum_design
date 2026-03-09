#include "../console/console.h"
#include "../shell/shell.h"

void kernel_main(void) {
    console_set_color(0x0F, 0x00);   // white on black
    console_clear();

    console_write_line("========================================");
    console_write_line("        Welcome to MyOS Kernel");
    console_write_line("========================================");
    console_write_line("");
    console_write_line("[OK] Booted by GRUB");
    console_write_line("[OK] Entered 32-bit kernel_main()");
    console_write_line("[OK] Console initialized");
    console_write_line("");

    shell_demo();

    while (1) {
        __asm__ __volatile__("hlt");
    }
}