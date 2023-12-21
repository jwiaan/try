void *c[1] __attribute__((section(".ctors")));

void init(void)
{
	void (*f)(void) = c[1];
	f();
}
