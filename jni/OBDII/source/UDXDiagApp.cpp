// UDXDiagApp.cpp: implementation of the CUDXDiagApp class.
//
//////////////////////////////////////////////////////////////////////
#include <android/log.h>
#include <fcntl.h>
#include "stdafx.h"
#include "sysstr.h"
#include "RunEnvironment.h"

#include "DiagStd.h"
#include "AutoLog.h"
#include "BasicDiagnosticUnit.h"
#include "VInfo.h"

bool Demo_Flag;
unsigned char bBrand_ID = 0x00;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int DiagnosisMain()
{
	CVInfo VInfoSelect;
//	CRunLog * Log = NULL;
	Demo_Flag = CRunEnvironment::GetDemoMode();

// 	if(Demo_Flag==true)
// 	{
// 		g_pDisplay->MessageBox(CBinary("\x00\xFF\x00\x00\x00\x12",6),CBinary("\x00\xFF\x00\x00\x00\x09",6));
// 		return 1;
// 	}

 /*	if (CRunEnvironment::GetCheckMode())
 	{
 		Log = new CRunLog;
 		if (Log)
 		{
 			g_pCommEcu->SetRunLog(Log);
 			Log=g_pCommEcu->GetRunLog();
 			Log->SetBufSize(0);
 		}
 	}*/
	/*const char* fileName = "/sdcard/Vpecker/scan/log.txt";
	FILE* pFile = fopen(fileName, "rb");
	if (NULL == pFile)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "DiagnosisMain",
							"Unable to open file--->%s", fileName);
	}
	__android_log_print(ANDROID_LOG_DEBUG, "DiagnosisMain",
								"successfully to open file--->%s", fileName);
	char* pBuff = new char[32];
	if (NULL == pBuff)
	{
		__android_log_write(ANDROID_LOG_DEBUG, "DiagnosisMain",
										"Unable to new memory");
		return 1;
	}
	memset(pBuff, 0, 32);
	int nRead = fread(pBuff, sizeof(char), 10, pFile);
	__android_log_print(ANDROID_LOG_DEBUG, "DiagnosisMain",
								"Read data from file--->%s", pBuff);
	if (NULL != pBuff)
	{
		delete []pBuff;
		pBuff = NULL;
	}*/
 	VInfoSelect.StartShow();


	/*char strVehicleID[20];
	 sprintf(strVehicleID, "%02X%02X%02X%02X", 0xAB, 0xBC, 0xCD, 0xDE);*/
	//g_pDisplay->MessageBox(strVehicleID, "Vehicle ID");
	/*g_pDisplay->Menu.Init("test");
	g_pDisplay->Menu.Add("MENU_ITEM1");
	g_pDisplay->Menu.Add("MENU_ITEM2");
	g_pDisplay->Menu.Add("MENU_ITEM3");
	g_pDisplay->Menu.Add("MENU_ITEM4");

	CMenuShow::CMenuStruct mstMenuStruct;
	if (g_pDisplay->Menu.Show(mstMenuStruct) == -1)
		return 1;
	g_pDisplay->Menu.Add("MENU_ITEM3");
	g_pDisplay->Menu.Add("MENU_ITEM4");
	g_pDisplay->Menu.Add("MENU_ITEM6");
	g_pDisplay->Menu.Add("MENU_ITEM5");

	if (g_pDisplay->Menu.Show(mstMenuStruct) == -1)
		return 1;

	g_pDisplay->Menu.Add("MENU_ITEM7");
	g_pDisplay->Menu.Add("MENU_ITEM8");
	g_pDisplay->Menu.Add("MENU_ITEM9");
	g_pDisplay->Menu.Add("MENU_ITEM10");

	if (g_pDisplay->Menu.Show(mstMenuStruct) == -1)
		return 1;*/



	return 1;
}

