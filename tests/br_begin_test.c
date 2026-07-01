#include <stdio.h>
#include "brender.h"

int main(void)
{
	br_error err;

	fprintf(stderr, "BrBegin\n");
	fflush(stderr);
	err = BrBegin();
	fprintf(stderr, "BrBegin=%d\n", (int)err);
	fflush(stderr);
	if (err != BRE_OK)
		return 1;

	fprintf(stderr, "BrEnd\n");
	fflush(stderr);
	err = BrEnd();
	fprintf(stderr, "BrEnd=%d\n", (int)err);
	fflush(stderr);
	if (err != BRE_OK)
		return 2;

	fprintf(stderr, "ok\n");
	return 0;
}
