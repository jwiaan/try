#include "common.h"

static int io(int i, int o)
{
	char b[4096];
	ssize_t r = read(i, b, sizeof(b));
	if (r < 0) {
		perror("read");
		return -1;
	}

	if (r == 0) {
		printf("%d eof\n", i);
		return -1;
	}

	ssize_t w = write(o, b, r);
	if (w < 0) {
		perror("write");
		return -1;
	}

	assert(w == r);
	return 0;
}

void loop(int x, int y)
{
	struct pollfd p[2] = { {x, POLLIN}, {y, POLLIN} };
	while (poll(p, 2, -1) > 0) {
		for (int i = 0; i < 2; i++) {
			if (p[i].revents & POLLIN) {
				if (io(p[i].fd, p[(i + 1) % 2].fd))
					return;
			}
		}
	}

	perror("poll");
}

int start(int (*action)(int, const struct sockaddr *, socklen_t),
	  const char *host, uint16_t port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int val = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	int i = inet_aton(host, &addr.sin_addr);
	assert(i);

	if (action(fd, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("");
		return -1;
	}

	return fd;
}
