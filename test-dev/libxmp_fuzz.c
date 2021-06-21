#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "../include/xmp.h"

#ifdef __cplusplus
extern "C"
#endif
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	xmp_context opaque = xmp_create_context();
	FILE *f;

	/* Fuzz loaders. */
	if (xmp_load_module_from_memory(opaque, data, size) == 0)
	{
		/* FIXME fuzz playback. */
		xmp_release_module(opaque);
	}

	/* Fuzz depackers. */
	f = fmemopen((void *)data, size, "rb");
	if (f != NULL)
	{
		struct xmp_test_info info;
		xmp_test_module_from_file(f, &info);
		fclose(f);
	}

	xmp_free_context(opaque);
	return 0;
}
