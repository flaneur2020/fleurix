CINC  := -Isrc/inc -Isrc/inc/usr
CFLAG := -Wall -Werror -nostdinc -fno-builtin -fno-stack-protector \
		 -finline-functions -finline-small-functions -findirect-inlining \
		 -finline-functions-called-once

include build/boot.mk
include build/kern.mk
include build/rootfs.mk
include build/user.mk

all: bochs

bochs: build
	bochs -q -f .bochsrc

debug: build
	bochs-dbg -q -f .bochsrc

build:
	echo

clean:
	-rm -rf bin/* root/bin/* src/kern/entry.S .bochsout

.PHONY: all bochs build debug clean
