#include <sys/socket.h>
#include <sys/un.h>
#include <semaphore.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum msg { REQ, RSP, BUSY, IDLE };

_Thread_local int id;
pthread_barrier_t barrier;
sem_t sem;
int idle[3];

const char *msg2str(enum msg msg)
{
	switch (msg) {
	case REQ:
		return "req";
	case RSP:
		return "rsp";
	case BUSY:
		return "BUSY";
	case IDLE:
		return "IDLE";
	default:
		assert(0);
	}
}

struct sockaddr_un id2addr(int id)
{
	struct sockaddr_un s = { AF_LOCAL };
	sprintf(s.sun_path + 1, "%d", id);
	return s;
}

int Bind(void)
{
	int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	struct sockaddr_un s = id2addr(id);
	int i = bind(fd, &s, sizeof(s));
	assert(i == 0);
	return fd;
}

void Send(int fd, int msg, int to)
{
	struct sockaddr_un s = id2addr(to);
	ssize_t n = sendto(fd, &msg, sizeof(msg), 0, &s, sizeof(s));
	assert(n == sizeof(msg));
}

void Read(int fd, int *msg, int *from)
{
	struct sockaddr_un s;
	socklen_t l = sizeof(s);
	ssize_t n = recvfrom(fd, msg, sizeof(int), 0, &s, &l);
	assert(n == sizeof(int));
	*from = atoi(s.sun_path + 1);
	char *c = (*msg == RSP ? "-" : "");
	printf("%d <-%s %d: %s\n", id, c, *from, msg2str(*msg));
}

int Poll(int fd)
{
	struct pollfd p = {
		.fd = fd,
		.events = POLLIN
	};

	int n = poll(&p, 1, 0);
	assert(n != -1);
	return n;
}

void start(int fd)
{
	for (int to = 0; to < sizeof(idle) / sizeof(idle[0]); ++to) {
		if (to != id) {
			int i = sem_post(&sem);
			assert(i == 0);
			Send(fd, REQ, to);
		}
	}
}

void *thread(void *p)
{
	id = (int)p;
	int fd = Bind();
	pthread_barrier_wait(&barrier);
	start(fd);

	while (1) {
		int n = Poll(fd);
		if (n == 0) {
			printf("note over %d: IDLE\n", id);
			Send(fd, IDLE, 0);
		}

		int msg, from;
		Read(fd, &msg, &from);
		assert(msg == REQ || msg == RSP);
		if (n == 0) {
			printf("note over %d: BUSY\n", id);
			Send(fd, BUSY, 0);
		}

		if (msg == REQ) {
			Send(fd, RSP, from);
		} else {
			int i = sem_trywait(&sem);
			assert(i == 0);
		}
	}
}

void check(int msg, int from)
{
	assert(msg == BUSY || msg == IDLE);
	idle[from] = (msg == IDLE);

	char s[256];
	int n = sprintf(s, "note over 0: ");
	int busy = 0;
	for (int i = 1; i < sizeof(idle) / sizeof(idle[0]); ++i) {
		if (!idle[i])
			busy = 1;

		n += sprintf(s + n, "%d %s\\n", i, idle[i] ? "IDLE" : "BUSY");
	}

	if (busy) {
		s[n - 2] = 0;
		printf("%s\n", s);
		return;
	}

	int i = sem_getvalue(&sem, &n);
	assert(i == 0);
	if (n)
		printf("note over 0: STILL BUSY\\nsem=%d\n", n);
	else
		printf("note over 0: ALL IDLE\\nsem=%d\n", n);
}

void Main(void)
{
	int fd = Bind();
	pthread_barrier_wait(&barrier);
	start(fd);

	while (1) {
		int i, msg, from;
		Read(fd, &msg, &from);
		switch (msg) {
		case REQ:
			Send(fd, RSP, from);
			break;
		case RSP:
			i = sem_trywait(&sem);
			assert(i == 0);
			break;
		case BUSY:
		case IDLE:
			check(msg, from);
			break;
		default:
			assert(0);
		}
	}
}

int main(void)
{
	setbuf(stdout, NULL);
	sem_init(&sem, 0, 0);
	pthread_barrier_init(&barrier, NULL, sizeof(idle) / sizeof(idle[0]));

	for (int i = 1; i < sizeof(idle) / sizeof(idle[0]); ++i) {
		pthread_t t;
		pthread_create(&t, NULL, thread, (void *)i);
	}

	Main();
}
