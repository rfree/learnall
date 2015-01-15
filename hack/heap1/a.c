#include <stdlib.h>

int func() {
	const size_t size=64; volatile char *x = malloc(size), p=x; size_t i;	for (i=0; i<size; ++i) *(x++) = 0x0C;
	typedef void (*t_func)(void); 	((t_func)(p)) ();
}

int main() { func(); return 0; }
