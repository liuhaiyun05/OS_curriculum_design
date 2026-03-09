#include "../console/console.h"
#include "shell.h"

void shell_demo(void) {
    console_write_line("MyOS> help");
    console_write_line("Supported commands (demo stage):");
    console_write_line("  help");
    console_write_line("  clear");
    console_write_line("  version");
    console_write_line("");
    console_write_line("MyOS> version");
    console_write_line("MyOS version 0.1");
    console_write_line("");
    console_write_line("MyOS> ");
}