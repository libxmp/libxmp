#include "test.h"

/* This input caused an out-of-bounds read in the Heatseeker depacker
 * due to a broken bounds check. Only occurs when using xmp_load_module_from_memory.
 */

TEST(test_fuzzer_prowizard_heatseek_truncated)
{
	xmp_context opaque;
	void *buffer;
	long len;
	int ret;

	read_file_to_memory("data/f/prowizard_heatseek_truncated", &buffer, &len);
	fail_unless(buffer != NULL, "buffer alloc");

	opaque = xmp_create_context();
	ret = xmp_load_module_from_memory(opaque, buffer, len);
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
	free(buffer);
}
END_TEST
