#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

void close2(int *p)
{
	int i = close(p[0]);
	assert(!i);
	i = close(p[1]);
	assert(!i);
}

void cat(int *p)
{
	int i = dup2(p[1], STDOUT_FILENO);
	assert(i == STDOUT_FILENO);
	close2(p);
	execlp("cat", "cat", "/etc/passwd", NULL);
	assert(0);
}

void grep(int *p)
{
	int i = dup2(p[0], STDIN_FILENO);
	assert(i == STDIN_FILENO);
	close2(p);
	execlp("grep", "grep", "root", NULL);
	assert(0);
}

int main(void)
{
	int p[2];
	pipe(p);

	if (fork() == 0)
		cat(p);

	if (fork() == 0)
		grep(p);

	close2(p);
	while (wait(NULL) != -1) ;
	perror("");
}
