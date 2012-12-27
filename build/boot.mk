# output: bin/boot.bin

bin/boot.bin: bin/boot.o tool/boot.ld
	@mkdir -p bin
	ld bin/boot.o -o bin/boot.bin -e c -T tool/boot.ld

bin/boot.o: src/boot/boot.S
	@mkdir -p bin
	nasm -f elf -o bin/boot.o src/boot/boot.S
