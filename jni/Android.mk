LOCAL_PATH := $(call my-dir)

# ����Ŀ¼����Ŀ¼�ĺ���
define walk
$(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef
####################################### ����security.so begin ##################################
include $(CLEAR_VARS)
LOCAL_MODULE    := security 
# ����sourceĿ¼
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/Security/source)
# �������ļ�����ȡ������.cpp�ļ�
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/Security/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
#�������������쳣����
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### ����security.so end #####################################


####################################### ����Linklayer.so begin ##################################
include $(CLEAR_VARS)
LOCAL_MODULE    := Linklayer 
# ����sourceĿ¼
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/LinkLayer/source)
# �������ļ�����ȡ������.cpp�ļ�
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/LinkLayer/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
#�������������쳣����
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### ����Linklayer.so end #####################################


####################################### ����DiagnoseBase.so begin ##################################

include $(CLEAR_VARS)

LOCAL_MODULE    := DiagnoseBase 

# ����Ŀ¼����Ŀ¼�ĺ���
#define walk
#$(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
#endef

# ����sourceĿ¼
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/DiagnoseBase/source)
# �������ļ�����ȡ������.cpp�ļ�
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
# �滻��LOCAL_SRC_FILES�е�LOCAL_PATH
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/LinkLayer/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Security/include
LOCAL_C_INCLUDES := $(LOCAL_PATH)/DiagnoseBase/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
LOCAL_SHARED_LIBRARIES := security
LOCAL_SHARED_LIBRARIES += Linklayer

#�������������쳣����
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### ����DiagnoseBase.so end ##################################

####################################### ����OBDII.so begin ##################################
include $(CLEAR_VARS)

LOCAL_MODULE    := OBDII 
# ����sourceĿ¼
ALLFILES_CPP := $(call walk, $(LOCAL_PATH)/DiagnosisFrame/source)
ALLFILES_CPP += $(call walk, $(LOCAL_PATH)/OBDII/source)


# �������ļ�����ȡ������.cpp�ļ�
FILE_LIST_CPP := $(filter %.cpp, $(ALLFILES_CPP))
LOCAL_SRC_FILES := $(FILE_LIST_CPP:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/DiagnosisFrame/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/DiagnoseBase/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/LinkLayer/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/OBDII/include
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -landroid
LOCAL_SHARED_LIBRARIES := DiagnoseBase
#�������������쳣����
LOCAL_CPPFLAGS := -fexceptions

include $(BUILD_SHARED_LIBRARY)
####################################### ����OBDII.so end ##################################
