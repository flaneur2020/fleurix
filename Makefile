CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
AS = nasm

CFLAGS = -Wall -Werror -Isrc/inc -Isrc/inc/usr -nostdinc -fno-builtin \
		 -fno-stack-protector -finline-functions -finline-small-functions \
		 -findirect-inlining -finline-functions-called-once

all: bochs

bochs: kernel.img
	bochs -q -f .bochsrc

debug: kernel.img
	bochs-dbg -q -f .bochsrc

kernel.img: bin/boot.bin bin/main.bin
	cat bin/boot.bin bin/main.bin > bin/kernel.img

# --------------------------------------------------------------------
# the bootloader part
# => boot.bin
bin/boot.bin: bin/boot.o tool/boot.ld
	$(LD) bin/boot.o -o bin/boot.bin -T tool/boot.ld

bin/boot.o: src/boot/boot.S
	$(AS) -f elf -o bin/boot.o src/boot/boot.S

clean:
	-rm -rf bin/* root/bin/* src/kern/entry.S .bochsout

.PHONY: all bochs build debug clean
