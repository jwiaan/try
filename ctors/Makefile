a.out: start.o init.o main.o
	g++ -nostdlib $^

start.o: start.c
	gcc -c $<

init.o: init.c
	gcc -c $<

main.o: main.cc
	g++ -c $<

.PHONY: clean
clean:
	rm -f *~ *.o a.out
