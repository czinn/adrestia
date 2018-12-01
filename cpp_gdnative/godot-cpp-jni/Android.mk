# Android.mk
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := godot-cpp
LOCAL_CPPFLAGS := -std=c++14
LOCAL_CPP_FEATURES := rtti exceptions

LOCAL_SRC_FILES := $(wildcard $(abspath $(LOCAL_PATH)/../src/core/*.cpp))
LOCAL_SRC_FILES += $(wildcard $(abspath $(LOCAL_PATH)/../src/*.cpp))

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/core
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../godot_headers

include $(BUILD_STATIC_LIBRARY)
