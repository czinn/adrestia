# Android.mk
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := godot-cpp
LOCAL_SRC_FILES := ../godot-cpp/obj/local/$(TARGET_ARCH_ABI)/libgodot-cpp.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libadrestia
LOCAL_CPPFLAGS := -std=c++17
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_LDLIBS := -llog

LOCAL_SRC_FILES := $(wildcard $(abspath $(LOCAL_PATH)/../src/*.cpp))
LOCAL_SRC_FILES += $(wildcard $(abspath $(LOCAL_PATH)/../../cpp/*.cc))
LOCAL_SRC_FILES += $(abspath $(LOCAL_PATH)/../../server/protocol.cc)

LOCAL_STATIC_LIBRARIES += godot-cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../godot-cpp/include/core
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../godot-cpp/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../godot_headers
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cpp/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cpp/fdeep-include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cpp/fplus-include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../server/

include $(BUILD_SHARED_LIBRARY)
