#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <stdio.h>

enum { N = 100 };

int Connect(const char *host, const char *port)
{
	struct addrinfo *l, hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM
	};

	getaddrinfo(host, port, &hints, &l);
	for (struct addrinfo * a = l; a; a = a->ai_next) {
		int fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
		if (fd < 0) {
			perror("socket");
			continue;
		}

		if (connect(fd, a->ai_addr, a->ai_addrlen) == 0) {
			freeaddrinfo(l);
			return fd;
		}

		perror("connect");
		close(fd);
	}

	assert(0);
}

void Send(int fd, const char *buf, size_t len)
{
	while (len) {
		ssize_t n = write(fd, buf, len);
		if (n < 0)
			perror("write");

		if (n < len)
			printf("%ld < %lu\n", n, len);

		assert(n > 0);
		buf += n;
		len -= n;
	}
}

void *thread(void *p)
{
	int fd = Connect("127.0.0.1", "1116");
	char buf[1000];
	size_t len = 0;

	for (int i = 0; i < 1000; ++i) {
		Send(fd, buf, sizeof(buf));
		len += sizeof(buf);
	}

	while (len) {
		ssize_t n = read(fd, buf, sizeof(buf));
		assert(n > 0);
		len -= n;
	}

	return NULL;
}

int main(void)
{
	pthread_t t[N];
	int i, e;

	for (i = 0; i < N; ++i) {
		e = pthread_create(&t[i], NULL, thread, NULL);
		assert(!e);
	}

	while (i--) {
		e = pthread_join(t[i], NULL);
		assert(!e);
	}
}
