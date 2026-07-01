/*
 * Portable C replacements for pixelmap inner loops (memloops.asm).
 */
#include "pm.h"
#include <stdint.h>
#include <string.h>

br_uint_16 BR_ASM_CALL _GetSysQual(void)
{
	return 0;
}

static void mem_copy_bytes(char *dest, char *src, br_uint_32 bytes)
{
	if (bytes)
		memcpy(dest, src, bytes);
}

void BR_ASM_CALL _MemCopy_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qual,
	br_uint_32 pixels, br_uint_32 bpp)
{
	(void)dest_qual;
	(void)src_qual;
	mem_copy_bytes(dest, src, pixels * bpp);
}

void BR_ASM_CALL _MemFill_A(char *dest, br_uint_32 dest_qual, br_uint_32 pixels, br_uint_32 bpp,
	br_uint_32 colour)
{
	br_uint_32 i;
	br_uint_32 bytes = pixels * bpp;
	(void)dest_qual;

	switch (bpp) {
	case 1:
		memset(dest, colour & 0xff, bytes);
		break;
	case 2:
		for (i = 0; i < pixels; i++)
			((br_uint_16 *)(void *)dest)[i] = (br_uint_16)colour;
		break;
	case 3:
		for (i = 0; i < pixels; i++) {
			dest[i * 3 + 0] = (char)(colour & 0xff);
			dest[i * 3 + 1] = (char)((colour >> 8) & 0xff);
			dest[i * 3 + 2] = (char)((colour >> 16) & 0xff);
		}
		break;
	case 4:
		for (i = 0; i < pixels; i++)
			((br_uint_32 *)(void *)dest)[i] = colour;
		break;
	default:
		break;
	}
}

void BR_ASM_CALL _MemRectFill_A(char *dest, br_uint_32 dest_qual, br_uint_32 pwidth, br_uint_32 pheight,
	br_int_32 d_stride, br_uint_32 bpp, br_uint_32 colour)
{
	br_uint_32 row;
	char *row_ptr = dest;
	br_int_32 row_bytes = (br_int_32)(pwidth * bpp);

	(void)dest_qual;

	for (row = 0; row < pheight; row++) {
		_MemFill_A(row_ptr, 0, pwidth, bpp, colour);
		row_ptr += d_stride;
	}

	(void)row_bytes;
}

void BR_ASM_CALL _MemRectCopy_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qualifier,
	br_uint_32 pwidth, br_uint_32 pheight, br_int_32 d_stride, br_int_32 s_stride, br_uint_32 bpp)
{
	br_uint_32 row;
	char *d = dest;
	char *s = src;
	br_uint_32 row_bytes = pwidth * bpp;

	(void)dest_qual;
	(void)src_qualifier;

	for (row = 0; row < pheight; row++) {
		mem_copy_bytes(d, s, row_bytes);
		d += d_stride;
		s += s_stride;
	}
}

static br_boolean pixel_is_zero(const char *src, br_uint_32 bpp)
{
	br_uint_32 i;

	for (i = 0; i < bpp; i++) {
		if (src[i] != 0)
			return BR_FALSE;
	}
	return BR_TRUE;
}

void BR_ASM_CALL _MemCopySourceColourKey0_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qualifier,
	br_uint_32 pixels, br_uint_32 bpp)
{
	br_uint_32 i;

	(void)dest_qual;
	(void)src_qualifier;

	for (i = 0; i < pixels; i++) {
		const char *sp = src + i * bpp;
		char *dp = dest + i * bpp;

		if (!pixel_is_zero(sp, bpp))
			mem_copy_bytes(dp, (char *)sp, bpp);
	}
}

void BR_ASM_CALL _MemRectCopySourceColourKey0_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qualifier,
	br_uint_32 pwidth, br_uint_32 pheight, br_int_32 d_stride, br_int_32 s_stride, br_uint_32 bpp)
{
	br_uint_32 row;
	char *d = dest;
	char *s = src;

	(void)dest_qual;
	(void)src_qualifier;

	for (row = 0; row < pheight; row++) {
		_MemCopySourceColourKey0_A(d, 0, s, 0, pwidth, bpp);
		d += d_stride;
		s += s_stride;
	}
}

void BR_ASM_CALL _MemPixelSet(char *dest, br_uint_32 dest_qual, br_uint_32 bytes, br_uint_32 colour)
{
	(void)dest_qual;
	_MemFill_A(dest, 0, 1, bytes, colour);
}

br_uint_32 BR_ASM_CALL _MemPixelGet(char *dest, br_uint_32 dest_qual, br_uint_32 bytes)
{
	br_uint_32 value = 0;
	br_uint_32 i;

	(void)dest_qual;

	for (i = 0; i < bytes && i < 4; i++)
		value |= ((br_uint_32)(br_uint_8)dest[i]) << (i * 8);

	return value;
}

void BR_ASM_CALL _MemCopyBits_A(char *dest, br_uint_32 dest_qual, br_int_32 d_stride, br_uint_8 *src,
	br_uint_32 s_stride, br_uint_32 start_bit, br_uint_32 end_bit, br_uint_32 nrows, br_uint_32 bpp,
	br_uint_32 colour)
{
	(void)dest;
	(void)dest_qual;
	(void)d_stride;
	(void)src;
	(void)s_stride;
	(void)start_bit;
	(void)end_bit;
	(void)nrows;
	(void)bpp;
	(void)colour;
}

#define FPU_MEMOPS _MemCopy_A

void BR_ASM_CALL _MemFillFPU_A(char *dest, br_uint_32 dest_qual, br_uint_32 pixels, br_uint_32 bpp,
	br_uint_32 colour)
{
	_MemFill_A(dest, dest_qual, pixels, bpp, colour);
}

void BR_ASM_CALL _MemRectFillFPU_A(char *dest, br_uint_32 dest_qual, br_uint_32 pwidth, br_uint_32 pheight,
	br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour)
{
	_MemRectFill_A(dest, dest_qual, pwidth, pheight, (br_int_32)stride, bpp, colour);
}

void BR_ASM_CALL _MemCopyFPU_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qualifier,
	br_uint_32 pixels, br_uint_32 bpp)
{
	FPU_MEMOPS(dest, dest_qual, src, src_qualifier, pixels, bpp);
}

void BR_ASM_CALL _MemRectCopyFPU_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qualifier,
	br_uint_32 pwidth, br_uint_32 pheight, br_uint_32 d_stride, br_uint_32 s_stride, br_uint_32 bpp)
{
	_MemRectCopy_A(dest, dest_qual, src, src_qualifier, pwidth, pheight, (br_int_32)d_stride, (br_int_32)s_stride, bpp);
}
