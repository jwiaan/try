#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

void Close(int *p)
{
	int i = close(p[0]);
	assert(!i);
	i = close(p[1]);
	assert(!i);
}

void cat(int *p)
{
	int fd = dup2(p[1], STDOUT_FILENO);
	assert(fd == STDOUT_FILENO);
	Close(p);
	execlp("cat", "cat", "/etc/passwd", NULL);
	assert(0);
}

void grep(int *p)
{
	int fd = dup2(p[0], STDIN_FILENO);
	assert(fd == STDIN_FILENO);
	Close(p);
	execlp("grep", "grep", "root", NULL);
	assert(0);
}

int main(void)
{
	int p[2];
	pipe(p);

	void (*f[])(int *) = { cat, grep };
	for (int i = 0; i < sizeof(f) / sizeof(*f); i++) {
		if (fork() == 0)
			f[i] (p);
	}

	Close(p);
	while (wait(NULL) != -1) ;
	perror("");
}
