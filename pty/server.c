#define _XOPEN_SOURCE 600
#include "common.h"

void child(int m)
{
	if (setsid() < 0) {
		perror("setsid");
		exit(1);
	}

	unlockpt(m);
	int s = open(ptsname(m), O_RDWR);
	if (s < 0) {
		perror("open");
		exit(1);
	}

	struct winsize w = { 80, 160 };
	if (ioctl(s, TIOCSWINSZ, &w) < 0)
		perror("ioctl");

	for (int i = 0; i < 3; i++)
		dup2(s, i);

	close(m);
	close(s);
	execlp("bash", "bash", NULL);
}

void *thread(void *v)
{
	int c = (int)v;
	int m = posix_openpt(O_RDWR);
	pid_t p = fork();
	if (!p) {
		close(c);
		child(m);
	}

	printf("%d %d\n", c, p);
	loop(c, m);
	kill(p, 9);
	p = waitpid(p, NULL, 0);
	if (p < 0)
		perror("waitpid");

	printf("wait %d\n", p);
	close(c);
	close(m);
	return NULL;
}

int main(void)
{
	int s = start(bind, "0.0.0.0", 1116);
	listen(s, 10);
	while (1) {
		int c = accept(s, NULL, NULL);
		if (c < 0)
			perror("accept");

		pthread_t t;
		pthread_create(&t, NULL, thread, (void *)c);
		int e = pthread_detach(t);
		assert(!e);
	}
}
