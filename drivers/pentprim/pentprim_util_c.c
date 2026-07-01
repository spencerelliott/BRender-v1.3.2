/*
 * C replacements for pentprim ASM utilities (safediv.asm, sar16.asm).
 */
#include "drv.h"

int BR_ASM_CALL SafeFixedMac2Div(int pa, int pb, int pc, int pd, int pe)
{
	long long num;
	unsigned int div;
	unsigned int hi, lo;
	int sign = 1;

	if (pe == 0)
		return 0;

	num = (long long)pa * (long long)pb + (long long)pc * (long long)pd;
	if (num < 0) {
		sign = -1;
		num = -num;
	}

	hi = (unsigned int)(num >> 32);
	lo = (unsigned int)num;
	div = (unsigned int)pe;
	if ((int)div < 0) {
		sign = -sign;
		div = (unsigned int)(-(int)div);
	}

	if (hi >= div)
		return 0;

	return (int)((((unsigned long long)hi << 32) | lo) / div) * sign;
}

br_int_32 BR_ASM_CALL _sar16(br_int_32 a)
{
	return a >> 16;
}

void BR_ASM_CALL RasteriseBufferDisable(void)
{
}
