LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := gpio
LOCAL_SRC_FILES := GPIO.c

LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)