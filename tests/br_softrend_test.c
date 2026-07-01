#include <stdio.h>
#include "brender.h"

struct br_device * BR_EXPORT BrDrv1SoftRendBegin(char *arguments);
struct br_device * BR_EXPORT BrDrv1SoftPrimBegin(char *arguments);

int main(void)
{
	br_error err;
	struct br_device *dev;
	struct br_renderer_facility *renderer_facility = NULL;
	struct br_primitive_library *primitive_library = NULL;
	br_int_32 count;

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

	err = BrDevFind(&dev, "SOFTPRMF");
	if (err != BRE_OK || dev == NULL) {
		fprintf(stderr, "SOFTPRMF device not found: %d\n", (int)err);
		BrEnd();
		return 4;
	}

	err = BrDevFind(&dev, "SOFTRNDF");
	if (err != BRE_OK || dev == NULL) {
		fprintf(stderr, "SOFTRNDF device not found: %d\n", (int)err);
		BrEnd();
		return 5;
	}

	err = BrDevCount(&count, "SOFT*");
	if (err != BRE_OK || count < 2) {
		fprintf(stderr, "expected >=2 SOFT* devices, got count=%d err=%d\n", (int)count, (int)err);
		BrEnd();
		return 6;
	}

	err = BrPrimitiveLibraryFind(&primitive_library, NULL, BRT_FLOAT);
	if (err != BRE_OK || primitive_library == NULL) {
		fprintf(stderr, "BrPrimitiveLibraryFind failed: %d\n", (int)err);
		BrEnd();
		return 7;
	}

	err = BrRendererFacilityFind(&renderer_facility, NULL, BRT_FLOAT);
	if (err != BRE_OK || renderer_facility == NULL) {
		fprintf(stderr, "BrRendererFacilityFind failed: %d\n", (int)err);
		BrEnd();
		return 8;
	}

	fprintf(stderr, "softrend devices=%d prim=%p renderer_facility=%p\n",
		(int)count, (void *)primitive_library, (void *)renderer_facility);

	err = BrEnd();
	if (err != BRE_OK) {
		fprintf(stderr, "BrEnd failed: %d\n", (int)err);
		return 9;
	}

	fprintf(stderr, "ok\n");
	return 0;
}
