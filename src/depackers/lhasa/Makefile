
LHASA_OBJS	= ext_header.o lha_file_header.o lha_input_stream.o \
		  lha_decoder.o lha_reader.o lha_basic_reader.o \
		  lh1_decoder.o lh5_decoder.o lh6_decoder.o lh7_decoder.o \
		  lhx_decoder.o lk7_decoder.o lz5_decoder.o lzs_decoder.o \
		  null_decoder.o pm1_decoder.o pm2_decoder.o macbinary.o

LHASA_DFILES	= Makefile README.lhasa $(LHASA_OBJS:.o=.c) \
		  bit_stream_reader.c lh_new_decoder.c pma_common.c tree_decode.c \
		  ext_header.h lha_basic_reader.h lha_decoder.h lha_file_header.h \
		  lha_input_stream.h macbinary.h  lhasa.h

LHASA_PATH	= src/depackers/lhasa

DEPACKER_OBJS	+= $(addprefix $(LHASA_PATH)/,$(LHASA_OBJS))

default: 

dist-lhasa::
	mkdir -p $(DIST)/$(LHASA_PATH)
	cp -RPp $(addprefix $(LHASA_PATH)/,$(LHASA_DFILES)) $(DIST)/$(LHASA_PATH)

