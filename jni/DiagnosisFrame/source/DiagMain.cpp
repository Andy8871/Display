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
#include "LogCat.h"
static const char* TAG = "DiagMain";

float g_flLibVersion;

/* 启动诊断程序 */
void Native_Run(JNIEnv* env, jobject obj)
{
	//开始诊断
	try
	{
		LOG_DEBUG(TAG, "Native_Run");
		CDiagnosisFrame theApp;
		theApp.InitInstance(env);
	} catch (...)
	{
		//处理全部未处理的异常,设置退出代码iRet；
	}

}

void Native_Exit(JNIEnv* env, jobject obj)
{

}
static JNINativeMethod gMethods[] =
{
{ "RunDiagnose", "()V", (void*) Native_Run },
{ "ExitDiagnose", "()V", (void*) Native_Exit }
};

void ThrowExceptionToJava(JNIEnv* env, string strNote)
{
	jclass clazzException = env->FindClass("java/lang/RuntimeException");
	env->ThrowNew(clazzException, strNote.c_str());
}

/* 加载JNI库 */
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

/* 卸载JNI库 */
void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
	__android_log_write(ANDROID_LOG_DEBUG, "DiagMain",
					"JNI_OnUnload");
}

