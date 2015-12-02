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
CCommWithEcu* g_pCommEcu = NULL; // 全局通信接口
CDisplay* g_pDisplay = NULL; // 全局显示接口
//JavaVM* g_pVM = NULL;
float g_flLibVersion;
void Native_Run(JNIEnv* env, jobject* obj)
{

	//开始诊断
	try
	{
		__android_log_write(ANDROID_LOG_DEBUG, "DiagMain",
				"Native_Run");
		CDiagnosisFrame theApp;
		theApp.InitInstance(env);
	} catch (...)
	{
		//处理全部未处理的异常,设置退出代码iRet；
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
 功能:
 1、	建立诊断程序主框架（CDiagnosisFrame）的实例，并运行其初始化程序。
 2、	建立诊断系统全局变量CDisplay及CCommEcu的实例，
 3、	本程序要求在后台运行，界面中不能显示出窗口，也不能在开始任务条中显示出程序切换按钮。
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
 //	处理全部未处理的异常,设置退出代码iRet；
 //	}

 return iRet;
 }*/

