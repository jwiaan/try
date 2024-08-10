.PHONY: all clean
all: server client
server: server.c common.c common.h
	gcc -Wall -o$@ $< common.c
client: client.c common.c common.h
	gcc -Wall -o$@ $< common.c
clean:
	indent -linux *.c *.h
	rm -f *~ *.o server client
