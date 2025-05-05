#include "test.h"


TEST(test_fuzzer_prowizard_pha_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the Pha depacker
	 * due to a missing note bounds check. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pha_invalid_note");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note)");

	/* This input caused an out-of-bounds writes in the Pha Packer test
	 * function due to allowing negative pattern offsets and expecting
	 * modulo to return positive values for them. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pha_invalid_offset");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (invalid_offset)");

	/* This input caused signed overflows in the Pha test function due to
	 * a badly constructed bounds check on pattern addresses. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pha_invalid_paddr");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (invalid_paddr)");

	/* This input crashed the ProWizard Pha depacker due to
	 * missing bounding on the pattern count. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pha_invalid_pattern_count");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_pattern_count)");

	/* This input caused an out-of-bounds read in the Pha depacker due to
	 * missing bounds checks when reading from the pattern data array. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pha_invalid_pattern_packing");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_pattern_packing)");

	/* These two inputs caused out-of-bounds reads/writes in the Pha depacker
	 * due to overflowing the ocpt/onote iterator, which for some reason was
	 * initialized off of input data instead of to 0. This combined with
	 * truncated modulo would underflow the ocpt/onote arrays. Reproducing
	 * the crashes with ASan requires leaving UBSan recovery enabled.
	 * CVE-2025-47256 */
	/* pdata[i] != 0xff, k overflows, out-of-bounds reads/writes */
	ret = xmp_load_module(opaque, "data/f/prowizard_pha_invalid_ocpt");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_ocpt)");
	/* pdata[i] == 0xff, k+3 overflows, out-of-bounds write */
	ret = xmp_load_module(opaque, "data/f/prowizard_pha_invalid_ocpt2");
	fail_unless(ret == 0, "module load (invalid_ocpt2)");

	xmp_free_context(opaque);
}
END_TEST
