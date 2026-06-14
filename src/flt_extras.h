#ifndef XMP_FLT_EXTRAS_H
#define XMP_FLT_EXTRAS_H

#include "common.h"

#define FLT_EXTRAS_MAGIC 0xea0824ad

enum flt_env_stage {
		FLT_ATTACK_1,
		FLT_ATTACK_2,
		FLT_DECAY,
		FLT_SUSTAIN,
		FLT_RELEASE
};

struct flt_instrument_extras {
	uint32 magic;
	uint16 l0;
	uint16 a1l;
	uint16 a1s;
	uint16 a2l;
	uint16 a2s;
	uint16 sl;
	uint16 ds;
	uint16 st;
	uint16 rs;
	int16  p_fall;
	uint16 fq;
};

struct flt_channel_extras {
	uint32 magic;
	int volume;
	int sustain;
	enum flt_env_stage env_stage;
};

struct flt_module_extras {
	uint32 magic;
};

#define FLT_INSTRUMENT_EXTRAS(x) ((struct flt_instrument_extras *)(x).extra)
#define HAS_FLT_INSTRUMENT_EXTRAS(x) \
	(FLT_INSTRUMENT_EXTRAS(x) != NULL && \
	 FLT_INSTRUMENT_EXTRAS(x)->magic == FLT_EXTRAS_MAGIC)

#define FLT_CHANNEL_EXTRAS(x) ((struct flt_channel_extras *)(x).extra)
#define HAS_FLT_CHANNEL_EXTRAS(x) \
	(FLT_CHANNEL_EXTRAS(x) != NULL && \
	 FLT_CHANNEL_EXTRAS(x)->magic == FLT_EXTRAS_MAGIC)

#define FLT_MODULE_EXTRAS(x) ((struct flt_module_extras *)(x).extra)
#define HAS_FLT_MODULE_EXTRAS(x) \
	(FLT_MODULE_EXTRAS(x) != NULL && \
	 FLT_MODULE_EXTRAS(x)->magic == FLT_EXTRAS_MAGIC)

LIBXMP_BEGIN_DECLS

void libxmp_flt_play_extras(struct context_data *, struct channel_data *, int);
int  libxmp_flt_new_instrument_extras(struct xmp_instrument *);
int  libxmp_flt_new_channel_extras(struct channel_data *);
void libxmp_flt_reset_channel_extras(struct channel_data *);
void libxmp_flt_release_channel_extras(struct channel_data *);
int  libxmp_flt_new_module_extras(struct module_data *);
void libxmp_flt_release_module_extras(struct module_data *);

LIBXMP_END_DECLS

#endif /* XMP_FLT_EXTRAS_H */
