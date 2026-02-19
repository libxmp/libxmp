#include "test.h"

TEST(test_fuzzer_gal5_truncated)
{
	xmp_context opaque;
	void *buffer;
	long size;
	int ret;

	opaque = xmp_create_context();

	/* This input caused UMRs in the Galaxy 5.0 loader due to
	 * not checking the hio_read return value for the module title. */
	ret = xmp_load_module(opaque, "data/f/load_gal5_truncated");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	/* This input caused uninitialized reads in the Galaxy 5.0 loader due to
	 * a missing return value check when reading the channel array in the
	 * INIT chunk. Requires testing using xmp_load_module_from_memory. */
	read_file_to_memory("data/f/load_gal5_truncated_init", &buffer, &size);
	fail_unless(buffer != NULL, "read file to memory");
	ret = xmp_load_module_from_memory(opaque, buffer, size);
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (init)");
	free(buffer);

	/* This input caused UMRs in the Galaxy 5.0 loader due to
	 * reading uninitialized channel panning data after failing to read
	 * the INIT chunk (in this example, due to an invalid size).
	 * This edge case relies on quirks of the IFF loader. */
	ret = xmp_load_module(opaque, "data/f/load_gal5_truncated_init_2");
	fail_unless(ret == 0, "module load (init_2)");

	xmp_free_context(opaque);
}
END_TEST
