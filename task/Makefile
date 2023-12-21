boot: boot.s start
	nasm $<
	dd if=start of=$@ oflag=append conv=notrunc,sync

start: start.o asm.o
	ld -melf_i386 -Ttext 0x80009000 -o$@ $^

start.o: start.c
	cc -c -m32 -nostdlib $<

asm.o: asm.s
	nasm -felf $<

.PHONY: clean
clean:
	rm -f *.o boot start
