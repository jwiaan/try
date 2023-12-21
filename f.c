#include <stdio.h>

int f1(int i)
{
	int x = 1, y = 1, z = 1;
	while (--i > 0) {
		z = x + y;
		x = y;
		y = z;
	}
	return z;
}

int f2(int i)
{
	if (i < 2)
		return 1;

	return f2(i - 1) + f2(i - 2);
}

void run(int (*f)(int), int n)
{
	for (int i = 0; i < n; ++i) {
		printf("%d%c", f(i), i == n - 1 ? '\n' : ' ');
		fflush(stdout);
	}
}

int main(void)
{
	int n = 40;
	run(f1, n);
	run(f2, n);
}
