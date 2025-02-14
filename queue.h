#pragma once

#include <stddef.h>

struct queue *create(size_t);
void destroy(struct queue *);
_Bool empty(const struct queue *);
_Bool full(const struct queue *);
int pop(struct queue *);
void push(struct queue *, int);
