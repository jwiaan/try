struct tree {
	int key, red;
	struct tree *left, *right;
};

const struct tree *find(const struct tree *t, int k)
{
	if (t && t->key != k) {
		if (k < t->key)
			t = find(t->left, k);
		else
			t = find(t->right, k);
	}

	return t;
}

struct tree *rotate_left(struct tree *t)
{
	struct tree *r = t->right;
	t->right = r->left;
	r->left = t;
	return r;
}

struct tree *rotate_right(struct tree *t)
{
	struct tree *l = t->left;
	t->left = l->right;
	l->right = t;
	return l;
}
