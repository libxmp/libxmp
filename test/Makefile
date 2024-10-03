
TEST_OBJS	= test.o
TESTLITE_OBJS	= testlite.o
TEST_DFILES	= Makefile CMakeLists.txt $(TEST_OBJS:.o=.c) $(TESTLITE_OBJS:.o=.c) test.it test.itz test.xm
TEST_PATH	= test

MAIN_OBJS	= md5.o
MAIN_PATH	= src

T_OBJS = $(addprefix $(TEST_PATH)/,$(TEST_OBJS))
T_OBJS += $(addprefix $(MAIN_PATH)/,$(MAIN_OBJS))
TLITE_OBJS = $(addprefix $(TEST_PATH)/,$(TESTLITE_OBJS))
TLITE_OBJS += $(addprefix $(MAIN_PATH)/,$(MAIN_OBJS))

default:
	$(MAKE) -C .. check

dist-test:
	mkdir -p $(DIST)/$(TEST_PATH)
	cp -RPp $(addprefix $(TEST_PATH)/,$(TEST_DFILES)) $(DIST)/$(TEST_PATH)

check: $(TEST_PATH)/libxmp-test
	cd $(TEST_PATH); LD_LIBRARY_PATH=../lib DYLD_LIBRARY_PATH=../lib LIBRARY_PATH=../lib:$$LIBRARY_PATH PATH=$$PATH:../lib ./libxmp-test

$(TEST_PATH)/libxmp-test: $(T_OBJS)
	@CMD='$(LD) $(LDFLAGS) -o $@ $(T_OBJS) $(LIBS) -Llib -lxmp'; \
	if [ "$(V)" -gt 0 ]; then echo $$CMD; else echo LD $@ ; fi; \
	eval $$CMD

check-lite: $(TEST_PATH)/libxmp-lite-test
	cd $(TEST_PATH); LD_LIBRARY_PATH=../lib DYLD_LIBRARY_PATH=../lib LIBRARY_PATH=../lib:$$LIBRARY_PATH PATH=$$PATH:../lib ./libxmp-lite-test

$(TEST_PATH)/libxmp-lite-test: $(TLITE_OBJS)
	@CMD='$(LD) -o $@ $(TLITE_OBJS) $(LIBS) -Llib -lxmp-lite'; \
	if [ "$(V)" -gt 0 ]; then echo $$CMD; else echo LD $@ ; fi; \
	eval $$CMD

