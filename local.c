#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

struct sockaddr_un address = { AF_LOCAL, "@server" };

pthread_barrier_t barrier;

void io(int from, int to)
{
	char buf[4096];
	ssize_t n;
	while ((n = read(from, buf, sizeof(buf))) > 0)
		write(to, buf, n);
}

void *server(void *p)
{
	int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind");
		return NULL;
	}

	listen(fd, 10);
	pthread_barrier_wait(&barrier);
	fd = accept(fd, NULL, NULL);
	io(fd, STDOUT_FILENO);
	return NULL;
}

void client(void)
{
	int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	pthread_barrier_wait(&barrier);
	if (connect(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("connect");
		return;
	}

	io(STDIN_FILENO, fd);
}

int main(void)
{
	address.sun_path[0] = 0;
	pthread_barrier_init(&barrier, NULL, 2);
	pthread_t t;
	pthread_create(&t, NULL, server, NULL);
	client();
}
