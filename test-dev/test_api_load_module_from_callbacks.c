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

TEST(test_api_load_module_from_callbacks)
{
	xmp_context ctx;
	struct xmp_callbacks t1, t2, t3;
	FILE *f;
	int state, ret;

	ctx = xmp_create_context();

	f = fopen("data/test.it", "rb");
	fail_unless(f != NULL, "open file");

	/* null data pointer */
	ret = xmp_load_module_from_callbacks(ctx, NULL, file_callbacks);
	fail_unless(ret == -XMP_ERROR_SYSTEM, "null data fail");

	/* null callback */
	t1 = t2 = t3 = file_callbacks;
	t1.read_func = NULL;
	t2.seek_func = NULL;
	t3.tell_func = NULL;
	ret = xmp_load_module_from_callbacks(ctx, f, t1);
	fail_unless(ret == -XMP_ERROR_SYSTEM, "null read_func fail");
	ret = xmp_load_module_from_callbacks(ctx, f, t2);
	fail_unless(ret == -XMP_ERROR_SYSTEM, "null seek_func fail");
	ret = xmp_load_module_from_callbacks(ctx, f, t3);
	fail_unless(ret == -XMP_ERROR_SYSTEM, "null tell_func fail");

	/* load */
	ret = xmp_load_module_from_callbacks(ctx, (void *)f, file_callbacks);
	fclose(f);
	fail_unless(ret == 0, "load file");

	state = xmp_get_player(ctx, XMP_PLAYER_STATE);
	fail_unless(state == XMP_STATE_LOADED, "state error");

	/* unload */
	xmp_release_module(ctx);

	state = xmp_get_player(ctx, XMP_PLAYER_STATE);
	fail_unless(state == XMP_STATE_UNLOADED, "state error");

	xmp_free_context(ctx);
}
END_TEST
