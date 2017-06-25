LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := substrate  
LOCAL_SRC_FILES := libsubstrate.so  
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE    := substrate-dvm  
LOCAL_SRC_FILES := libsubstrate-dvm.so  
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS) 
#这里一定要注意编译的文件名必须是cy结尾的
LOCAL_MODULE    := hookdvm.cy
LOCAL_SRC_FILES := hookdvm.cpp
LOCAL_LDLIBS := -llog 
LOCAL_ARM_MODE := arm
LOCAL_LDLIBS += -L$(LOCAL_PATH) -lsubstrate-dvm -lsubstrate
include $(BUILD_SHARED_LIBRARY)