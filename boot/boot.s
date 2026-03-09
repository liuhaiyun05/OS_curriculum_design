.section .multiboot
.align 4
.long 0x1BADB002              # magic
.long 0x00000003              # flags: align | meminfo
.long -(0x1BADB002 + 0x00000003)

.section .text
.global _start
.extern kernel_main

_start:
    mov $stack_top, %esp
    call kernel_main

1:
    cli
    hlt
    jmp 1b

.section .bss
.align 16
stack_bottom:
    .skip 16384
stack_top: