/*
 * Portable C implementations of block memory operations.
 */
#include "brender.h"
#include <stdint.h>
#include <string.h>

void BR_ASM_CALL BrBlockFill(void *dest_ptr, int value, int dwords);
void BR_ASM_CALL BrBlockCopy(void *dest_ptr, void *src_ptr, int dwords);
void BR_ASM_CALL BrFarBlockCopy(void *dest_ptr, void *src_ptr, int dwords);

void BR_ASM_CALL BrBlockFill(void *dest_ptr, int value, int dwords)
{
	memset(dest_ptr, value & 0xff, (size_t)dwords * sizeof(br_uint_32));
}

void BR_ASM_CALL BrBlockCopy(void *dest_ptr, void *src_ptr, int dwords)
{
	memcpy(dest_ptr, src_ptr, (size_t)dwords * sizeof(br_uint_32));
}

void BR_ASM_CALL BrFarBlockCopy(void *dest_ptr, void *src_ptr, int dwords)
{
	memcpy(dest_ptr, src_ptr, (size_t)dwords * sizeof(br_uint_32));
}
