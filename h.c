#include <stdio.h>
#include <stdlib.h>

void move(char s, char d)
{
	printf("%c -> %c\n", s, d);
}

void h(int n, char s, char d, char t)
{
	if (n == 1) {
		move(s, d);
	} else {
		h(n - 1, s, t, d);
		move(s, d);
		h(n - 1, t, d, s);
	}
}

int main(int c, char **v)
{
	int n = atoi(v[1]);
	printf("%d\n", n);
	h(n, 'S', 'D', 't');
}
