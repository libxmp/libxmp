#include "test.h"

static const char * const ins_paths[] =
{
	"sdjklfsd",
	"/tmp/libxmp",
	"C:\\jfkd\\dfklsd"
};

TEST(test_api_set_instrument_path)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct module_data *m;
	int ret;
	int i;

	opaque = xmp_create_context();
	fail_unless(opaque != NULL, "failed to create context");

	ctx = (struct context_data *)opaque;
	m = &ctx->m;

	/* Should be unset at startup */
	fail_unless(m->instrument_path == NULL, "instrument path is set");

	/* NULL should do nothing */
	ret = xmp_set_instrument_path(opaque, NULL);
	fail_unless(ret == 0, "failed to clear instrument path (1)");
	fail_unless(m->instrument_path == NULL, "failed to clear instrument path (1)");

	/* set instrument path */
	for (i = 0; i < ARRAY_SIZE(ins_paths); i++) {
		char msg[80];
		snprintf(msg, sizeof(msg), "failed to set path (%d)", i + 1);

		ret = xmp_set_instrument_path(opaque, ins_paths[i]);
		fail_unless(ret == 0, msg);
		fail_unless(m->instrument_path != NULL, msg);
		ret = strcmp(m->instrument_path, ins_paths[i]);
		fail_unless(ret == 0, msg);
	}

	/* NULL should reset the instrument path */
	ret = xmp_set_instrument_path(opaque, NULL);
	fail_unless(ret == 0, "failed to clear instrument path (2)");
	fail_unless(m->instrument_path == NULL, "failed to clear instrument path (2)");

	xmp_free_context(opaque);
}
END_TEST
