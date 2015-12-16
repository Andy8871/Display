LOCAL_PATH := $(call my-dir)

# ����Ŀ¼����Ŀ¼�ĺ���
define walk
$(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef
####################################### ����CommWithSlave.so begin ##################################
include $(CLEAR_VARS)
LOCAL_MODULE    := CommWithSlave 
# ����sourceĿ¼
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/Security/source)
# �������ļ�����ȡ������.cpp�ļ�
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/CommWithSlave/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
#�������������쳣����
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### ����CommWithSlave.so end #####################################


####################################### ����External.so begin ##################################
include $(CLEAR_VARS)
LOCAL_MODULE    := External 
# ����sourceĿ¼
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/LinkLayer/source)
# �������ļ�����ȡ������.cpp�ļ�
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/External/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
#�������������쳣����
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### ����External.so end #####################################
