#include <sys/epoll.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>

enum { N = 10 };

struct queue {
	pthread_mutex_t mutex;
	pthread_cond_t empty, full;
	size_t put, get;
	struct epoll_event events[N];
};

struct connection {
	int cfd, efd, err;
	uint32_t events;
	char *buf;
	size_t cap, size, sent;
};

void nonblock(int fd)
{
	int fl = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

void put(struct queue *q, struct epoll_event *e)
{
	pthread_mutex_lock(&q->mutex);
	while ((q->put + 1) % N == q->get)
		pthread_cond_wait(&q->full, &q->mutex);

	q->events[q->put] = *e;
	q->put = (q->put + 1) % N;
	pthread_cond_signal(&q->empty);
	pthread_mutex_unlock(&q->mutex);
}

struct epoll_event get(struct queue *q)
{
	pthread_mutex_lock(&q->mutex);
	while (q->get == q->put)
		pthread_cond_wait(&q->empty, &q->mutex);

	struct epoll_event e = q->events[q->get];
	q->get = (q->get + 1) % N;
	pthread_cond_signal(&q->full);
	pthread_mutex_unlock(&q->mutex);
	return e;
}

void Send(struct connection *c)
{
	while (!c->err) {
		ssize_t n = write(c->cfd, c->buf + c->sent, c->size - c->sent);
		assert(n);
		if (n < 0) {
			if (errno == EAGAIN) {
				c->events = EPOLLOUT;
			} else {
				perror("write");
				c->err = 1;
			}

			return;
		}

		c->sent += n;
		if (c->sent == c->size) {
			c->sent = c->size = 0;
			c->events = EPOLLIN;
			return;
		}
	}
}

void Read(struct connection *c)
{
	while (1) {
		char buf[4096];
		ssize_t n = read(c->cfd, buf, sizeof(buf));
		if (n < 0) {
			if (errno != EAGAIN) {
				perror("read");
				c->err = 1;
			}

			return;
		}

		if (n == 0) {
			printf("%d EOF\n", c->cfd);
			c->err = 1;
			return;
		}

		size_t cap = c->size + n;
		if (cap > c->cap) {
			c->buf = realloc(c->buf, cap);
			c->cap = cap;
		}

		memcpy(c->buf + c->size, buf, n);
		c->size += n;
	}
}

void finish(struct connection *c)
{
	if (c->err) {
		printf("close %d\n", c->cfd);
		close(c->cfd);
		free(c->buf);
		free(c);
		return;
	}

	struct epoll_event e = { c->events,.data.ptr = c };
	int i = epoll_ctl(c->efd, EPOLL_CTL_ADD, c->cfd, &e);
	assert(i == 0);
}

void *work(void *q)
{
	while (1) {
		struct epoll_event e = get(q);
		if (e.events & EPOLLIN) {
			Read(e.data.ptr);
			Send(e.data.ptr);
		}

		if (e.events & EPOLLOUT)
			Send(e.data.ptr);

		finish(e.data.ptr);
	}
}

void Accept(int sfd, int efd)
{
	while (1) {
		int cfd = accept(sfd, NULL, NULL);
		if (cfd < 0) {
			assert(errno == EAGAIN);
			return;
		}

		int i = 0;
		i = setsockopt(cfd, SOL_SOCKET, SO_SNDBUF, &i, sizeof(i));
		assert(i == 0);
		nonblock(cfd);
		struct connection *c = calloc(1, sizeof(struct connection));
		c->cfd = cfd;
		c->efd = efd;
		struct epoll_event e = { EPOLLIN,.data.ptr = c };
		epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &e);
	}
}

void Poll(int sfd, struct queue *q)
{
	int efd = epoll_create1(0);
	struct epoll_event e[100] = { {EPOLLIN,.data.fd = sfd} };
	epoll_ctl(efd, EPOLL_CTL_ADD, sfd, e);

	while (1) {
		int n = epoll_wait(efd, e, sizeof(e) / sizeof(e[0]), -1);
		if (n < 0) {
			perror("epoll_wait");
			continue;
		}

		for (int i = 0; i < n; ++i) {
			uint32_t events = e[i].events;
			assert(events == EPOLLIN || events == EPOLLOUT);
			if (e[i].data.fd == sfd) {
				Accept(sfd, efd);
			} else {
				struct connection *c = e[i].data.ptr;
				epoll_ctl(efd, EPOLL_CTL_DEL, c->cfd, 0);
				put(q, &e[i]);
			}
		}
	}
}

int Listen(const char *port)
{
	struct addrinfo *l, hints = {
		.ai_flags = AI_PASSIVE,
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM
	};

	getaddrinfo(NULL, port, &hints, &l);
	for (struct addrinfo * a = l; a; a = a->ai_next) {
		int fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
		if (fd < 0) {
			perror("socket");
			continue;
		}

		int i = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
		if (bind(fd, a->ai_addr, a->ai_addrlen) == 0) {
			listen(fd, 1000);
			nonblock(fd);
			freeaddrinfo(l);
			return fd;
		}

		perror("bind");
		close(fd);
	}

	assert(0);
}

int main(void)
{
	struct sigaction s = {.sa_handler = SIG_IGN };
	sigemptyset(&s.sa_mask);
	sigaction(SIGPIPE, &s, NULL);

	struct queue q = {
		.mutex = PTHREAD_MUTEX_INITIALIZER,
		.empty = PTHREAD_COND_INITIALIZER,
		.full = PTHREAD_COND_INITIALIZER
	};

	for (int i = 0; i < 3; ++i) {
		pthread_t t;
		pthread_create(&t, NULL, work, &q);
	}

	int fd = Listen("1116");
	Poll(fd, &q);
}
