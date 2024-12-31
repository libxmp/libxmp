#include "test.h"

TEST(test_fuzzer_it_truncated)
{
	xmp_context opaque;
	void *buffer;
	long size;
	int ret;

	opaque = xmp_create_context();

	/* This input caused uninitialized reads in the IT loader due to not
	 * checking for EOFs when loading the IT channel tables from the header.
	 * This needs to be tested using xmp_load_module_from_memory.
	 */
	read_file_to_memory("data/f/load_it_truncated_header.it", &buffer, &size);
	fail_unless(buffer != NULL, "read file to memory");

	ret = xmp_load_module_from_memory(opaque, buffer, size);
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");
	free(buffer);
	buffer = NULL;

	/* This input caused hangs in the IT loader due to missing EOF
	 * checks during the pattern scan and pattern loading loops.
	 */
	ret = xmp_load_module(opaque, "data/f/load_it_truncated_pattern.it");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
