#include <arpa/inet.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <sys/poll.h>
#include <unistd.h>
#include <poll.h>

int start(int (*action)(int, const struct sockaddr *, socklen_t),
	  const char *host, uint16_t port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int val = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(host, &addr.sin_addr);

	if (action(fd, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("");
		return -1;
	}

	return fd;
}

void print(uint32_t e)
{
	printf("%u:", e);
	if (e & EPOLLIN)
		printf("EPOLLIN ");

	if (e & EPOLLOUT)
		printf("EPOLLOUT");

	printf("\n");
}

void *server(void *p)
{
	int fd = start(bind, "0.0.0.0", 1116);
	listen(fd, 10);
	fd = accept(fd, NULL, NULL);

	int ep = epoll_create1(0);
	struct epoll_event e = {.events = EPOLLIN | EPOLLOUT | EPOLLET };
	epoll_ctl(ep, EPOLL_CTL_ADD, fd, &e);

	int first = 1;
	while (1) {
		printf("epoll_wait\n");
		int n = epoll_wait(ep, &e, 1, -1);
		assert(n == 1);
		print(e.events);
		if (first) {
			first = 0;
			write(fd, "hello\n", 6);
			continue;
		}

		if (e.events & EPOLLIN) {
			char b[256];
			ssize_t s = read(fd, &b, sizeof(b));
			write(fd, b, s);
		}
	}
}

void echo(int i, int o)
{
	char b[256];
	ssize_t n = read(i, b, sizeof(b));
	if (n > 0)
		write(o, b, n);
}

int main(void)
{
	pthread_t t;
	pthread_create(&t, NULL, server, NULL);
	sleep(1);
	int fd = start(connect, "127.0.0.1", 1116);

	struct pollfd fds[2] = { {0, POLLIN}, {fd, POLLIN} };
	while (1) {
		int n = poll(fds, 2, -1);
		for (int i = 0; i < n; i++) {
			if (fds[0].revents & POLLIN)
				echo(0, fd);

			if (fds[1].revents & POLLIN)
				echo(fd, 1);
		}
	}
}
