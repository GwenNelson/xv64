#include <stdarg.h>

#include "types.h"
#include "stat.h"
#include "user.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

static void
putc(int fd, char c)
{
	write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
	static char digits[] = "0123456789ABCDEF";
	char buf[16];
	int i, neg;
	uint x;

	neg = 0;
	if(sgn && xx < 0) {
		neg = 1;
		x = -xx;
	} else {
		x = xx;
	}

	i = 0;
	do {
		buf[i++] = digits[x % base];
	} while((x /= base) != 0);
	if(neg)
		buf[i++] = '-';

	while(--i >= 0)
		putc(fd, buf[i]);
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void fprintf(int32 fd, char *fmt, ...){
	va_list ap;
	char *s;
	int c, i, state;
	va_start(ap, fmt);

	state = 0;
	for(i = 0; fmt[i]; i++) {
		c = fmt[i] & 0xff;
		if(state == 0) {
			if(c == '%') {
				state = '%';
			} else {
				putc(fd, c);
			}
		} else if(state == '%') {
			if(c == 'd') {
				printint(fd, va_arg(ap, int), 10, 1);
			} else if(c == 'x' || c == 'p') {
				printint(fd, va_arg(ap, int), 16, 0);
			} else if(c == 's') {
				s = va_arg(ap, char*);
				if(s == 0)
					s = "(null)";
				while(*s != 0) {
					putc(fd, *s);
					s++;
				}
			} else if(c == 'c') {
				putc(fd, va_arg(ap, uint));
			} else if(c == '%') {
				putc(fd, c);
			} else {
				// Unknown % sequence.  Print it to draw attention.
				putc(fd, '%');
				putc(fd, c);
			}
			state = 0;
		}
	}
}

int snprintf(char *UNUSED(s), unsigned int UNUSED(n), const char *UNUSED(fmt), ...) {
    return -1;
}
