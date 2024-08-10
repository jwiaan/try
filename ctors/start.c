void init(void);
int main(void);

struct {
	void (*f)(void *);
	void *p;
} d[2];

int i, __dso_handle;

void __cxa_atexit(void (*f)(void *), void *p)
{
	d[i].f = f;
	d[i].p = p;
	++i;
}

void exit(int n)
{
	while (--i > -1)
		d[i].f(d[i].p);

	asm("syscall"::"a"(60), "D"(n));
}

void _start(void)
{
	init();
	exit(main());
}
