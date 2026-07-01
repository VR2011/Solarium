CC=gcc
LD=ld
AS=nasm

CFLAGS=-m32 -ffreestanding -Wall -Wextra -Wpedantic -c
LDFLAGS=-m elf_i386 -T linker.ld -nostdlib

all: iso

boot.o:
	$(AS) -f elf32 boot.asm -o boot.o

kernel.o:
	$(CC) $(CFLAGS) kernel.c -o kernel.o

terminal.o:
	$(CC) $(CFLAGS) terminal.c -o terminal.o

string.o:
	$(CC) $(CFLAGS) string.c -o string.o

keyboard.o:
	gcc -m32 -ffreestanding -Wall -Wextra -Wpedantic -c keyboard.c -o keyboard.o

kernel.bin: boot.o kernel.o terminal.o keyboard.o shell.o string.o
	$(LD) $(LDFLAGS) boot.o kernel.o terminal.o keyboard.o shell.o string.o -o kernel.bin

iso: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/
	cp grub.cfg iso/boot/grub/
	grub-mkrescue -o kernel.iso iso

run: iso
	qemu-system-i386 -cdrom kernel.iso

clean:
	rm -rf *.o *.bin *.iso iso/boot/kernel.bin