#include <stdlib.h>

struct queue {
	int *data;
	size_t size, get, put;
};

struct queue *create(size_t n)
{
	struct queue *q = malloc(sizeof(*q));
	q->data = calloc(n, sizeof(int));
	q->size = n;
	q->get = q->put = 0;
	return q;
}

void destroy(struct queue *q)
{
	free(q->data);
	free(q);
}

_Bool empty(const struct queue *q)
{
	return q->put == q->get;
}

_Bool full(const struct queue *q)
{
	return q->put == q->get + q->size;
}

int pop(struct queue *q)
{
	return q->data[q->get++ % q->size];
}

void push(struct queue *q, int i)
{
	q->data[q->put++ % q->size] = i;
}
