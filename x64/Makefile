.PHONY: test clean
test: disk
	qemu-system-x86_64 disk
disk: boot core
	cp $< $@
	dd if=core of=$@ oflag=append conv=notrunc,sync
boot: boot.s
	nasm $<
core: core.ld main.o time.o
	ld -static -T$< main.o time.o -o$@
main.o: main.c
	indent -linux $<
	cc -c -nostdlib -Wall $<
time.o: time.s
	nasm -felf64 $<
clean:
	rm *~ *.o boot core disk
