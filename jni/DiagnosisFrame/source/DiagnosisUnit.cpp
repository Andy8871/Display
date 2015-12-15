#include "StdAfx.h"
#include "DiagnosisUnit.h"

#include "RunEnvironment.h"
#include "RunLog.h"
#include "CommWithEcu.h"
//extern CCommWithEcu*	g_pCommEcu;
/********************************************************************************
功    能：	诊断功能运行入口 
参数说明：	unsigned short uiWay 工作方式，即要执行的功能
返 回 值：	大于等于0表示成功或存在或读取的故障码个数，小于0表示不存在或不成功
说    明：	无
********************************************************************************/
short CDiagnosisUnit::Enter (unsigned short uiWay)
{
	short iRet = 0;
	if (CRunEnvironment::GetCheckMode() == true){
		CRunLog* log = CCommWithEcu::GetInstance()->GetRunLog();
		if (log){
			string temp = "Menu:\n";
			temp += CRunEnvironment::GetMenuChoosedDir();
			log->WriteContent(temp.c_str());
		}
	}
	
	OnEnter (uiWay);
	switch (uiWay) 
	{
		case DUF_NORMAL_DIAGNOSIS:
			iRet = NormalDiagnosis ();
			break;

		case DUF_SCAN_SYSTEM:
			iRet = ScanSystemExist();
			break;

		case DUF_QUICK_READ_TROUBLE_CODE:
			iRet = QuickReadTroubleCode ();
			break;

		case DUF_QUICK_CLEAR_TROUBLE_CODE:
			iRet = QuickClearTroubleCode ();
			break;

		default:
			break;
	}
	
	OnExit (uiWay);

	return iRet;
}


/********************************************************************************
功    能：	完整系统诊断功能 
参数说明：	无
返 回 值：	错误代码
说    明：	这是一个系统诊断模型，适用于所有系统，如诊断程序有特殊需要可重载本函数
********************************************************************************/
short CDiagnosisUnit::NormalDiagnosis (void)
{
	short iRet = 0;

	if ( (iRet = EnterSystem()) )//进系统返回错误码
		return iRet;

	iRet = MenuAndProcess();
	ExitSystem();

	return iRet;
}


/********************************************************************************
功    能：	扫描系统是否存在
参数说明：	无
返 回 值：	成功—0 不成功—返回错误代码
说    明：	无
********************************************************************************/
short CDiagnosisUnit::ScanSystemExist (void)
{
	short iRet = 0;
	if (! (iRet = EnterSystem()) )
		ExitSystem();

	return iRet;
}


//by add for AutoLog when exit a system 2012-3-26
CDiagnosisUnit::~CDiagnosisUnit (void)
{
  if (CRunEnvironment::GetCheckMode() == true)
  {
    CRunLog* Log4Exit = CCommWithEcu::GetInstance()->GetRunLog();
    if (Log4Exit)
    {
      //Log4Exit->WriteContent("",TRUE);// by deleted for Logging Function Optimization 20120712
      Log4Exit->Close();//by added for Logging Function Optimization 20120712
    }
  }
} 

////Begin: by added for Logging Function Optimization 20120712
CDiagnosisUnit::CDiagnosisUnit (void)
{
	CRunLog * Log = NULL;
	if (CRunEnvironment::GetCheckMode())//hpy
	{
		Log = new CRunLog;
		if (Log)
		{
			if (!Log->Init())
			{
				delete Log;
				Log = NULL;
				return ;
			}
		}
		else
			return ;

		Log->WriteMenu();
		CCommWithEcu::GetInstance()->SetRunLog(Log);
	}
} 
////End: by added for Logging Function Optimization 20120712
