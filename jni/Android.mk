LOCAL_PATH	:= $(call my-dir)/..

include $(CLEAR_VARS)

# full library:
include $(LOCAL_PATH)/src/Makefile
include $(LOCAL_PATH)/src/loaders/Makefile
include $(LOCAL_PATH)/src/loaders/prowizard/Makefile
include $(LOCAL_PATH)/src/depackers/Makefile
include $(LOCAL_PATH)/src/depackers/lhasa/Makefile

SRC_SOURCES	:= $(addprefix src/,$(SRC_OBJS))
LOADERS_SOURCES := $(addprefix src/loaders/,$(LOADERS_OBJS))
PROWIZ_SOURCES	:= $(addprefix src/loaders/prowizard/,$(PROWIZ_OBJS))
LHASA_SOURCES := $(addprefix src/depackers/lhasa/,$(LHASA_OBJS))
DEPACKERS_SOURCES := $(addprefix src/depackers/,$(DEPACKERS_OBJS))

LOCAL_MODULE    := xmp
LOCAL_CFLAGS	:= -O3 -DHAVE_MKSTEMP -DHAVE_FNMATCH -DHAVE_DIRENT -DHAVE_POWF \
		   -I$(LOCAL_PATH)/include
LOCAL_SRC_FILES := $(SRC_SOURCES:.o=.c) \
		   $(LOADERS_SOURCES:.o=.c) \
		   $(PROWIZ_SOURCES:.o=.c) \
		   $(LHASA_SOURCES:.o=.c) \
		   $(DEPACKERS_SOURCES:.o=.c)

include $(BUILD_STATIC_LIBRARY)

# lite library:
include $(CLEAR_VARS)

include $(LOCAL_PATH)/src/lite/Makefile
LITE_SOURCES	:= $(addprefix src/lite/,$(LITE))
LOCAL_MODULE    := xmp-lite
LOCAL_CFLAGS	:= -O3 -DHAVE_MKSTEMP -DHAVE_FNMATCH -DHAVE_DIRENT -DHAVE_POWF \
		   -DLIBXMP_CORE_PLAYER \
		   -I$(LOCAL_PATH)/include
LOCAL_SRC_FILES := $(LITE_SOURCES:.o=.c)

include $(BUILD_STATIC_LIBRARY)
