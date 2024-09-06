#include "../include/xmp.h"
#include "test.h"

void new_event(struct context_data *ctx, int pat, int row, int chn,
		int note, int ins, int vol, int fxt, int fxp, int f2t, int f2p)
{
	// nop to shut up linker
}

int main(int argc, char *argv[])
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f = stdout;
	int ret;

	if (argc != 2 && argc != 3) {
		fprintf(stderr, "Usage: gen_module_data module_file [output file]\n");
		return 0;
	}

	if (argc == 3) {
		f = fopen(argv[2], "wb");
		fail_unless(f, "failed to open output file");
	}

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, argv[1]);
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	dump_module(info.mod, f);
	if (f != stdout)
		fclose(f);

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
