/* Extended Module Player
 * Copyright (C) 1996-2024 Claudio Matsuoka and Hipolito Carraro Jr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "common.h"

/* Process a pattern loop effect with the parameter fxp. A parameter of 0
 * will set the loop target, and a parameter of 1-15 (most formats) or
 * 1-255 (OctaMED) will perform a loop.
 *
 * The compatibility logic for Pattern Loop is complex, so a flow_control
 * argument is taken such that the scan can use this function directly.
 *
 * If the development tests ever start building against effects.c, this
 * can be moved back to effects.c.
 */
void libxmp_process_pattern_loop(struct context_data *ctx,
		struct flow_control *f, int chn, int row, int fxp)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	int *start = &f->loop[chn].start;
	int *count = &f->loop[chn].count;
	int i;

	/* Digital Tracker: only the first E60 or E6x is handled per row. */
	if (HAS_FLOW_MODE(FLOW_LOOP_FIRST_EFFECT) && f->loop_param >= 0) {
		return;
	}
	f->loop_param = fxp;

	/* Scream Tracker 3, Digital Tracker, Octalyser, and probably others
	 * use global loop targets and counts. Later versions of Digital
	 * Tracker use a global target but per-track counts. */
	if (HAS_FLOW_MODE(FLOW_LOOP_GLOBAL_TARGET)) {
		start = &f->loop_start;
	}
	if (HAS_FLOW_MODE(FLOW_LOOP_GLOBAL_COUNT)) {
		count = &f->loop_count;
	}

	if (fxp == 0) {
		/* mark start of loop */
		/* Liquid Tracker: M60 is ignored for channels with count >= 1 */
		if (HAS_FLOW_MODE(FLOW_LOOP_IGNORE_TARGET) && *count >= 1) {
			return;
		}
		*start = row;
		if (HAS_QUIRK(QUIRK_FT2BUGS))
			f->jumpline = row;
	} else {
		/* end of loop */
		if (*start < 0) {
			/* Scream Tracker 3.01b: if SB0 wasn't used, the first
			 * SBx used will set the loop target to its row. */
			if (HAS_FLOW_MODE(FLOW_LOOP_INIT_SAMEROW)) {
				*start = row;
			} else {
				*start = 0;
			}
		}

		if (*count) {
			if (--(*count)) {
				f->loop_dest = *start;
			} else {
				/* S3M and IT: loop termination advances the
				 * loop target past SBx. */
				if (HAS_FLOW_MODE(FLOW_LOOP_END_ADVANCES)) {
					*start = row + 1;
				}
				/* Liquid Tracker cancels any other loop jumps
				 * this row started on loop termination. */
				if (HAS_FLOW_MODE(FLOW_LOOP_END_CANCELS)) {
					f->loop_dest = -1;
				}
				f->loop_active_num--;
			}
		} else {
			/* Modplug Tracker: only begin a loop if no
			 * other channel is currently looping. */
			if (HAS_FLOW_MODE(FLOW_LOOP_ONE_AT_A_TIME)) {
				for (i = 0; i < mod->chn; i++) {
					if (i != chn && f->loop[i].count != 0)
						return;
				}
			}
			*count = fxp;
			f->loop_dest = *start;
			f->loop_active_num++;
		}
	}
}
