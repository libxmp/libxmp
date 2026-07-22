#include "test.h"

TEST(test_loader_flt8)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	void *buffer;
	long size;
	int ret;

	f = fopen("data/format_flt8.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/m/Gidion_Graveland.mod");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);

	/* This format supports optional external files.
	 * Failing to open an external file should not cause a load failure.
	 */
	read_file_to_memory("data/m/Gidion_Graveland.mod", &buffer, &size);
	fail_unless(buffer != NULL, "read file to memory");
	ret = xmp_load_module_from_memory(opaque, buffer, size);
	free(buffer);
	fail_unless(ret == 0, "module load (memory)");

	xmp_get_module_info(opaque, &info);

	rewind(f);
	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded (memory)");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
