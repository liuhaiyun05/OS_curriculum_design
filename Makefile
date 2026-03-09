CC      = gcc
AS      = as
LD      = ld

CFLAGS  = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc -Wall -Wextra -Iinclude
ASFLAGS = -m32
LDFLAGS = -m elf_i386 -T linker.ld

C_SOURCES = \
	kernel/kernel.c \
	console/console.c \
	shell/shell.c

C_OBJECTS = $(C_SOURCES:.c=.o)

all: myos.iso

boot/boot.o: boot/boot.s
	$(CC) -m32 -c $< -o $@

kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

console/%.o: console/%.c
	$(CC) $(CFLAGS) -c $< -o $@

shell/%.o: shell/%.c
	$(CC) $(CFLAGS) -c $< -o $@

myos.bin: boot/boot.o $(C_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -o $@ boot/boot.o $(C_OBJECTS)

iso/boot/myos.bin: myos.bin
	cp myos.bin iso/boot/myos.bin

myos.iso: iso/boot/myos.bin iso/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso iso

run: myos.iso
	qemu-system-i386 -cdrom myos.iso

clean:
	rm -f boot/*.o kernel/*.o console/*.o shell/*.o
	rm -f myos.bin myos.iso iso/boot/myos.bin

.PHONY: all run clean