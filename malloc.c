#include <unistd.h>
#include <stdio.h>

struct head {
	size_t size;
	struct head *next;
} head = {.next = &head }, *list = &head;

enum { N = sizeof(struct head) };

void put(void *p)
{
	struct head *prev, *curr = p;
	--curr;

	for (prev = list; !(prev < curr && curr < prev->next);
	     prev = prev->next) {
		if (prev->next <= prev) {
			if (curr < prev->next || prev < curr)
				break;
		}
	}

	if (curr + curr->size == prev->next) {
		curr->size += prev->next->size;
		curr->next = prev->next->next;
	} else {
		curr->next = prev->next;
	}

	if (prev + prev->size == curr) {
		prev->size += curr->size;
		prev->next = curr->next;
	} else {
		prev->next = curr;
	}

	list = prev;
}

struct head *more(size_t n)
{
	if (n < 1024)
		n = 1024;

	struct head *p = sbrk(n * N);
	p->size = n;
	put(++p);
	return list;
}

void *get(size_t n)
{
	struct head *curr, *prev = list;
	n = (n + N - 1) / N + 1;

	while (1) {
		curr = prev->next;
		if (curr->size >= n) {
			if (curr->size == n) {
				prev->next = curr->next;
			} else {
				curr->size -= n;
				curr += curr->size;
				curr->size = n;
			}

			list = prev;
			return ++curr;
		}

		if (curr == list)
			curr = more(n);

		prev = curr;
	}
}

void print(void)
{
	struct head *p = list;
	do {
		printf("%p %lu %p\n", p, p->size, p->next);
		p = p->next;
	} while (p != list);
	printf("\n");
}

int main(void)
{
	print();
	void *p1 = get(1);
	print();
	void *p2 = get(2);
	print();
	put(p1);
	print();
	put(p2);
	print();
}
