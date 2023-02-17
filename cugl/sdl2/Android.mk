# MAKEFILE FOR WINDOWS
# While we wanted to keep everything in the build directory, Windows sucks
# It cannot handle the long pathnames caused by multiple redirections.
# So we simplify the process by puttin the makefile with the source

LOCAL_PATH := $(call my-dir)
SDL2_PATH := $(LOCAL_PATH)

###########################
#
# SDL shared library
#
###########################

include $(CLEAR_VARS)

LOCAL_MODULE := SDL2

LOCAL_C_INCLUDES := $(SDL2_PATH)/include

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(SDL2_PATH)/src/*.c) \
	$(wildcard $(SDL2_PATH)/src/audio/*.c) \
	$(wildcard $(SDL2_PATH)/src/audio/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/audio/dummy/*.c) \
	$(wildcard $(SDL2_PATH)/src/audio/aaudio/*.c) \
	$(wildcard $(SDL2_PATH)/src/audio/openslES/*.c) \
	$(SDL2_PATH)/src/atomic/SDL_atomic.c.arm \
	$(SDL2_PATH)/src/atomic/SDL_spinlock.c.arm \
	$(wildcard $(SDL2_PATH)/src/core/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/cpuinfo/*.c) \
	$(wildcard $(SDL2_PATH)/src/dynapi/*.c) \
	$(wildcard $(SDL2_PATH)/src/extras/*.c) \
	$(wildcard $(SDL2_PATH)/src/extras/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/events/*.c) \
	$(wildcard $(SDL2_PATH)/src/file/*.c) \
	$(wildcard $(SDL2_PATH)/src/haptic/*.c) \
	$(wildcard $(SDL2_PATH)/src/haptic/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/hidapi/*.c) \
	$(wildcard $(SDL2_PATH)/src/hidapi/android/*.cpp) \
	$(wildcard $(SDL2_PATH)/src/joystick/*.c) \
	$(wildcard $(SDL2_PATH)/src/joystick/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/joystick/hidapi/*.c) \
	$(wildcard $(SDL2_PATH)/src/joystick/virtual/*.c) \
	$(wildcard $(SDL2_PATH)/src/loadso/dlopen/*.c) \
	$(wildcard $(SDL2_PATH)/src/locale/*.c) \
	$(wildcard $(SDL2_PATH)/src/locale/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/misc/*.c) \
	$(wildcard $(SDL2_PATH)/src/misc/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/power/*.c) \
	$(wildcard $(SDL2_PATH)/src/power/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/filesystem/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/sensor/*.c) \
	$(wildcard $(SDL2_PATH)/src/sensor/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/render/*.c) \
	$(wildcard $(SDL2_PATH)/src/render/*/*.c) \
	$(wildcard $(SDL2_PATH)/src/stdlib/*.c) \
	$(wildcard $(SDL2_PATH)/src/thread/*.c) \
	$(wildcard $(SDL2_PATH)/src/thread/pthread/*.c) \
	$(wildcard $(SDL2_PATH)/src/timer/*.c) \
	$(wildcard $(SDL2_PATH)/src/timer/unix/*.c) \
	$(wildcard $(SDL2_PATH)/src/video/*.c) \
	$(wildcard $(SDL2_PATH)/src/video/android/*.c) \
	$(wildcard $(SDL2_PATH)/src/video/yuv2rgb/*.c))

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES
LOCAL_CFLAGS += \
	-Wall -Wextra \
	-Wdocumentation \
	-Wdocumentation-unknown-command \
	-Wmissing-prototypes \
	-Wunreachable-code-break \
	-Wunneeded-internal-declaration \
	-Wmissing-variable-declarations \
	-Wfloat-conversion \
	-Wshorten-64-to-32 \
	-Wunreachable-code-return \
	-Wshift-sign-overflow \
	-Wstrict-prototypes \
	-Wkeyword-macro \

# Warnings we haven't fixed (yet)
LOCAL_CFLAGS += -Wno-unused-parameter -Wno-sign-compare

LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -lGLESv3 -lOpenSLES -llog -landroid

ifeq ($(NDK_DEBUG),1)
    cmd-strip :=
endif

LOCAL_STATIC_LIBRARIES := cpufeatures

include $(BUILD_SHARED_LIBRARY)

###########################
#
# SDL static library
#
###########################

LOCAL_MODULE := SDL2_static

LOCAL_MODULE_FILENAME := libSDL2

LOCAL_LDLIBS :=
LOCAL_EXPORT_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -lGLESv3 -llog -landroid

include $(BUILD_STATIC_LIBRARY)

###########################
#
# SDL main static library
#
###########################

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(SDL2_PATH)/include

LOCAL_MODULE := SDL2_main

LOCAL_MODULE_FILENAME := libSDL2main

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/cpufeatures)
