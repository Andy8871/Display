LOCAL_PATH := $(call my-dir)

# 遍历目录及子目录的函数
define walk
$(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef
####################################### 编译security.so begin ##################################
include $(CLEAR_VARS)
LOCAL_MODULE    := security 
# 遍历source目录
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/Security/source)
# 从所有文件中提取出所有.cpp文件
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/Security/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
#开启编译器的异常捕获
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### 编译security.so end #####################################


####################################### 编译Linklayer.so begin ##################################
include $(CLEAR_VARS)
LOCAL_MODULE    := Linklayer 
# 遍历source目录
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/LinkLayer/source)
# 从所有文件中提取出所有.cpp文件
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/LinkLayer/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
#开启编译器的异常捕获
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### 编译Linklayer.so end #####################################


####################################### 编译DiagnoseBase.so begin ##################################

include $(CLEAR_VARS)

LOCAL_MODULE    := DiagnoseBase 

# 遍历目录及子目录的函数
#define walk
#$(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
#endef

# 遍历source目录
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/DiagnoseBase/source)
# 从所有文件中提取出所有.cpp文件
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
# 替换掉LOCAL_SRC_FILES中的LOCAL_PATH
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/LinkLayer/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Security/include
LOCAL_C_INCLUDES := $(LOCAL_PATH)/DiagnoseBase/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
LOCAL_SHARED_LIBRARIES := security
LOCAL_SHARED_LIBRARIES += Linklayer

#开启编译器的异常捕获
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### 编译DiagnoseBase.so end ##################################

####################################### 编译OBDII.so begin ##################################
include $(CLEAR_VARS)

LOCAL_MODULE    := OBDII 
# 遍历source目录
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/DiagnosisFrame/source)
ALLFILES_CPP += $(call walk, $(LOCAL_PATH)/OBDII/source)


# 从所有文件中提取出所有.cpp文件
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/DiagnosisFrame/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/DiagnoseBase/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/LinkLayer/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/OBDII/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
LOCAL_SHARED_LIBRARIES := DiagnoseBase
#开启编译器的异常捕获
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### 编译OBDII.so end ##################################
