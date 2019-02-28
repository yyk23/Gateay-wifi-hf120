#include "gslice.h"


void *g_slice_alloc0(size_t size)
{
	void *p = (void *)malloc(size);
	if (p == NULL) { return NULL; }
	memset(p, 0, size);
	return p;
}