/*
 * Minimal built-in font placeholders (replaces fnt*.asm for C-only builds).
 */
#include "brender.h"

static br_uint_16 font_space_glyph[5];

static br_uint_16 font_encoding[96];

struct br_font BR_ASM_DATA _FontFixed3x5 = {
	0,
	3,
	5,
	4,
	6,
	NULL,
	font_encoding,
	font_space_glyph,
};

struct br_font BR_ASM_DATA _FontProp4x6 = {
	0,
	4,
	6,
	5,
	7,
	NULL,
	font_encoding,
	font_space_glyph,
};

struct br_font BR_ASM_DATA _FontProp7x9 = {
	0,
	7,
	9,
	8,
	10,
	NULL,
	font_encoding,
	font_space_glyph,
};
