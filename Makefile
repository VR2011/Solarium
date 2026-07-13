CC=gcc
LD=ld
AS=nasm

CFLAGS=-m32 -ffreestanding -O2 -Wall -Wextra -Wpedantic -c
LDFLAGS=-m elf_i386 -T linker.ld -nostdlib

all: iso

logo_fb.h: Images/logo.png Tools/img2fb.py
	python3 Tools/img2fb.py Images/logo.png logo_fb.h

boot.o:
	$(AS) -f elf32 boot.asm -o boot.o

kernel.o: logo_fb.h
	$(CC) $(CFLAGS) kernel.c -o kernel.o

terminal.o:
	$(CC) $(CFLAGS) terminal.c -o terminal.o

string.o:
	$(CC) $(CFLAGS) string.c -o string.o

keyboard.o:
	gcc -m32 -ffreestanding -Wall -Wextra -Wpedantic -c keyboard.c -o keyboard.o

delay.o:
	$(CC) $(CFLAGS) delay.c -o delay.o

vgagraphics.o:
	$(CC) $(CFLAGS) vgagraphics.c -o vgagraphics.o

framebuffer.o:
	$(CC) $(CFLAGS) framebuffer.c -o framebuffer.o

kernel.bin: boot.o kernel.o terminal.o keyboard.o shell.o string.o delay.o vgagraphics.o framebuffer.o
	$(LD) $(LDFLAGS) boot.o kernel.o terminal.o keyboard.o shell.o string.o delay.o vgagraphics.o framebuffer.o -o kernel.bin

iso: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/
	cp grub.cfg iso/boot/grub/
	grub-mkrescue -o solarium.iso iso

run: iso
	qemu-system-i386 -cdrom solarium.iso

clean:
	rm -rf *.o *.bin *.iso iso/boot/kernel.bin logo_fb.h 