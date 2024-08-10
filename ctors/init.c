void (*f[1])(void) __attribute__((section(".ctors")));

void init(void)
{
	f[1] ();
}
