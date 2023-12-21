#include <stdlib.h>
#include <assert.h>
#include <time.h>

void swap(int *x, int *y)
{
	int i = *x;
	*x = *y;
	*y = i;
}

void sort(int *a, int n)
{
	if (n < 2)
		return;

	int k = 0;
	for (int i = 1; i < n; i++) {
		if (a[i] < a[0])
			swap(a + i, a + ++k);
	}

	swap(a, a + k);
	sort(a, k);
	sort(a + k + 1, n - k - 1);
}

int main(void)
{
	int n = 100, a[n];
	srand(time(NULL));
	for (int i = 0; i < n; i++)
		a[i] = rand();

	sort(a, n);
	for (int i = 1; i < n; i++)
		assert(a[i - 1] <= a[i]);
}
