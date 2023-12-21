#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "tree.h"

int height(const struct tree *t)
{
	if (!t)
		return 1;

	int h = height(t->left);
	assert(h == height(t->right));
	return t->red ? h : h + 1;
}

int red(const struct tree *t)
{
	return t && t->red;
}

struct tree *check_left(struct tree *t)
{
	if (red(t->left) && (red(t->left->left) || red(t->left->right))) {
		if (red(t->right)) {
			t->red = 1;
			t->left->red = t->right->red = 0;
		} else {
			if (red(t->left->right))
				t->left = rotate_left(t->left);

			t = rotate_right(t);
			t->red = 0;
			t->right->red = 1;
		}
	}

	return t;
}

struct tree *check_right(struct tree *t)
{
	if (red(t->right) && (red(t->right->left) || red(t->right->right))) {
		if (red(t->left)) {
			t->red = 1;
			t->left->red = t->right->red = 0;
		} else {
			if (red(t->right->left))
				t->right = rotate_right(t->right);

			t = rotate_left(t);
			t->red = 0;
			t->left->red = 1;
		}
	}

	return t;
}

struct tree *insert(struct tree *t, int k)
{
	if (!t) {
		t = calloc(1, sizeof(*t));
		t->key = k;
		t->red = 1;
	} else if (k < t->key) {
		t->left = insert(t->left, k);
		t = check_left(t);
	} else if (k > t->key) {
		t->right = insert(t->right, k);
		t = check_right(t);
	}

	return t;
}

struct tree *Insert(struct tree *t, int k)
{
	printf("insert %d\n", k);
	t = insert(t, k);
	t->red = 0;
	return t;
}

void test(const struct tree *t, int *v, int *n, int d)
{
	if (t) {
		test(t->left, v, n, d + 1);
		for (int i = 0; i < d; ++i)
			printf("\t");

		if (t->red) {
			printf("\e[1;31m%d-%d\e[m\n", t->key, height(t));
			assert(!red(t->left) && !red(t->right));
		} else {
			printf("%d-%d\n", t->key, height(t));
		}

		v[(*n)++] = t->key;
		test(t->right, v, n, d + 1);
	}
}

void Test(const struct tree *t)
{
	int v[N], n = 0;
	test(t, v, &n, 0);
	for (int i = 0; i < n - 1; ++i)
		assert(v[i] < v[i + 1]);

	printf("size %d\n", n);
}

int main(void)
{
	struct tree *t = NULL;
	srand(time(NULL));
	for (int i = 0; i < N; ++i) {
		t = Insert(t, rand() % N);
		Test(t);
	}
}
