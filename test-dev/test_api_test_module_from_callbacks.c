#include "test.h"

static unsigned long read_func(void *dest, unsigned long len, unsigned long nmemb, void *priv)
{
	FILE *f = (FILE *)priv;
	return fread(dest, len, nmemb, f);
};

static int seek_func(void *priv, long offset, int whence)
{
	FILE *f = (FILE *)priv;
	return fseek(f, offset, whence);
}

static long tell_func(void *priv)
{
	FILE *f = (FILE *)priv;
	return ftell(f);
}

static const struct xmp_callbacks file_callbacks =
{
	read_func,
	seek_func,
	tell_func
};

static int test_module_from_callbacks_helper(const char *filename, struct xmp_test_info *tinfo)
{
	FILE *f;
	int ret;

	f = fopen(filename, "rb");
	fail_unless(f != NULL, "open file");
	ret = xmp_test_module_from_callbacks((void *)f, file_callbacks, tinfo);
	fclose(f);
	return ret;
}

TEST(test_api_test_module_from_callbacks)
{
	struct xmp_test_info tinfo;
	struct xmp_callbacks t1, t2, t3;
	int ret, err;
	FILE *f;

	/* unsupported format */
	ret = test_module_from_callbacks_helper("data/storlek_01.data", &tinfo);
	fail_unless(ret == -XMP_ERROR_FORMAT, "unsupported format fail");

	/* file too small */
	ret = test_module_from_callbacks_helper("data/sample-16bit.raw", &tinfo);
	fail_unless(ret == -XMP_ERROR_FORMAT, "small file fail");

	/* null data pointer */
	ret = xmp_test_module_from_callbacks(NULL, file_callbacks, &tinfo);
	fail_unless(ret == -XMP_ERROR_SYSTEM, "null data fail");

	f = fopen("data/storlek_05.it", "rb");
	fail_unless(f != NULL, "open file");

	/* null callback */
	t1 = t2 = t3 = file_callbacks;
	t1.read_func = NULL;
	t2.seek_func = NULL;
	t3.tell_func = NULL;
	ret = xmp_test_module_from_callbacks(f, t1, &tinfo);
	fail_unless(ret == -XMP_ERROR_SYSTEM, "null read_func fail");
	ret = xmp_test_module_from_callbacks(f, t2, &tinfo);
	fail_unless(ret == -XMP_ERROR_SYSTEM, "null seek_func fail");
	ret = xmp_test_module_from_callbacks(f, t3, &tinfo);
	fail_unless(ret == -XMP_ERROR_SYSTEM, "null tell_func fail");
	fclose(f);

	/* null info */
	ret = test_module_from_callbacks_helper("data/storlek_05.it", NULL);
	fail_unless(ret == 0, "null info test fail");

	/* XM */
	ret = test_module_from_callbacks_helper("data/xm_portamento_target.xm", &tinfo);
	fail_unless(ret == 0, "XM test module fail");
	fail_unless(strcmp(tinfo.type, "Fast Tracker II") == 0, "XM module type fail");

	/* MOD */
	ret = test_module_from_callbacks_helper("data/ode2ptk.mod", &tinfo);
	fail_unless(ret == 0, "MOD test module fail");
	fail_unless(strcmp(tinfo.name, "Ode to Protracker") == 0, "MOD module name fail");
	fail_unless(strcmp(tinfo.type, "Amiga Protracker/Compatible") == 0, "MOD module type fail");

	/* IT */
	ret = test_module_from_callbacks_helper("data/storlek_01.it", &tinfo);
	fail_unless(ret == 0, "IT test module fail");
	fail_unless(strcmp(tinfo.name, "arpeggio + pitch slide") == 0, "IT module name fail");
	fail_unless(strcmp(tinfo.type, "Impulse Tracker") == 0, "IT module type fail");

	/* S3M (no unpacker for memory) */
	ret = test_module_from_callbacks_helper("data/xzdata", &tinfo);
	fail_unless(ret == -XMP_ERROR_FORMAT, "S3M test module compressed fail");
}
END_TEST
