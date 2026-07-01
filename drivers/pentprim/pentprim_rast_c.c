/*
 * Portable C triangle rasterizers for pentprim (Phase 1).
 */
#include "drv.h"
#include "shortcut.h"

static void swap_vertex(brp_vertex **a, brp_vertex **b)
{
	brp_vertex *t = *a;
	*a = *b;
	*b = t;
}

static br_int_32 fixed_to_screen(br_fixed_ls v)
{
	return BrFixedToInt(v);
}

static br_uint_16 encode_depth(br_fixed_ls z)
{
	return (br_uint_16)((br_uint_32)z >> 16) ^ 0x8000U;
}

static br_int_8 encode_index(br_fixed_ls i)
{
	return (br_int_8)BrFixedToInt(i);
}

static void raster_span_z_i8_d16(br_int_32 y, br_int_32 x0, br_int_32 x1,
	br_fixed_ls z0, br_fixed_ls dz, br_int_8 colour, br_boolean interpolate_i,
	br_fixed_ls i0, br_fixed_ls di)
{
	br_int_32 x;
	br_int_32 width = work.colour.width_p;
	br_int_32 height = work.colour.height;
	br_int_32 stride = work.colour.stride_b;
	char *colour_base = (char *)work.colour.base;
	br_uint_16 *depth_base = (br_uint_16 *)work.depth.base;
	br_fixed_ls z = z0;
	br_fixed_ls intensity = i0;
	br_uint_16 pz;
	br_int_32 row;
	br_int_32 span;

	if (y < 0 || y >= height)
		return;

	if (x0 > x1) {
		br_int_32 t = x0;
		x0 = x1;
		x1 = t;
		z = z0 + dz * (br_fixed_ls)(x1 - x0);
		if (interpolate_i)
			intensity = i0 + di * (br_fixed_ls)(x1 - x0);
	}

	if (x0 < 0) {
		z += dz * (br_fixed_ls)(-x0);
		if (interpolate_i)
			intensity += di * (br_fixed_ls)(-x0);
		x0 = 0;
	}
	if (x1 >= width)
		x1 = width - 1;
	if (x0 > x1)
		return;

	row = y * stride;
	span = x1 - x0 + 1;

	for (x = 0; x < span; x++) {
		br_int_32 o = row + x0 + x;
		br_int_8 pix = interpolate_i ? encode_index(intensity) : colour;

		pz = encode_depth(z);
		if (depth_base[o] > pz) {
			depth_base[o] = pz;
			colour_base[o] = pix;
		}

		z += dz;
		if (interpolate_i)
			intensity += di;
	}
}

static void raster_triangle_z(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2, br_boolean interpolate_i)
{
	brp_vertex *t, *m, *b;
	br_int_32 y0, y1, y2;
	br_int_32 x;
	br_int_32 y;
	br_int_32 dx_long, dy_long;
	br_fixed_ls x_long, dx_long_step;
	br_fixed_ls x_top, dx_top_step;
	br_fixed_ls x_bot, dx_bot_step;
	br_fixed_ls z_long, dz_long_step;
	br_fixed_ls z_top, dz_top_step;
	br_fixed_ls z_bot, dz_bot_step;
	br_fixed_ls i_long, di_long_step;
	br_fixed_ls i_top, di_top_step;
	br_fixed_ls i_bot, di_bot_step;
	br_int_8 flat_colour;
	br_int_32 top_count, bot_count;

	t = v0;
	m = v1;
	b = v2;

	if (fixed_to_screen(t->comp_x[C_SY]) > fixed_to_screen(m->comp_x[C_SY]))
		swap_vertex(&t, &m);
	if (fixed_to_screen(m->comp_x[C_SY]) > fixed_to_screen(b->comp_x[C_SY]))
		swap_vertex(&m, &b);
	if (fixed_to_screen(t->comp_x[C_SY]) > fixed_to_screen(m->comp_x[C_SY]))
		swap_vertex(&t, &m);

	y0 = fixed_to_screen(t->comp_x[C_SY]);
	y1 = fixed_to_screen(m->comp_x[C_SY]);
	y2 = fixed_to_screen(b->comp_x[C_SY]);

	if (y0 == y2)
		return;

	flat_colour = encode_index(t->comp_x[C_I]);

	dy_long = y2 - y0;
	dx_long = fixed_to_screen(b->comp_x[C_SX]) - fixed_to_screen(t->comp_x[C_SX]);
	x_long = BrIntToFixed(fixed_to_screen(t->comp_x[C_SX]));
	dx_long_step = dy_long ? BrFixedDiv(BrIntToFixed(dx_long), BrIntToFixed(dy_long)) : 0;
	z_long = t->comp_x[C_SZ];
	dz_long_step = dy_long ? BrFixedDiv(b->comp_x[C_SZ] - t->comp_x[C_SZ], BrIntToFixed(dy_long)) : 0;
	i_long = t->comp_x[C_I];
	di_long_step = dy_long ? BrFixedDiv(b->comp_x[C_I] - t->comp_x[C_I], BrIntToFixed(dy_long)) : 0;

	x_top = x_long;
	dx_top_step = dx_long_step;
	z_top = z_long;
	dz_top_step = dz_long_step;
	i_top = i_long;
	di_top_step = di_long_step;
	top_count = y1 - y0;

	if (top_count > 0) {
		br_int_32 dx_top = fixed_to_screen(m->comp_x[C_SX]) - fixed_to_screen(t->comp_x[C_SX]);
		dx_top_step = BrFixedDiv(BrIntToFixed(dx_top), BrIntToFixed(top_count));
		dz_top_step = BrFixedDiv(m->comp_x[C_SZ] - t->comp_x[C_SZ], BrIntToFixed(top_count));
		di_top_step = BrFixedDiv(m->comp_x[C_I] - t->comp_x[C_I], BrIntToFixed(top_count));
	}

	x_bot = x_long + dx_long_step * (br_fixed_ls)top_count;
	dx_bot_step = dx_long_step;
	z_bot = z_long + dz_long_step * (br_fixed_ls)top_count;
	dz_bot_step = dz_long_step;
	i_bot = i_long + di_long_step * (br_fixed_ls)top_count;
	di_bot_step = di_long_step;
	bot_count = y2 - y1;

	if (bot_count > 0) {
		br_int_32 dx_bot = fixed_to_screen(b->comp_x[C_SX]) - fixed_to_screen(m->comp_x[C_SX]);
		dx_bot_step = BrFixedDiv(BrIntToFixed(dx_bot), BrIntToFixed(bot_count));
		dz_bot_step = BrFixedDiv(b->comp_x[C_SZ] - m->comp_x[C_SZ], BrIntToFixed(bot_count));
		di_bot_step = BrFixedDiv(b->comp_x[C_I] - m->comp_x[C_I], BrIntToFixed(bot_count));
	}

	for (y = y0; y < y1; y++) {
		br_int_32 xa = BrFixedToInt(x_top);
		br_int_32 xb = BrFixedToInt(x_long);
		br_fixed_ls z0 = z_top;
		br_fixed_ls dz = BrFixedDiv(z_long - z_top, BrIntToFixed(xb - xa ? xb - xa : 1));
		br_fixed_ls i0 = i_top;
		br_fixed_ls di = BrFixedDiv(i_long - i_top, BrIntToFixed(xb - xa ? xb - xa : 1));

		raster_span_z_i8_d16(y, xa, xb, z0, dz, flat_colour, interpolate_i, i0, di);

		x_top += dx_top_step;
		z_top += dz_top_step;
		i_top += di_top_step;
		x_long += dx_long_step;
		z_long += dz_long_step;
		i_long += di_long_step;
	}

	x_top = x_bot;
	z_top = z_bot;
	i_top = i_bot;
	dx_top_step = dx_bot_step;
	dz_top_step = dz_bot_step;
	di_top_step = di_bot_step;

	for (y = y1; y <= y2; y++) {
		br_int_32 xa = BrFixedToInt(x_top);
		br_int_32 xb = BrFixedToInt(x_long);
		br_fixed_ls z0 = z_top;
		br_fixed_ls dz = BrFixedDiv(z_long - z_top, BrIntToFixed(xb - xa ? xb - xa : 1));
		br_fixed_ls i0 = i_top;
		br_fixed_ls di = BrFixedDiv(i_long - i_top, BrIntToFixed(xb - xa ? xb - xa : 1));

		raster_span_z_i8_d16(y, xa, xb, z0, dz, flat_colour, interpolate_i, i0, di);

		x_top += dx_top_step;
		z_top += dz_top_step;
		i_top += di_top_step;
		x_long += dx_long_step;
		z_long += dz_long_step;
		i_long += di_long_step;
	}
}

static void raster_triangle_i8(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2)
{
	br_int_32 y;
	br_int_32 y0, y1, y2;
	brp_vertex *t = v0, *m = v1, *b = v2;
	br_fixed_ls x_long, dx_long_step;
	br_fixed_ls x_top, dx_top_step;
	br_int_32 dy_long, top_count, bot_count;
	br_int_8 flat_colour;

	if (fixed_to_screen(t->comp_x[C_SY]) > fixed_to_screen(m->comp_x[C_SY]))
		swap_vertex(&t, &m);
	if (fixed_to_screen(m->comp_x[C_SY]) > fixed_to_screen(b->comp_x[C_SY]))
		swap_vertex(&m, &b);
	if (fixed_to_screen(t->comp_x[C_SY]) > fixed_to_screen(m->comp_x[C_SY]))
		swap_vertex(&t, &m);

	y0 = fixed_to_screen(t->comp_x[C_SY]);
	y1 = fixed_to_screen(m->comp_x[C_SY]);
	y2 = fixed_to_screen(b->comp_x[C_SY]);
	if (y0 == y2)
		return;

	flat_colour = encode_index(t->comp_x[C_I]);
	dy_long = y2 - y0;
	x_long = BrIntToFixed(fixed_to_screen(t->comp_x[C_SX]));
	dx_long_step = dy_long ? BrFixedDiv(
		BrIntToFixed(fixed_to_screen(b->comp_x[C_SX]) - fixed_to_screen(t->comp_x[C_SX])),
		BrIntToFixed(dy_long)) : 0;

	x_top = x_long;
	dx_top_step = dx_long_step;
	top_count = y1 - y0;
	if (top_count > 0)
		dx_top_step = BrFixedDiv(
			BrIntToFixed(fixed_to_screen(m->comp_x[C_SX]) - fixed_to_screen(t->comp_x[C_SX])),
			BrIntToFixed(top_count));

	for (y = y0; y < y1; y++) {
		br_int_32 x0 = BrFixedToInt(x_top);
		br_int_32 x1 = BrFixedToInt(x_long);
		br_int_32 width = work.colour.width_p;
		br_int_32 height = work.colour.height;
		br_int_32 stride = work.colour.stride_b;
		char *base = (char *)work.colour.base;
		br_int_32 x, row;

		if (y >= 0 && y < height) {
			if (x0 > x1) { br_int_32 tmp = x0; x0 = x1; x1 = tmp; }
			if (x0 < 0) x0 = 0;
			if (x1 >= width) x1 = width - 1;
			row = y * stride;
			for (x = x0; x <= x1; x++)
				base[row + x] = flat_colour;
		}

		x_top += dx_top_step;
		x_long += dx_long_step;
	}

	x_top = x_long - dx_long_step * (br_fixed_ls)top_count;
	dx_top_step = dx_long_step;
	if ((y2 - y1) > 0)
		dx_top_step = BrFixedDiv(
			BrIntToFixed(fixed_to_screen(b->comp_x[C_SX]) - fixed_to_screen(m->comp_x[C_SX])),
			BrIntToFixed(y2 - y1));

	for (y = y1; y <= y2; y++) {
		br_int_32 x0 = BrFixedToInt(x_top);
		br_int_32 x1 = BrFixedToInt(x_long);
		br_int_32 width = work.colour.width_p;
		br_int_32 height = work.colour.height;
		br_int_32 stride = work.colour.stride_b;
		char *base = (char *)work.colour.base;
		br_int_32 x, row;

		if (y >= 0 && y < height) {
			if (x0 > x1) { br_int_32 tmp = x0; x0 = x1; x1 = tmp; }
			if (x0 < 0) x0 = 0;
			if (x1 >= width) x1 = width - 1;
			row = y * stride;
			for (x = x0; x <= x1; x++)
				base[row + x] = flat_colour;
		}

		x_top += dx_top_step;
		x_long += dx_long_step;
	}
}

void BR_ASM_CALL TriangleRender_Z_I8_D16(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2)
{
	(void)block;
	raster_triangle_z(v0, v1, v2, BR_FALSE);
}

void BR_ASM_CALL TriangleRender_ZI_I8_D16(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2)
{
	(void)block;
	raster_triangle_z(v0, v1, v2, BR_TRUE);
}

void BR_ASM_CALL TriangleRender_I8(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2)
{
	(void)block;
	raster_triangle_i8(v0, v1, v2);
}
