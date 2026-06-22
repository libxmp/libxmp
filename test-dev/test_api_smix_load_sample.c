#include <errno.h>
#include "test.h"

#define MD5_8BIT_MONO		"3ef6168240590762450a7cd65deaebfa"
#define MD5_16BIT_MONO		"3d16ad99b4a80ea35719344e4bb5da17"
#define MD5_16BIT_STEREO	"11d9e7f24987932818709ea9bf1c0a57"

static inline void smix_compare_md5(const struct xmp_sample *xxs,
				    const char *digest, const char *desc)
{
	MD5_CTX ctx;
	unsigned char d[16];
	size_t len = xxs->len;
	int ret;

	if (xxs->flg & XMP_SAMPLE_16BIT)
		len <<= 1;
	if (xxs->flg & XMP_SAMPLE_STEREO)
		len <<= 1;

	MD5Init(&ctx);
	MD5Update(&ctx, xxs->data, len);
	MD5Final(d, &ctx);

	ret = compare_md5(d, digest);
	fail_unless(ret == 0, desc);
}

TEST(test_api_smix_load_sample)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct smix_data *smix;
	int ret;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	smix = &ctx->smix;

	ret = xmp_load_module(opaque, "data/mod.loving_is_easy.pp");
	fail_unless(ret == 0, "load module");

	/* try to load sample before initializing */
	ret = xmp_smix_load_sample(opaque, 0, "data/blip.wav");
	fail_unless(ret == -XMP_ERROR_INVALID, "load sample before init");

	xmp_start_smix(opaque, 1, 2);

	/* try to load in invalid slot */
	ret = xmp_smix_load_sample(opaque, 2, "data/blip.wav");
	fail_unless(ret == -XMP_ERROR_INVALID, "load sample in invalid slot");

	ret = xmp_smix_load_sample(opaque, -1, "data/blip.wav");
	fail_unless(ret == -XMP_ERROR_INVALID, "load sample in invalid slot");

	/* try to load non-existent file */
	ret = xmp_smix_load_sample(opaque, 0, "doesnt.exist");
	fail_unless(ret == -XMP_ERROR_SYSTEM, "sample doesn't exist");
	fail_unless(xmp_syserrno() == ENOENT, "errno");

	/* try to load sample with invalid format */
        ret = xmp_smix_load_sample(opaque, 1, "data/mod.loving_is_easy.pp");
	fail_unless(ret == -XMP_ERROR_FORMAT, "invalid format");

	ret = xmp_smix_load_sample(opaque, 0, "data/blip.wav");
	fail_unless(ret == 0, "load sample");
	smix_compare_md5(&smix->xxs[0], MD5_16BIT_MONO, "data/blip.wav MD5");

	/* Double-load should not leak */
	ret = xmp_smix_load_sample(opaque, 0, "data/blip8.wav");
	fail_unless(ret == 0, "load sample");
	smix_compare_md5(&smix->xxs[0], MD5_8BIT_MONO, "data/blip8.wav MD5");

	/* try to load stereo sample */
	ret = xmp_smix_load_sample(opaque, 1, "data/send.wav");
	fail_unless(ret == 0, "load sample (stereo)");
	smix_compare_md5(&smix->xxs[1], MD5_16BIT_STEREO, "data/send.wav MD5");

	ret = xmp_smix_release_sample(opaque, 0);
	fail_unless(ret == 0, "release sample");

	/* Double free sample */
	ret = xmp_smix_release_sample(opaque, 0);
	fail_unless(ret == 0, "release sample (double free)");

	/* Invalid sample number */
	ret = xmp_smix_release_sample(opaque, -1);
	fail_unless(ret == -XMP_ERROR_INVALID, "release sample with invalid number");

	xmp_end_player(opaque);
	xmp_end_smix(opaque);

	/* Try to load sample after deinit */
	ret = xmp_smix_load_sample(opaque, 0, "data/blip.wav");
	fail_unless(ret == -XMP_ERROR_INVALID, "load sample after deinit");

	/* Try to release sample after deinit */
	ret = xmp_smix_release_sample(opaque, 0);
	fail_unless(ret == -XMP_ERROR_INVALID, "release sample after deinit");

	xmp_free_context(opaque);
}
END_TEST
