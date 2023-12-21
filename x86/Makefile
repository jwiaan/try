.PHONY: run
run: image
	qemu-system-i386 image
image: boot start
	cp boot $@
	dd if=start of=$@ oflag=append conv=notrunc,sync
boot: boot.s
	nasm $<
start: start.o
	ld -melf_i386 -Ttext 0xa000 -o$@ $<
start.o: start.c
	indent -linux start.c
	cc -c -m32 -nostdlib -fno-pie -Wall $<
