#include <stdio.h>
#include <string.h>
#include "brender.h"

struct br_device * BR_EXPORT BrDrv1SoftRendBegin(char *arguments);
struct br_device * BR_EXPORT BrDrv1SoftPrimBegin(char *arguments);

#define PM_ROW(pm, y) (((char *)((pm)->pm_pixels)) + (y) * (pm)->pm_row_bytes)

static int count_rendered_pixels(br_pixelmap *colour, br_pixelmap *depth)
{
	br_device_pixelmap *colour_pm = (br_device_pixelmap *)colour;
	br_device_pixelmap *depth_pm = (br_device_pixelmap *)depth;
	int x, y, count = 0;

	for (y = 0; y < colour_pm->pm_height; y++) {
		char *colour_row = PM_ROW(colour_pm, y);
		char *depth_row = PM_ROW(depth_pm, y);
		for (x = 0; x < colour_pm->pm_width; x++) {
			br_uint_16 *depth_px = (br_uint_16 *)(depth_row + x * 2);

			if (colour_row[x] != 0 || *depth_px != 0xffff)
				count++;
		}
	}
	return count;
}

static br_model *make_triangle_model(br_material *material)
{
	br_model *model;

	model = BrModelAllocate("triangle", 3, 1);
	model->flags = BR_MODF_UPDATEABLE;

	BrVector3Set(&model->vertices[0].p, BR_SCALAR(-1.0), BR_SCALAR(-1.0), BR_SCALAR(0.0));
	BrVector3Set(&model->vertices[1].p, BR_SCALAR(1.0), BR_SCALAR(-1.0), BR_SCALAR(0.0));
	BrVector3Set(&model->vertices[2].p, BR_SCALAR(0.0), BR_SCALAR(1.0), BR_SCALAR(0.5));

	model->faces[0].vertices[0] = 0;
	model->faces[0].vertices[1] = 1;
	model->faces[0].vertices[2] = 2;
	model->faces[0].material = material;
	model->faces[0].index = 5;

	return model;
}

static br_actor *setup_scene(br_model *model, br_actor **camera_out, br_actor **model_actor_out)
{
	br_actor *world;
	br_actor *camera;
	br_actor *model_actor;
	br_camera *camera_data;

	world = BrActorAllocate(BR_ACTOR_NONE, NULL);
	world->t.type = BR_TRANSFORM_IDENTITY;

	camera = BrActorAdd(world, BrActorAllocate(BR_ACTOR_CAMERA, NULL));
	camera->t.type = BR_TRANSFORM_MATRIX34;
	BrMatrix34Translate(&camera->t.t.mat, BR_SCALAR(0.0), BR_SCALAR(0.0), BR_SCALAR(5.0));

	camera_data = (br_camera *)camera->type_data;
	camera_data->type = BR_CAMERA_PERSPECTIVE;
	camera_data->field_of_view = BR_ANGLE_DEG(45.0);
	camera_data->aspect = BR_SCALAR(1.0);
	camera_data->hither_z = BR_SCALAR(0.1);
	camera_data->yon_z = BR_SCALAR(50.0);

	model_actor = BrActorAdd(world, BrActorAllocate(BR_ACTOR_MODEL, NULL));
	model_actor->model = model;
	model_actor->t.type = BR_TRANSFORM_IDENTITY;

	*camera_out = camera;
	*model_actor_out = model_actor;
	return world;
}

int main(void)
{
	br_error err;
	br_pixelmap *colour;
	br_pixelmap *depth;
	br_material *material;
	br_model *model;
	br_actor *world;
	br_actor *camera;
	br_actor *model_actor;
	int pixels;

	err = BrBegin();
	if (err != BRE_OK) {
		fprintf(stderr, "BrBegin failed: %d\n", (int)err);
		return 1;
	}

	err = BrDevAddStatic(NULL, BrDrv1SoftPrimBegin, NULL);
	if (err != BRE_OK) {
		fprintf(stderr, "BrDevAddStatic(softprim) failed: %d\n", (int)err);
		BrEnd();
		return 2;
	}

	err = BrDevAddStatic(NULL, BrDrv1SoftRendBegin, NULL);
	if (err != BRE_OK) {
		fprintf(stderr, "BrDevAddStatic(softrend) failed: %d\n", (int)err);
		BrEnd();
		return 3;
	}

	colour = BrPixelmapAllocate(BR_PMT_INDEX_8, 64, 64, NULL, 0);
	if (colour == NULL) {
		fprintf(stderr, "BrPixelmapAllocate failed\n");
		BrEnd();
		return 4;
	}
	colour->origin_x = colour->width / 2;
	colour->origin_y = colour->height / 2;

	depth = BrPixelmapMatch(colour, BR_PMMATCH_DEPTH_16);
	if (depth == NULL) {
		fprintf(stderr, "BrPixelmapMatch(depth) failed\n");
		BrPixelmapFree(colour);
		BrEnd();
		return 5;
	}

	BrZbBegin(colour->type, depth->type);

	material = BrMaterialAllocate("test");
	material->flags = BR_MATF_PRELIT;
	material->colour = BR_COLOUR_RGB(255, 255, 255);
	BrMaterialAdd(material);
	BrMaterialUpdate(material, BR_MATU_ALL);
	BrMaterialUpdate(material, BR_MATU_ALL);

	model = make_triangle_model(material);
	BrModelAdd(model);

	world = setup_scene(model, &camera, &model_actor);

	{
		br_device_pixelmap *colour_pm = (br_device_pixelmap *)colour;
		br_device_pixelmap *depth_pm = (br_device_pixelmap *)depth;

		memset(colour_pm->pm_pixels, 0,
			(size_t)colour_pm->pm_row_bytes * colour_pm->pm_height);
		memset(depth_pm->pm_pixels, 0xff,
			(size_t)depth_pm->pm_row_bytes * depth_pm->pm_height);
	}

	BrZbSceneRenderBegin(world, camera, colour, depth);
	RendererPartSet(BrV1dbRendererQuery(), BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, BRT_NONE);
	RendererPartSet(BrV1dbRendererQuery(), BRT_HIDDEN_SURFACE, 0, BRT_DIVERT_T, BRT_NONE);
	BrZbModelRender(model_actor, model, material, BR_RSTYLE_DEFAULT, OSC_PARTIAL, 0);
	BrZbSceneRenderEnd();

	pixels = count_rendered_pixels(colour, depth);
	fprintf(stderr, "pixels=%d\n", pixels);

	BrZbEnd();
	BrPixelmapFree(depth);
	BrPixelmapFree(colour);
	BrEnd();

	if (pixels == 0)
		return 6;

	fprintf(stderr, "ok\n");
	return 0;
}
