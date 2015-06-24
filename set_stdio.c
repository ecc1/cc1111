#include "set_stdio.h"

static get_t getc;
static put_t putc;

void set_stdio(get_t g, put_t p)
{
	getc = g;
	putc = p;
}

char getchar(void)
{
	char c;

	if (!getc)
		return 0;
	c = getc();
	if (c == '\r')
		return '\n';
	return c;
}

void putchar(char c)
{
	if (!putc)
		return;
	if (c == '\n')
		putc('\r');
	putc(c);
}
