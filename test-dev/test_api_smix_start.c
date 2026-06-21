#include "test.h"

TEST(test_api_smix_start)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct smix_data *smix;
	int ret;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	smix = &ctx->smix;

	/* Invalid channel count */
	ret = xmp_start_smix(opaque, 0, 2);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid channel count");
	ret = xmp_start_smix(opaque, -1, 2);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid channel count");

	/* Invalid sample count */
	ret = xmp_start_smix(opaque, 1, -1);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid sample count");

	/* Correct arguments */
	ret = xmp_start_smix(opaque, 1, 2);
	fail_unless(ret == 0, "valid arguments");

	xmp_end_smix(opaque);
	/* Double free should be safe */
	xmp_end_smix(opaque);

	/* Start smix after deinit should work without issues. */
	ret = xmp_start_smix(opaque, 1, 2);
	fail_unless(ret == 0, "init after deinit");
	xmp_end_smix(opaque);

	/* Sample count of zero should succeed but report 0 ins/smp. */
	ret = xmp_start_smix(opaque, 1, 0);
	fail_unless(ret == 0, "init with 0 samples");
	fail_unless(smix->chn == 1, "should be 1");
	fail_unless(smix->ins == 0, "should be 0");
	fail_unless(smix->smp == 0, "should be 0");
	fail_unless(smix->xxi != NULL, "should be allocated");
	fail_unless(smix->xxs != NULL, "should be allocated");
	/* Shouldn't leak the instrument/sample pointers. */
	xmp_end_smix(opaque);

	/* Double init should succeed, destroy the prior initialization.
	 * Previously, this overwrote and leaked the old state. */
	ret = xmp_start_smix(opaque, 1, 2);
	fail_unless(ret == 0, "init after deinit");

	ret = xmp_start_smix(opaque, 2, 3);
	fail_unless(ret == 0, "init after init");
	fail_unless(smix->chn == 2, "should now be 2");
	fail_unless(smix->ins == 3, "should now be 3");
	fail_unless(smix->smp == 3, "should now be 3");

	/* Don't leak when corresponding xmp_end_smix call is missing. */
	xmp_free_context(opaque);
}
END_TEST
