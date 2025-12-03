#include "test.h"
#include "../src/effects.h"

/* Note cut, DCA note cut, S70 note cut should produce
 * anticlick decay curves.
 */

TEST(test_mixer_note_cut_ac)
{
	xmp_context opaque;
	struct xmp_frame_info fi;
	struct xmp_event ev;
	FILE *f;
	char line[80];
	int16 *samples;
	int num;
	int i, j;

	f = fopen("data/note_cut_ac.data",
#ifndef GENERATE
		"r");
#else
		"w");
#endif

	opaque = xmp_create_context();

	xmp_load_module(opaque, "data/note_cut_ac.it");
	xmp_start_player(opaque, XMP_MIN_SRATE, XMP_FORMAT_MONO);
	xmp_set_player(opaque, XMP_PLAYER_INTERP, XMP_INTERP_LINEAR);

	/* Speed up module to reduce output size
	 * Decay curves are still apparent in output at this BPM */
	memset(&ev, 0, sizeof(ev));
	ev.fxt = FX_S3M_BPM;
	ev.fxp = 0x7f;
	xmp_inject_event(opaque, 0, &ev);

	for (i = 0; i < 17; i++) {
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &fi);

		samples = (int16 *)fi.buffer;
		num = fi.buffer_size >> 1;

		for (j = 0; j < num; j++) {
#ifndef GENERATE
			char *t = fgets(line, sizeof(line), f);
			int v;
			fail_unless(t, "read failed");
			sscanf(t, "%d", &v);
			fail_unless(v == samples[j], "buffer mismatch");
#else
			fprintf(f, "%d\n", samples[j]);
#endif
		}
	}

#ifndef GENERATE
	fail_unless(fgets(line, sizeof(line), f) == NULL, "not end of file");
	fclose(f);
#else
	fclose(f);
	fail_unless(0, "generate enabled!");
#endif

	xmp_free_context(opaque);
}
END_TEST
