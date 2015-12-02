#include "stdafx.h"
#ifdef WIN32
#include <tchar.h>
#endif
#include "CommWithEcu.h"
#include "Display.h"
#include "adsStd.h"
#include  <assert.h>
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include "DiagnosisFrame.h"
#include "RunLog.h"
CCommWithEcu* g_pCommEcu = NULL; // ȫ��ͨ�Žӿ�
CDisplay* g_pDisplay = NULL; // ȫ����ʾ�ӿ�
//JavaVM* g_pVM = NULL;
float g_flLibVersion;
void Native_Run(JNIEnv* env, jobject* obj)
{

	//��ʼ���
	try
	{
		__android_log_write(ANDROID_LOG_DEBUG, "DiagMain",
				"Native_Run");
		CDiagnosisFrame theApp;
		theApp.InitInstance(env);
	} catch (...)
	{
		//����ȫ��δ������쳣,�����˳�����iRet��
	}

}
static JNINativeMethod gMethods[] =
{
{ "RunDiagnose", "()V", (void*) Native_Run }
};

void ThrowExceptionToJava(JNIEnv* env, string strNote)
{
	jclass clazzException = env->FindClass("java/lang/RuntimeException");
	env->ThrowNew(clazzException, strNote.c_str());
}

jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	__android_log_write(ANDROID_LOG_DEBUG, "DiagMain",
						"JNI_OnUnload");

	JNIEnv* env = NULL;
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
		return JNI_ERR;
	jclass clazz = env->FindClass("com/TD/Model/Diagnose");
	if (NULL == clazz)
	{
		ThrowExceptionToJava(env, "Unable to find class com/TD/Model/Diagnose");
	}
	env->RegisterNatives(clazz, gMethods,
			sizeof(gMethods) / sizeof(gMethods[0]));
	return JNI_VERSION_1_4;
}

void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
	__android_log_write(ANDROID_LOG_DEBUG, "DiagMain",
					"JNI_OnUnload");
}
/***************************************************************************************
 ����:
 1��	������ϳ�������ܣ�CDiagnosisFrame����ʵ�������������ʼ������
 2��	�������ϵͳȫ�ֱ���CDisplay��CCommEcu��ʵ����
 3��	������Ҫ���ں�̨���У������в�����ʾ�����ڣ�Ҳ�����ڿ�ʼ����������ʾ�������л���ť��
 ***************************************************************************************/
/*int WINAPI WinMain(	HINSTANCE hInstance,
 HINSTANCE hPrevInstance,
 LPTSTR    lpCmdLine,
 int       nCmdShow)
 {

 int iRet = 0;
 //	try {
 CDiagnosisFrame theApp;
 iRet = theApp.InitInstance ();
 //	}
 //	catch(...)
 //	{
 //	����ȫ��δ������쳣,�����˳�����iRet��
 //	}

 return iRet;
 }*/

