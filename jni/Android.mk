LOCAL_PATH := $(call my-dir)

# 遍历目录及子目录的函数
define walk
$(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef
####################################### 编译CommWithSlave.so begin ##################################
include $(CLEAR_VARS)
LOCAL_MODULE    := CommWithSlave 
# 遍历source目录
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/Security/source)
# 从所有文件中提取出所有.cpp文件
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/CommWithSlave/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
#开启编译器的异常捕获
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### 编译CommWithSlave.so end #####################################


####################################### 编译External.so begin ##################################
include $(CLEAR_VARS)
LOCAL_MODULE    := External 
# 遍历source目录
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/LinkLayer/source)
# 从所有文件中提取出所有.cpp文件
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/External/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
#开启编译器的异常捕获
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### 编译External.so end #####################################
