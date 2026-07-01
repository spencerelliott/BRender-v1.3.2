#include <stdio.h>
#include <string.h>
#include "brender.h"
#include "priminfo.h"
#include "work.h"

void BR_ASM_CALL TriangleRender_Z_I8_D16(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);

static void set_screen_vertex(brp_vertex *v, br_int_32 x, br_int_32 y, br_int_32 z, br_int_32 i)
{
	memset(v, 0, sizeof(*v));
	v->comp_x[C_SX] = BrIntToFixed(x);
	v->comp_x[C_SY] = BrIntToFixed(y);
	v->comp_x[C_SZ] = BrIntToFixed(z);
	v->comp_x[C_I] = BrIntToFixed(i);
}

int main(void)
{
	static char colour_buf[64 * 64];
	static br_uint_16 depth_buf[64 * 64];
	brp_vertex v0, v1, v2;
	int pixels = 0;
	int x, y;

	memset(colour_buf, 0, sizeof(colour_buf));
	memset(depth_buf, 0xff, sizeof(depth_buf));

	work.colour.base = colour_buf;
	work.colour.width_p = 64;
	work.colour.height = 64;
	work.colour.stride_b = 64;
	work.colour.bpp = 1;

	work.depth.base = depth_buf;
	work.depth.width_p = 64;
	work.depth.height = 64;
	work.depth.stride_b = 64 * 2;
	work.depth.bpp = 2;

	set_screen_vertex(&v0, 10, 10, 100, 5);
	set_screen_vertex(&v1, 50, 10, 100, 5);
	set_screen_vertex(&v2, 30, 40, 100, 5);

	TriangleRender_Z_I8_D16(NULL, &v0, &v1, &v2);

	for (y = 0; y < 64; y++) {
		for (x = 0; x < 64; x++) {
			if (colour_buf[y * 64 + x] != 0)
				pixels++;
		}
	}

	fprintf(stderr, "pixels=%d\n", pixels);
	if (pixels == 0)
		return 1;

	fprintf(stderr, "ok\n");
	return 0;
}
