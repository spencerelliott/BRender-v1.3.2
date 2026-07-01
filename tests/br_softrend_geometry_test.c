#include <stdio.h>
#include <string.h>
#include "brender.h"

struct br_device * BR_EXPORT BrDrv1SoftRendBegin(char *arguments);
struct br_device * BR_EXPORT BrDrv1SoftPrimBegin(char *arguments);

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

	BrModelUpdate(model, BR_MODU_ALL);
	return model;
}

int main(void)
{
	br_error err;
	br_pixelmap *colour;
	br_pixelmap *depth;
	br_material *material;
	br_model *model;
	br_renderer *renderer;
	void *prepared;

	err = BrBegin();
	if (err != BRE_OK)
		return 1;

	err = BrDevAddStatic(NULL, BrDrv1SoftPrimBegin, NULL);
	if (err != BRE_OK) {
		BrEnd();
		return 2;
	}

	err = BrDevAddStatic(NULL, BrDrv1SoftRendBegin, NULL);
	if (err != BRE_OK) {
		BrEnd();
		return 3;
	}

	colour = BrPixelmapAllocate(BR_PMT_INDEX_8, 64, 64, NULL, 0);
	if (colour == NULL) {
		BrEnd();
		return 4;
	}

	depth = BrPixelmapMatch(colour, BR_PMMATCH_DEPTH_16);
	if (depth == NULL) {
		BrPixelmapFree(colour);
		BrEnd();
		return 5;
	}

	BrZbBegin(colour->type, depth->type);

	renderer = BrV1dbRendererQuery();
	if (renderer == NULL) {
		fprintf(stderr, "renderer missing\n");
		BrZbEnd();
		BrPixelmapFree(depth);
		BrPixelmapFree(colour);
		BrEnd();
		return 6;
	}

	material = BrMaterialAllocate("test");
	material->flags = BR_MATF_PRELIT;
	BrMaterialAdd(material);
	BrMaterialUpdate(material, BR_MATU_ALL);
	BrMaterialUpdate(material, BR_MATU_ALL);

	model = make_triangle_model(material);
	BrModelAdd(model);

	prepared = model->prepared;
	if (prepared == NULL || model->nfaces != 1) {
		fprintf(stderr, "model prep failed prepared=%p nfaces=%d\n",
			prepared, model->nfaces);
		BrZbEnd();
		BrPixelmapFree(depth);
		BrPixelmapFree(colour);
		BrEnd();
		return 7;
	}

	fprintf(stderr, "prepared=%p nfaces=%d renderer=%p material_stored=%p\n",
		prepared, model->nfaces,
		(void *)renderer, (void *)material->stored);

	BrZbEnd();
	BrPixelmapFree(depth);
	BrPixelmapFree(colour);
	BrEnd();

	fprintf(stderr, "ok\n");
	return 0;
}
