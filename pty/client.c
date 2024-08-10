#include "common.h"

int main(void)
{
	int c = start(connect, "127.0.0.1", 1116);
	loop(c, 0);
}
