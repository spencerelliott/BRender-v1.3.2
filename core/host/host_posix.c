/*
 * POSIX host layer: flat-memory replacements for legacy DOS/Win32 ASM.
 */
#include "host.h"
#include "host_ip.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(__APPLE__)
#include <sys/sysctl.h>
#elif defined(__GNUC__) || defined(__clang__)
#if defined(__i386__) || defined(__x86_64__)
#include <cpuid.h>
#endif
#endif

#if defined(__unix__) || defined(__APPLE__)
#include <dlfcn.h>
#endif

br_uint_16 _RealSelector;

br_error BR_ASM_CALL RealSelectorBegin(void)
{
	_RealSelector = 0;
	return BRE_OK;
}

void BR_ASM_CALL RealSelectorEnd(void)
{
	_RealSelector = 0;
}

br_error BR_RESIDENT_ENTRY HostInterruptGet(br_uint_8 vector, br_uint_32 *offp, br_uint_16 *selp)
{
	(void)vector;
	if (offp)
		*offp = 0;
	if (selp)
		*selp = 0;
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostInterruptSet(br_uint_8 vector, br_uint_32 off, br_uint_16 sel)
{
	(void)vector;
	(void)off;
	(void)sel;
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostExceptionGet(br_uint_8 exception, br_uint_32 *offp, br_uint_16 *selp)
{
	(void)exception;
	if (offp)
		*offp = 0;
	if (selp)
		*selp = 0;
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostExceptionSet(br_uint_8 exception, br_uint_32 off, br_uint_16 sel)
{
	(void)exception;
	(void)off;
	(void)sel;
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostInterruptCall(br_uint_8 vector, union host_regs *regs)
{
	(void)vector;
	(void)regs;
	return BRE_FAIL;
}

static char *host_ptr_u32(br_uint_32 offset, br_uint_16 sel)
{
	(void)sel;
	return (char *)(uintptr_t)offset;
}

static char *host_ptr_u16(br_uint_16 offset, br_uint_16 seg)
{
	(void)seg;
	return (char *)(uintptr_t)offset;
}

void BR_RESIDENT_ENTRY HostFarBlockWrite(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count)
{
	memcpy(host_ptr_u32(offset, sel), block, count);
}

void BR_RESIDENT_ENTRY HostFarBlockRead(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count)
{
	memcpy(block, host_ptr_u32(offset, sel), count);
}

br_uint_32 BR_RESIDENT_ENTRY HostFarStringWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max)
{
	char *dest = host_ptr_u32(offset, sel);
	br_uint_32 i;

	for (i = 0; i < max - 1 && string[i] != '\0'; i++)
		dest[i] = (char)string[i];
	dest[i] = '\0';
	return i;
}

br_uint_32 BR_RESIDENT_ENTRY HostFarStringRead(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max)
{
	char *src = host_ptr_u32(offset, sel);
	br_uint_32 i;

	for (i = 0; i < max - 1 && src[i] != '\0'; i++)
		string[i] = (br_uint_8)src[i];
	string[i] = '\0';
	return i;
}

void BR_RESIDENT_ENTRY HostFarBlockFill(br_uint_32 offset, br_uint_16 sel, br_uint_8 value, br_uint_32 count)
{
	memset(host_ptr_u32(offset, sel), value, count);
}

void BR_RESIDENT_ENTRY HostFarByteWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 value)
{
	*host_ptr_u32(offset, sel) = (char)value;
}

void BR_RESIDENT_ENTRY HostFarWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_16 value)
{
	*(br_uint_16 *)(void *)host_ptr_u32(offset, sel) = value;
}

void BR_RESIDENT_ENTRY HostFarDWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_32 value)
{
	*(br_uint_32 *)(void *)host_ptr_u32(offset, sel) = value;
}

br_uint_8 BR_RESIDENT_ENTRY HostFarByteRead(br_uint_32 offset, br_uint_16 sel)
{
	return (br_uint_8)*host_ptr_u32(offset, sel);
}

br_uint_16 BR_RESIDENT_ENTRY HostFarWordRead(br_uint_32 offset, br_uint_16 sel)
{
	return *(br_uint_16 *)(void *)host_ptr_u32(offset, sel);
}

br_uint_32 BR_RESIDENT_ENTRY HostFarDWordRead(br_uint_32 offset, br_uint_16 sel)
{
	return *(br_uint_32 *)(void *)host_ptr_u32(offset, sel);
}

void BR_RESIDENT_ENTRY HostRealBlockWrite(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count)
{
	memcpy(host_ptr_u16(offset, seg), block, count);
}

void BR_RESIDENT_ENTRY HostRealBlockRead(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count)
{
	memcpy(block, host_ptr_u16(offset, seg), count);
}

br_uint_32 BR_RESIDENT_ENTRY HostRealStringWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max)
{
	return HostFarStringWrite(offset, seg, string, max);
}

br_uint_32 BR_RESIDENT_ENTRY HostRealStringRead(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max)
{
	return HostFarStringRead(offset, seg, string, max);
}

void BR_RESIDENT_ENTRY HostRealBlockFill(br_uint_16 offset, br_uint_16 seg, br_uint_8 value, br_uint_32 count)
{
	memset(host_ptr_u16(offset, seg), value, count);
}

void BR_RESIDENT_ENTRY HostRealByteWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 value)
{
	*host_ptr_u16(offset, seg) = (char)value;
}

void BR_RESIDENT_ENTRY HostRealWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_16 value)
{
	*(br_uint_16 *)(void *)host_ptr_u16(offset, seg) = value;
}

void BR_RESIDENT_ENTRY HostRealDWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_32 value)
{
	*(br_uint_32 *)(void *)host_ptr_u16(offset, seg) = value;
}

br_uint_8 BR_RESIDENT_ENTRY HostRealByteRead(br_uint_16 offset, br_uint_16 seg)
{
	return (br_uint_8)*host_ptr_u16(offset, seg);
}

br_uint_16 BR_RESIDENT_ENTRY HostRealWordRead(br_uint_16 offset, br_uint_16 seg)
{
	return *(br_uint_16 *)(void *)host_ptr_u16(offset, seg);
}

br_uint_32 BR_RESIDENT_ENTRY HostRealDWordRead(br_uint_16 offset, br_uint_16 seg)
{
	return *(br_uint_32 *)(void *)host_ptr_u16(offset, seg);
}

br_error BR_RESIDENT_ENTRY HostRealAllocate(struct host_real_memory *mem, br_uint_32 size)
{
	void *block;

	if (mem == NULL)
		return BRE_FAIL;

	block = malloc(size);
	if (block == NULL)
		return BRE_FAIL;

	memset(block, 0, size);
	mem->pm_off = (br_uint_32)(uintptr_t)block;
	mem->pm_seg = 0;
	mem->rm_off = 0;
	mem->rm_seg = 0;
	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostRealFree(struct host_real_memory *mem)
{
	if (mem == NULL || mem->pm_off == 0)
		return BRE_FAIL;

	free((void *)(uintptr_t)mem->pm_off);
	mem->pm_off = 0;
	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostRealInterruptGet(br_uint_8 vector, br_uint_16 *offp, br_uint_16 *vsegp)
{
	(void)vector;
	if (offp)
		*offp = 0;
	if (vsegp)
		*vsegp = 0;
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostRealInterruptSet(br_uint_8 vector, br_uint_16 voff, br_uint_16 vseg)
{
	(void)vector;
	(void)voff;
	(void)vseg;
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostRealInterruptCall(br_uint_8 vector, union host_regs *regs)
{
	(void)vector;
	(void)regs;
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostRegistersGet(union host_regs *regs)
{
	if (regs)
		memset(regs, 0, sizeof(*regs));
	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostSelectorReal(br_uint_16 *selp)
{
	if (selp)
		*selp = 0;
	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostSelectorDS(br_uint_16 *selp)
{
	if (selp)
		*selp = 0;
	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostSelectorCS(br_uint_16 *selp)
{
	if (selp)
		*selp = 0;
	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostSelectorSS(br_uint_16 *selp)
{
	if (selp)
		*selp = 0;
	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostSelectorES(br_uint_16 *selp)
{
	if (selp)
		*selp = 0;
	return BRE_OK;
}

void BR_ASM_CALL CPUInfo(br_token *cpu_type, br_uint_32 *features)
{
	br_token type = BRT_INTEL_PENTIUM_PRO;
	br_uint_32 caps = HOST_CAPS_FPU;

	if (cpu_type)
		*cpu_type = type;
	if (features)
		*features = caps;

#if (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))
	if (__get_cpuid_max(0, NULL) >= 1) {
		unsigned int eax, ebx, ecx, edx;

		if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
			if (cpu_type) {
				unsigned int family = (eax >> 8) & 0xf;

				if (family <= 4)
					*cpu_type = BRT_INTEL_486;
				else if (family == 5)
					*cpu_type = BRT_INTEL_PENTIUM;
				else
					*cpu_type = BRT_INTEL_PENTIUM_PRO;
			}
			if (features) {
				if (edx & (1u << 0))
					*features |= HOST_CAPS_FPU;
				if (edx & (1u << 23))
					*features |= HOST_CAPS_MMX;
				if (edx & (1u << 15))
					*features |= HOST_CAPS_CMOV;
			}
		}
	}
#endif
}

#if defined(__unix__) || defined(__APPLE__)

void *BR_RESIDENT_ENTRY HostImageLoad(char *name)
{
	return dlopen(name, RTLD_NOW | RTLD_LOCAL);
}

void BR_RESIDENT_ENTRY HostImageUnload(void *image)
{
	if (image)
		(void)dlclose(image);
}

void *BR_RESIDENT_ENTRY HostImageLookupName(void *img, char *name, br_uint_32 hint)
{
	(void)hint;
	if (img == NULL || name == NULL)
		return NULL;
	return dlsym(img, name);
}

void *BR_RESIDENT_ENTRY HostImageLookupOrdinal(void *img, br_uint_32 ordinal)
{
	(void)img;
	(void)ordinal;
	return NULL;
}

#endif
