#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>

int start(const char *host, const char *port, const struct addrinfo *hint,
	  int (*action)(int, const struct sockaddr *, socklen_t))
{
	struct addrinfo *a, *l;
	getaddrinfo(host, port, hint, &l);
	for (a = l; a; a = a->ai_next) {
		int fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
		if (fd < 0) {
			perror("socket");
			continue;
		}

		if (!action(fd, a->ai_addr, a->ai_addrlen))
			return fd;

		close(fd);
	}

	assert(0);
}

int Listen(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	const int opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(fd, addr, addrlen) < 0) {
		perror("bind");
		return -1;
	}

	return listen(fd, 10);
}

int main(void)
{
	const char *port = "1116";
	struct addrinfo hint = {
		.ai_flags = AI_PASSIVE,
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM
	};

	int l = start(NULL, port, &hint, Listen);
	hint.ai_flags = 0;
	int c = start(NULL, port, &hint, connect);
	int s = accept(l, NULL, NULL);
	if (s < 0) {
		perror("accept");
		return 1;
	}

	char buf[90000] = "hello";
	ssize_t w = write(c, buf, sizeof(buf));
	if (w < 0) {
		perror("write");
		return 1;
	}

	printf("send %ld\n", w);
	assert(w == sizeof(buf));

	while (w) {
		ssize_t r = read(s, buf, sizeof(buf));
		if (r < 0) {
			perror("read");
			return 1;
		}

		printf("read %ld: %s\n", r, buf);
		w -= r;
	}
}
