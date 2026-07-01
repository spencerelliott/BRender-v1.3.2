/*
 * C replacements for softrend ASM helpers (subdiv.asm).
 */
#include "drv.h"
#include "rend.h"
#include "shortcut.h"

br_boolean BR_ASM_CALL subdivideCheck(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2)
{
	br_scalar z0, z1, z2, zt;

	z0 = BR_ABS(v0->comp[C_Z]);
	z1 = BR_ABS(v1->comp[C_Z]);
	z2 = BR_ABS(v2->comp[C_Z]);

	if (z0 > z1) {
		zt = z0;
		z0 = z1;
		z1 = zt;
	}
	if (z0 > z2) {
		zt = z0;
		z0 = z2;
		z2 = zt;
	}
	if (z1 > z2) {
		zt = z1;
		z1 = z2;
		z2 = zt;
	}

	if (z2 == BR_SCALAR(0.0))
		return BR_FALSE;

	if (z0 > BR_MUL(rend.subdivide_threshold, z2))
		return BR_FALSE;

	return BR_TRUE;
}

#if BASED_FLOAT

void BR_ASM_CALL averageVertices(struct br_renderer *renderer, brp_vertex *dest1, brp_vertex *dest2,
	brp_vertex *dest3, brp_vertex *src1, brp_vertex *src2, brp_vertex *src3)
{
	(void)renderer;

	dest1->comp[C_X] = BR_CONST_DIV(BR_ADD(src1->comp[C_X], src2->comp[C_X]), 2);
	dest1->comp[C_Y] = BR_CONST_DIV(BR_ADD(src1->comp[C_Y], src2->comp[C_Y]), 2);
	dest1->comp[C_Z] = BR_CONST_DIV(BR_ADD(src1->comp[C_Z], src2->comp[C_Z]), 2);
	dest1->comp[C_W] = BR_CONST_DIV(BR_ADD(src1->comp[C_W], src2->comp[C_W]), 2);
	dest1->comp[C_U] = BR_CONST_DIV(BR_ADD(src1->comp[C_U], src2->comp[C_U]), 2);
	dest1->comp[C_V] = BR_CONST_DIV(BR_ADD(src1->comp[C_V], src2->comp[C_V]), 2);
	dest1->comp[C_I] = BR_CONST_DIV(BR_ADD(src1->comp[C_I], src2->comp[C_I]), 2);
	dest1->comp[C_R] = BR_CONST_DIV(BR_ADD(src1->comp[C_R], src2->comp[C_R]), 2);
	dest1->comp[C_G] = BR_CONST_DIV(BR_ADD(src1->comp[C_G], src2->comp[C_G]), 2);
	dest1->comp[C_B] = BR_CONST_DIV(BR_ADD(src1->comp[C_B], src2->comp[C_B]), 2);
	dest1->flags = OUTCODES_NOT;
	OUTCODE_POINT(dest1->flags, (br_vector4 *)(dest1->comp + C_X));
	if (!(dest1->flags & OUTCODES_ALL))
		PROJECT_VERTEX_WRITE_Q(dest1, dest1->comp[C_X], dest1->comp[C_Y], dest1->comp[C_Z], dest1->comp[C_W]);

	dest2->comp[C_X] = BR_CONST_DIV(BR_ADD(src2->comp[C_X], src3->comp[C_X]), 2);
	dest2->comp[C_Y] = BR_CONST_DIV(BR_ADD(src2->comp[C_Y], src3->comp[C_Y]), 2);
	dest2->comp[C_Z] = BR_CONST_DIV(BR_ADD(src2->comp[C_Z], src3->comp[C_Z]), 2);
	dest2->comp[C_W] = BR_CONST_DIV(BR_ADD(src2->comp[C_W], src3->comp[C_W]), 2);
	dest2->comp[C_U] = BR_CONST_DIV(BR_ADD(src2->comp[C_U], src3->comp[C_U]), 2);
	dest2->comp[C_V] = BR_CONST_DIV(BR_ADD(src2->comp[C_V], src3->comp[C_V]), 2);
	dest2->comp[C_I] = BR_CONST_DIV(BR_ADD(src2->comp[C_I], src3->comp[C_I]), 2);
	dest2->comp[C_R] = BR_CONST_DIV(BR_ADD(src2->comp[C_R], src3->comp[C_R]), 2);
	dest2->comp[C_G] = BR_CONST_DIV(BR_ADD(src2->comp[C_G], src3->comp[C_G]), 2);
	dest2->comp[C_B] = BR_CONST_DIV(BR_ADD(src2->comp[C_B], src3->comp[C_B]), 2);
	dest2->flags = OUTCODES_NOT;
	OUTCODE_POINT(dest2->flags, (br_vector4 *)(dest2->comp + C_X));
	if (!(dest2->flags & OUTCODES_ALL))
		PROJECT_VERTEX_WRITE_Q(dest2, dest2->comp[C_X], dest2->comp[C_Y], dest2->comp[C_Z], dest2->comp[C_W]);

	dest3->comp[C_X] = BR_CONST_DIV(BR_ADD(src3->comp[C_X], src1->comp[C_X]), 2);
	dest3->comp[C_Y] = BR_CONST_DIV(BR_ADD(src3->comp[C_Y], src1->comp[C_Y]), 2);
	dest3->comp[C_Z] = BR_CONST_DIV(BR_ADD(src3->comp[C_Z], src1->comp[C_Z]), 2);
	dest3->comp[C_W] = BR_CONST_DIV(BR_ADD(src3->comp[C_W], src1->comp[C_W]), 2);
	dest3->comp[C_U] = BR_CONST_DIV(BR_ADD(src3->comp[C_U], src1->comp[C_U]), 2);
	dest3->comp[C_V] = BR_CONST_DIV(BR_ADD(src3->comp[C_V], src1->comp[C_V]), 2);
	dest3->comp[C_I] = BR_CONST_DIV(BR_ADD(src3->comp[C_I], src1->comp[C_I]), 2);
	dest3->comp[C_R] = BR_CONST_DIV(BR_ADD(src3->comp[C_R], src1->comp[C_R]), 2);
	dest3->comp[C_G] = BR_CONST_DIV(BR_ADD(src3->comp[C_G], src1->comp[C_G]), 2);
	dest3->comp[C_B] = BR_CONST_DIV(BR_ADD(src3->comp[C_B], src1->comp[C_B]), 2);
	dest3->flags = OUTCODES_NOT;
	OUTCODE_POINT(dest3->flags, (br_vector4 *)(dest3->comp + C_X));
	if (!(dest3->flags & OUTCODES_ALL))
		PROJECT_VERTEX_WRITE_Q(dest3, dest3->comp[C_X], dest3->comp[C_Y], dest3->comp[C_Z], dest3->comp[C_W]);
}

void BR_ASM_CALL averageVerticesOnScreen(struct br_renderer *renderer, brp_vertex *dest1, brp_vertex *dest2,
	brp_vertex *dest3, brp_vertex *src1, brp_vertex *src2, brp_vertex *src3)
{
	(void)renderer;

	dest1->comp[C_X] = BR_CONST_DIV(BR_ADD(src1->comp[C_X], src2->comp[C_X]), 2);
	dest1->comp[C_Y] = BR_CONST_DIV(BR_ADD(src1->comp[C_Y], src2->comp[C_Y]), 2);
	dest1->comp[C_Z] = BR_CONST_DIV(BR_ADD(src1->comp[C_Z], src2->comp[C_Z]), 2);
	dest1->comp[C_W] = BR_CONST_DIV(BR_ADD(src1->comp[C_W], src2->comp[C_W]), 2);
	dest1->comp[C_U] = BR_CONST_DIV(BR_ADD(src1->comp[C_U], src2->comp[C_U]), 2);
	dest1->comp[C_V] = BR_CONST_DIV(BR_ADD(src1->comp[C_V], src2->comp[C_V]), 2);
	dest1->comp[C_I] = BR_CONST_DIV(BR_ADD(src1->comp[C_I], src2->comp[C_I]), 2);
	dest1->comp[C_R] = BR_CONST_DIV(BR_ADD(src1->comp[C_R], src2->comp[C_R]), 2);
	dest1->comp[C_G] = BR_CONST_DIV(BR_ADD(src1->comp[C_G], src2->comp[C_G]), 2);
	dest1->comp[C_B] = BR_CONST_DIV(BR_ADD(src1->comp[C_B], src2->comp[C_B]), 2);
	PROJECT_VERTEX_WRITE_Q(dest1, dest1->comp[C_X], dest1->comp[C_Y], dest1->comp[C_Z], dest1->comp[C_W]);

	dest2->comp[C_X] = BR_CONST_DIV(BR_ADD(src2->comp[C_X], src3->comp[C_X]), 2);
	dest2->comp[C_Y] = BR_CONST_DIV(BR_ADD(src2->comp[C_Y], src3->comp[C_Y]), 2);
	dest2->comp[C_Z] = BR_CONST_DIV(BR_ADD(src2->comp[C_Z], src3->comp[C_Z]), 2);
	dest2->comp[C_W] = BR_CONST_DIV(BR_ADD(src2->comp[C_W], src3->comp[C_W]), 2);
	dest2->comp[C_U] = BR_CONST_DIV(BR_ADD(src2->comp[C_U], src3->comp[C_U]), 2);
	dest2->comp[C_V] = BR_CONST_DIV(BR_ADD(src2->comp[C_V], src3->comp[C_V]), 2);
	dest2->comp[C_I] = BR_CONST_DIV(BR_ADD(src2->comp[C_I], src3->comp[C_I]), 2);
	dest2->comp[C_R] = BR_CONST_DIV(BR_ADD(src2->comp[C_R], src3->comp[C_R]), 2);
	dest2->comp[C_G] = BR_CONST_DIV(BR_ADD(src2->comp[C_G], src3->comp[C_G]), 2);
	dest2->comp[C_B] = BR_CONST_DIV(BR_ADD(src2->comp[C_B], src3->comp[C_B]), 2);
	PROJECT_VERTEX_WRITE_Q(dest2, dest2->comp[C_X], dest2->comp[C_Y], dest2->comp[C_Z], dest2->comp[C_W]);

	dest3->comp[C_X] = BR_CONST_DIV(BR_ADD(src3->comp[C_X], src1->comp[C_X]), 2);
	dest3->comp[C_Y] = BR_CONST_DIV(BR_ADD(src3->comp[C_Y], src1->comp[C_Y]), 2);
	dest3->comp[C_Z] = BR_CONST_DIV(BR_ADD(src3->comp[C_Z], src1->comp[C_Z]), 2);
	dest3->comp[C_W] = BR_CONST_DIV(BR_ADD(src3->comp[C_W], src1->comp[C_W]), 2);
	dest3->comp[C_U] = BR_CONST_DIV(BR_ADD(src3->comp[C_U], src1->comp[C_U]), 2);
	dest3->comp[C_V] = BR_CONST_DIV(BR_ADD(src3->comp[C_V], src1->comp[C_V]), 2);
	dest3->comp[C_I] = BR_CONST_DIV(BR_ADD(src3->comp[C_I], src1->comp[C_I]), 2);
	dest3->comp[C_R] = BR_CONST_DIV(BR_ADD(src3->comp[C_R], src1->comp[C_R]), 2);
	dest3->comp[C_G] = BR_CONST_DIV(BR_ADD(src3->comp[C_G], src1->comp[C_G]), 2);
	dest3->comp[C_B] = BR_CONST_DIV(BR_ADD(src3->comp[C_B], src1->comp[C_B]), 2);
	PROJECT_VERTEX_WRITE_Q(dest3, dest3->comp[C_X], dest3->comp[C_Y], dest3->comp[C_Z], dest3->comp[C_W]);
}

#endif
