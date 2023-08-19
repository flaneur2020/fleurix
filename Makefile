CC = i386-elf-gcc
LD = i386-elf-ld
OBJCOPY = i386-elf-objcopy
AS = nasm

CFLAGS = -Wall -Isrc/inc -Isrc/inc/usr -nostdinc -fno-builtin \
		 -fno-stack-protector -finline-functions -finline-small-functions \
		 -findirect-inlining -finline-functions-called-once

all: bochs

bochs: kernel.img
	bochs -q -f .bochsrc

debug: kernel.img
	bochs-dbg -q -f .bochsrc

kernel.img: boot.bin kernel.bin
	cat boot.bin kernel.bin > kernel.img

# --------------------------------------------------------------------
# bootloader
boot.bin: src/boot/boot.o
	$(LD) $< -o $@ -T tool/boot.ld

# --------------------------------------------------------------------
# kernel
HEADERS  = $(wildcard src/**/*.h)
SRCS = $(wildcard src/**/*.c)
OBJS = src/kern/entry.o $(SRCS:.c=.o)

kernel.bin: kernel.elf
	$(OBJCOPY) -R .pdr -R .comment -R .note -S -O binary $< $@

kernel.elf: $(OBJS)
	$(LD) $^ -o $@ -T tool/main.ld

%.o: %.S
	$(AS) -f elf -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f src/boot/boot.o src/kern/entry.o $(OBJS)
	rm -f kernel.img boot.bin kernel.bin kernel.elf

.PHONY: all bochs build debug clean
