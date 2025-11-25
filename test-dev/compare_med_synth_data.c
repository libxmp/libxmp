#include "test.h"
#undef TEST
#include "../src/player.h"
#include "../src/mixer.h"
#include "../src/virtual.h"

void compare_med_synth_data(const char *mod, const char *data)
{
	xmp_context opaque;
	struct xmp_frame_info info;
	struct xmp_channel_info *ci;
	struct context_data *ctx;
	struct player_data *p;
	int time, row, frame, chan, period, volume, ins, pan, smp;
	char line[200];
	char *ret;
	FILE *f;
	int max_channels;
	int voc;
	int i, j;

#ifndef MIXER_GENERATE
	f = fopen(data, "r");
#else
	f = fopen(data, "w");
#endif

	opaque = xmp_create_context();
	xmp_load_module(opaque, mod);
	xmp_start_player(opaque, 44100, 0);

	ctx = (struct context_data *)opaque;
	p = &ctx->p;

	max_channels = p->virt.virt_channels;

	for (i = 0; i < 500; i++) {
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &info);
		if (info.loop_count) {
			break;
		}

		for (j = 0; j < max_channels; j++) {
			struct channel_data *xc = &p->xc_data[j];
			ci = &info.channel_info[j];

			voc = map_channel(p, j);
			if (voc < 0 || TEST_NOTE(NOTE_SAMPLE_END))
				continue;

#ifndef MIXER_GENERATE
			ret = fgets(line, 200, f);
			fail_unless(ret == line, "read error");
			sscanf(line, "%d %d %d %d %d %d %d %d %d",
					&time, &row, &frame, &chan, &period,
					&volume, &ins, &pan, &smp);

			fail_unless(info.time  == time,   "time mismatch");
			fail_unless(info.row   == row,    "row mismatch");
			fail_unless(info.frame == frame,  "frame mismatch");
			fail_unless(j == chan,            "channel mismatch");
			fail_unless(ci->period == period, "period mismatch");
			fail_unless(ci->volume == volume, "volume mismatch");
			fail_unless(ci->instrument == ins,"instrument mismatch");
			fail_unless(ci->pan == pan,       "pan mismatch");
			fail_unless(ci->sample == smp,    "sample mismatch");
#else
			fprintf(f, "%d %d %d %d %d %d %d %d %d\n",
				info.time, info.row, info.frame, j,
				ci->period, ci->volume, ci->instrument,
				ci->pan, ci->sample);
#endif
		}
	}

#ifndef MIXER_GENERATE
	ret = fgets(line, 200, f);
	fail_unless(ret == NULL && feof(f), "not end of data file");
	fclose(f);
#else
	fclose(f);
	fail_unless(0, "generate enabled!");
#endif

	xmp_end_player(opaque);
	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
