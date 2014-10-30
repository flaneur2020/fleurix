# output: kern.bin

H_FILES = $(wildcard src/**/*.h)
C_FILES = $(wildcard src/**/*.c)
S_FILES = src/kern/entry.S
O_FILES = $(addprefix bin/, $(basename $(C_FILES:.c=.o) $(S_FILES:.S=.o)))

bin/%.o: $(wildcard src/**/%.c) $(H_FILES)
	gcc $(CFLAG) $(CINC) -o $@ -c $<

bin/entry.o: src/kern/entry.S
	nasm -f elf -o $@ $<

bin/kern.bin: bin/kern.elf
	objcopy -R .pdr -R .comment -R .note -S -O binary $< $@

bin/kern.elf: $(O_FILES) tool/main.ld
	ld $< -o $@ -e -c -T tool/main.ld
