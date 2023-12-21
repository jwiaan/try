#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "tree.h"

int height(const struct tree *t)
{
	if (!t)
		return -1;

	int l = height(t->left), r = height(t->right);
	return (l > r ? l : r) + 1;
}

struct tree *check_left(struct tree *t)
{
	if (height(t->left) - height(t->right) > 1) {
		if (height(t->left->right) > height(t->left->left))
			t->left = rotate_left(t->left);

		t = rotate_right(t);
	}

	return t;
}

struct tree *check_right(struct tree *t)
{
	if (height(t->right) - height(t->left) > 1) {
		if (height(t->right->left) > height(t->right->right))
			t->right = rotate_right(t->right);

		t = rotate_left(t);
	}

	return t;
}

struct tree *insert(struct tree *t, int k)
{
	if (!t) {
		t = calloc(1, sizeof(*t));
		t->key = k;
	} else if (k < t->key) {
		t->left = insert(t->left, k);
		t = check_left(t);
	} else if (k > t->key) {
		t->right = insert(t->right, k);
		t = check_right(t);
	}

	return t;
}

struct tree *erase(struct tree *t, int k)
{
	if (t) {
		if (k < t->key) {
			t->left = erase(t->left, k);
			t = check_right(t);
		} else if (k > t->key) {
			t->right = erase(t->right, k);
			t = check_left(t);
		} else {
			struct tree *p;
			if (t->left && t->right) {
				p = t->left;
				while (p->right)
					p = p->right;

				t->key = p->key;
				t->left = erase(t->left, p->key);
				t = check_right(t);
			} else {
				p = t;
				if (!t->left)
					t = t->right;
				else
					t = t->left;

				free(p);
			}
		}
	}

	return t;
}

void print(const struct tree *t, int d)
{
	if (t) {
		print(t->left, d + 1);
		for (int i = 0; i < d; ++i)
			printf("\t");

		printf("%d-%d\n", t->key, height(t));
		print(t->right, d + 1);
	}
}

void test(const struct tree *t, int *v, int *n)
{
	if (t) {
		test(t->left, v, n);
		v[(*n)++] = t->key;
		int i = height(t->left) - height(t->right);
		assert(-2 < i && i < 2);
		test(t->right, v, n);
	}
}

void Test(const struct tree *t)
{
	int v[N], n = 0;
	test(t, v, &n);
	for (int i = 0; i < n - 1; ++i)
		assert(v[i] < v[i + 1]);
}

int main(void)
{
	struct tree *t = NULL;
	srand(time(NULL));
	for (int i = 0; i < N; ++i)
		t = insert(t, rand() % N);

	print(t, 0);
	Test(t);

	while (t) {
		int k = rand() % N;
		if (find(t, k)) {
			t = erase(t, k);
			Test(t);
		}
	}
}
