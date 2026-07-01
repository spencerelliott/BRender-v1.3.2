/*
 * C replacements for fixed386.asm divide helpers used on POSIX/GCC builds.
 */
#include "brender.h"
#include "brmath.h"

br_fixed_ls BR_ASM_CALL BrFixedDiv(br_fixed_ls a, br_fixed_ls b)
{
	if (b == 0)
		return 0;

	return (br_fixed_ls)(((long long)a << 16) / (long long)b);
}

br_fixed_ls BR_ASM_CALL BrFixedDivR(br_fixed_ls a, br_fixed_ls b)
{
	long long num;

	if (b == 0)
		return 0;

	num = (long long)a << 16;
	if (num >= 0)
		return (br_fixed_ls)((num + (b / 2)) / b);
	return (br_fixed_ls)((num - (b / 2)) / b);
}

br_fixed_ls BR_ASM_CALL BrFixedDivF(br_fixed_ls a, br_fixed_ls b)
{
	unsigned long long num;

	if (b <= 0)
		return 0;

	num = ((unsigned long long)(br_uint_32)a) << 31;
	return (br_fixed_ls)(num / (unsigned long long)(br_uint_32)b);
}
