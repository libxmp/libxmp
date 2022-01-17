
DEPACKERS_OBJS	= depacker.o ppdepack.o unsqsh.o mmcmp.o s404_dec.o \
		  arc.o arcfs.o arc_unpack.o lzx.o lzx_unpack.o \
		  muse.o miniz_tinfl.o miniz_zip.o \
		  unzip.o gunzip.o uncompress.o bunzip2.o unlha.o \
		  unxz.o xz_dec_lzma2.o xz_dec_stream.o \
		  crc32.o xfnmatch.o ptpopen.o xfd.o xfd_link.o

DEPACKERS_DFILES = Makefile $(DEPACKERS_OBJS:.o=.c) depacker.h \
		  miniz.h miniz_zip.h arc_unpack.h lzx_unpack.h \
		  xz_lzma2.h README.unxz xz.h xz_private.h \
		  xz_stream.h xz_config.h crc32.h xfnmatch.h ptpopen.h

DEPACKERS_PATH	= src/depackers

DEPACKER_OBJS	= $(addprefix $(DEPACKERS_PATH)/,$(DEPACKERS_OBJS))

default-depackers::
	$(MAKE) -C ..

dist-depackers::
	mkdir -p $(DIST)/$(DEPACKERS_PATH)
	cp -RPp $(addprefix $(DEPACKERS_PATH)/,$(DEPACKERS_DFILES)) $(DIST)/$(DEPACKERS_PATH)

