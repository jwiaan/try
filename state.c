#include <stdio.h>
#include <string.h>

struct handler {
	const char *event;
	void (*handle)(const char *);
};

struct changer {
	const char *state;
	const char *event;
	void (*change)(const char *, const char **);
};

struct machine {
	const char *state;
	struct changer *changer;
	struct handler *handler;
	size_t c, h;
};

struct handler *find_handler(const struct machine *m, const char *e)
{
	for (size_t i = 0; i < m->h; i++) {
		if (!strcmp(m->handler[i].event, e))
			return &m->handler[i];
	}
	return NULL;
}

struct changer *find_changer(const struct machine *m, const char *e)
{
	for (size_t i = 0; i < m->c; i++) {
		if (!strcmp(m->changer[i].event, e)
		    && !strcmp(m->changer[i].state, m->state))
			return &m->changer[i];
	}
	return NULL;
}

void handle(struct machine *m, const char *e)
{
	struct changer *c = find_changer(m, e);
	if (c) {
		c->change(e, &m->state);
	} else {
		struct handler *h = find_handler(m, e);
		if (h)
			h->handle(e);
	}
}

void print(const char *e)
{
	printf("%s\n", e);
}

void open_success(const char *e, const char **s)
{
	printf("%s: %s -> ", e, *s);
	*s = "open";
	printf("%s\n", *s);
}

void close_failure(const char *e, const char **s)
{
	printf("%s: %s -> ", e, *s);
	*s = "error";
	printf("%s\n", *s);
}

void fix_success(const char *e, const char **s)
{
	printf("%s: %s -> ", e, *s);
	*s = "close";
	printf("%s\n", *s);
}

int main(void)
{
	struct handler h[] =
	    { {"open", print}, {"close", print}, {"fix", print} };
	struct changer c[] =
	    { {"close", "open", open_success}, {"open", "close", close_failure},
	{"error", "fix", fix_success}
	};

	struct machine m = { "close" };
	m.handler = h;
	m.changer = c;
	m.h = sizeof(h) / sizeof(*h);
	m.c = sizeof(c) / sizeof(*c);
	handle(&m, "open");
	handle(&m, "open");
	handle(&m, "close");
	handle(&m, "close");
	handle(&m, "fix");
	handle(&m, "fix");
	handle(&m, "open");
}
