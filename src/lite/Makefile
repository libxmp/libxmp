
# this _must_ match the libxmp-lite package
LITE		= lite-virtual.o lite-format.o lite-period.o lite-player.o lite-read_event.o \
		  lite-misc.o lite-dataio.o lite-lfo.o lite-scan.o lite-control.o lite-filter.o \
		  lite-effects.o lite-mixer.o lite-mix_all.o lite-load_helpers.o lite-load.o \
		  lite-filetype.o lite-hio.o lite-smix.o lite-memio.o lite-rng.o lite-win32.o \
		  lite-flow.o \
		  \
		  lite-common.o lite-itsex.o lite-sample.o \
		  lite-xm_load.o lite-mod_load.o lite-s3m_load.o lite-it_load.o

LITE_OBJS = $(addprefix $(LITE_PATH)/,$(LITE))
LITE_DFILES	= Makefile README.lite \
		  libxmp-lite-config.cmake libxmp-lite-config.cmake.autotools \
		  libxmp-lite-config-version.cmake.autotools.in libxmp-lite.pc.in \
		  $(LITE:.o=.c)
LITE_PATH	= src/lite

default:

dist-lite::
	mkdir -p $(DIST)/$(LITE_PATH)
	cp -RPp $(addprefix $(LITE_PATH)/,$(LITE_DFILES)) $(DIST)/$(LITE_PATH)

