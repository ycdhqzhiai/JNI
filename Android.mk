LOCAL_PATH := $(call my-dir)
OPENCV_PATH := $(OPENCV_ANDROID_SDK)/sdk/native/jni

LOCAL_CPP_EXTENSION := .cxx .cpp .cc
LOCAL_CPP_FEATURES := rtti exceptions features

## Build static library
include $(CLEAR_VARS)

LOCAL_C_INCLUDES :=    \
  $(LOCAL_PATH)/3rd/mnn/include               \
  $(LOCAL_PATH)/3rd/opencv/include            \
  $(LOCAL_PATH)/src

MY_FILES_PATH   := $(LOCAL_PATH)/src
MY_FILES_SUFFIX := %.cpp %.c %.cc
 
My_All_Files := $(foreach src_path,$(MY_FILES_PATH), $(shell find "$(src_path)" -type f) ) 
My_All_Files := $(My_All_Files:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(My_All_Files)) 
MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(MY_SRC_LIST)

$(warning $(LOCAL_SRC_FILES))

LOCAL_MODULE    := libultraface
LOCAL_LDFLAGS   := -L$(SYSROOT)/usr/lib $(LOCAL_PATH)/3rd/mnn/lib/libMNN.so
LOCAL_LDLIBS    := -L$(SYSROOT)/usr/lib
LOCAL_LDLIBS    += -lm -llog -lc -ldl -lz -Wl,--gc-sections,-ljnigraphics -static-openmp -DPERFORM_TIME
LOCAL_CPPFLAGS  += -ffunction-sections -fdata-sections -fvisibility=hidden -static-openmp -DPERFORM_TIME
LOCAL_CFLAGS    += -ffunction-sections -fdata-sections -fvisibility=hidden -std=c++11 -static-openmp -DPERFORM_TIME 
include $(BUILD_STATIC_LIBRARY)

## Build shared library
include $(CLEAR_VARS)

LOCAL_C_INCLUDES :=    \
  $(LOCAL_PATH)/3rd/mnn/include               \
  $(LOCAL_PATH)/3rd/opencv/include            \
  $(LOCAL_PATH)/src

## Build binary execute file
include $(CLEAR_VARS)

OPENCV_CAMERA_MODULES:=off
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=STATIC

include $(OPENCV_PATH)/OpenCV.mk

LOCAL_C_INCLUDES :=    \
  $(LOCAL_PATH)/3rd/mnn/include               \
  $(LOCAL_PATH)/3rd/opencv/include            \
  $(LOCAL_PATH)/src


LOCAL_LDFLAGS   := -L$(SYSROOT)/usr/lib -L$(LOCAL_PATH)/3rd/opencv/lib \
		   $(LOCAL_PATH)/3rd/mnn/lib/libMNN.so
LOCAL_LDLIBS    += -lm -llog -lc -ldl -lz -Wl,--gc-sections,-ljnigraphics -Wl,-Bstatic -lomp -Wl,-Bdynamic \
		   -ltegra_hal -ltbb -littnotify -lIlmImf -llibpng -llibtiff -llibwebp -llibopenjp2 -llibjpeg-turbo
LOCAL_CPPFLAGS  += -ffunction-sections -fdata-sections -fvisibility=hidden -Wl,-Bstatic -lomp -Wl,-Bdynamic
LOCAL_CFLAGS    += -ffunction-sections -fdata-sections -fvisibility=hidden -std=c++11

LOCAL_STATIC_LIBRARIES := libtegra_hal libtbb libittnotify libIlmImf libpng libtiff libwebp libopenjp2 libjpeg-turbo \
		libopencv_highgui libopencv_imgcodecs libopencv_imgproc libopencv_core libultraface

LOCAL_SRC_FILES	:= test/main.cpp
LOCAL_MODULE	:= main_slim
include $(BUILD_EXECUTABLE)

APP_ABI := arm64-v8a