void *__dso_handle;
int i;
struct {
	void (*f)(void *);
	void *p;
} d[3];

void __cxa_atexit(void f(void *), void *p)
{
	d[i].f = f;
	d[i].p = p;
	++i;
}

void exit2(void)
{
	asm("syscall"::"a"(60), "D"(2));
}

void exit(int n)
{
	while (--i > -1)
		d[i].f(d[i].p);

	exit2();
}

void _start(void)
{
	init();
	exit(main());
}
