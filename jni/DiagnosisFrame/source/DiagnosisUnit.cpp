#include "StdAfx.h"
#include "DiagnosisUnit.h"

#include "RunEnvironment.h"
#include "RunLog.h"
#include "CommWithEcu.h"
extern CCommWithEcu*	g_pCommEcu;
/********************************************************************************
��    �ܣ�	��Ϲ���������� 
����˵����	unsigned short uiWay ������ʽ����Ҫִ�еĹ���
�� �� ֵ��	���ڵ���0��ʾ�ɹ�����ڻ��ȡ�Ĺ����������С��0��ʾ�����ڻ򲻳ɹ�
˵    ����	��
********************************************************************************/
short CDiagnosisUnit::Enter (unsigned short uiWay)
{
	short iRet = 0;
	if (CRunEnvironment::GetCheckMode() == true){
		CRunLog* log = g_pCommEcu->GetRunLog();
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
��    �ܣ�	����ϵͳ��Ϲ��� 
����˵����	��
�� �� ֵ��	�������
˵    ����	����һ��ϵͳ���ģ�ͣ�����������ϵͳ������ϳ�����������Ҫ�����ر�����
********************************************************************************/
short CDiagnosisUnit::NormalDiagnosis (void)
{
	short iRet = 0;

	if ( (iRet = EnterSystem()) )//��ϵͳ���ش�����
		return iRet;

	iRet = MenuAndProcess();
	ExitSystem();

	return iRet;
}


/********************************************************************************
��    �ܣ�	ɨ��ϵͳ�Ƿ����
����˵����	��
�� �� ֵ��	�ɹ���0 ���ɹ������ش������
˵    ����	��
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
    CRunLog* Log4Exit = g_pCommEcu->GetRunLog();
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
		g_pCommEcu->SetRunLog(Log);
	}
} 
////End: by added for Logging Function Optimization 20120712
