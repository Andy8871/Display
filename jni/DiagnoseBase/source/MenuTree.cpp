/******************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�MenuTree.cpp
	�ĵ���ʶ��007�������ƽ̨��ϸ���˵����
	ժ    Ҫ�����ڸ��Ӳ˵�����ʾ���˵����ࣨCMenuTree����������ʾ�����˵�ʱʹ�ã�
			�˵������ݿ��԰���ʾ���ݷֳɶ�����ÿһ���˵���������Щ����е���
			��һ����ͨ���˵����ɹ������ɲ˵���(MENU.DB)������ͨ����ͬ�ĸ������
			ֵ�Ĳ�ͬ������ͬһ��ϳ�ϵ��ʹ�ö���˵����������˵��ӿں������ʹ
			�á��˵�������Ƕ�׵��á�


	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2005.12.15  zhangsh		1.0			�������ࡣ                                           

******************************************************************************/

#include "menutree.h"
#include "Database.h"
#include "display.h"

#include "SecurityBaseClass.h"
#include "RunEnvironment.h"
#include "Information.h"

#include "NewVsIndicator.h"


//define errors
#define NOMENUFILE	    	-1
#define ERRNODEMENU	    	-2
#define ERRNEXTIDNUM	    -3
#define ERRENTRYMENUID	    -4
#define ERRCALLBACKRESULT	-5

extern CDisplay * g_pDisplay;

#if 1

//by DEBUG FOR MENU DISPLAY TIME
unsigned long m_TimeStart = 0;
unsigned long m_TimeCost = 0;
FILE * m_fp = NULL;

//by DEBUG FOR MENU DISPLAY TIME
void LogTimeInit(void);
void GetTimeStart(void);
void GetTimeCost(void);
void LogTimeCost(const string strLog);
void LogTimeDestroy(void);
#endif


CMenuTree::CMenuTree(void)
{
	m_pfnTask = NULL;
	m_strFileMenuDb = "menu.db";//hpy : m_strFileMenuDb = "NFOV.PNT";//

	LogTimeInit();//by DEBUG FOR MENU DISPLAY TIME
}

CMenuTree::CMenuTree(string strMenuDbFileName)
{
	m_pfnTask = NULL;
	m_strFileMenuDb = strMenuDbFileName;	
}

CMenuTree::~CMenuTree(void)
{
	LogTimeDestroy();//by DEBUG FOR MENU DISPLAY TIME
}

/************************************************************
��    �ܣ���ʾ�˵���
����˵����CBinary idMenu �˵����ID
�� �� ֵ���������
˵    ����1�����˵���������ε����¼��˵���
		  2������ǵ����һ���˵���ִ��������������绹Ҫ
		     ������һ��ʱ����ʾ���һ����
		  3�������������������趨�Ļص���������������ID��
		  4���ص���������ֵ˵��
								 0��������һ���˵���
								-1����ʾ�����˵���
								-2����ʾ��һ���˵�
************************************************************/
int CMenuTree::Org_ShowMenu (CBinary binIDMenu,unsigned char byFlag)
{
	CMenuLevel menuLevel;
	CDatabase  dataFile;
	CBinary	   idMenu = binIDMenu;

	vector<CBinary> vctBinCurMenuItem;		// ��ǰ�˵��б�
	vector<CBinary> vctBinResult;			// ��ⷵ�ص���������
		

	int iSelected        = 0;			    // �˵���ѡ�б��
	short iTaskMarkNum = 0;
		
	bool m_bExecuteTask  = true;
	
//	string strFileMenuDb = "menu.db";

	// ��ʼ�˵��������
	while(true)
	{
		vctBinCurMenuItem.clear();  // ��յ�ǰ�˵��б�
		vctBinResult.clear();       // ��ղ˵���ѡ�б��

		LogTimeCost("Run here 1 = ");//by for debug

		if(!dataFile.Open(m_strFileMenuDb))
		{
			return NOMENUFILE;
		}

		LogTimeCost("Run here 2 = ");//by for debug

		vctBinResult = dataFile.SearchId(idMenu);

		LogTimeCost("Run here 3 = ");//by for debug
		
		if(vctBinResult.empty()) 
		{
			return ERRNODEMENU;
		}

		dataFile.Close();

#ifndef _TASKIDTYPE
		unsigned short iTaskID	   = (vctBinResult[1])[0]*256 + (unsigned char)(vctBinResult[1])[1];  // ��������ID
#else
		unsigned int iTaskID	   = (vctBinResult[1])[0]*256*256*256 + (vctBinResult[1])[1]*256*256+(vctBinResult[1])[2]*256+(unsigned char)(vctBinResult[1])[3];  // ��������ID
#endif
		unsigned short iNextNumber = (vctBinResult[2])[0]*256 + (unsigned char)(vctBinResult[2])[1];  // ���в˵���

		// ���������������ִ����������
		if(iTaskID > 0 && m_bExecuteTask)
		{
			if(!IsTaskIdOk(iTaskID))
			{
				menuLevel = stackMenuLevel.top();
				idMenu = menuLevel.idMenu;
				
				stackMenuLevel.pop();
				continue;
			}

			if(m_pfnTask != NULL)
			{
				iTaskMarkNum = (*m_pfnTask)(iTaskID, vctBinResult[0]);
			}
			else
			{
				iTaskMarkNum = -1;
				m_bExecuteTask = true;  // ��Ҫִ������ID
			}

			if(-1 == iTaskMarkNum) // ��ʾ����
			{
				m_bExecuteTask = true;  // ��Ҫִ������ID
			}			
		    else if(0 == iTaskMarkNum )  // ��������һ���˵����绹Ҫ������һ��ʱ����ʾ���һ��
			{
				if(0 == iNextNumber)
				{
					iTaskMarkNum = -1;
					m_bExecuteTask = false;  // ����Ҫִ������ID
				}
			}
			
			if(-1 == iTaskMarkNum) // ��ʾ����
			{
				if(stackMenuLevel.empty())
				{					
					return ERRENTRYMENUID;  // ���ش������ڲ˵�ID
				}	

				menuLevel = stackMenuLevel.top();
				idMenu = menuLevel.idMenu;

				stackMenuLevel.pop();
				continue;
			}
			else if(iTaskMarkNum == -2)  // ��ʾ�ϼ�
			{
				if(stackMenuLevel.empty()) // ���ش������ڲ˵�ID
				{
					return ERRENTRYMENUID;
				}

				stackMenuLevel.pop();
				if(stackMenuLevel.empty())
				{
					return 0;
				}

				menuLevel = stackMenuLevel.top();
				idMenu = menuLevel.idMenu;

				stackMenuLevel.pop();
				continue;
			}
		}

		LogTimeCost("Run here 4 = ");//by for debug
		
		// ���û�����в˵�(�Ѿ�����ײ�˵�)
		if(iNextNumber == 0)
		{
			// ��ȡ��ջ���ݣ��ٵ�ջ
			if(!stackMenuLevel.empty())
			{
				menuLevel = stackMenuLevel.top();
				idMenu = menuLevel.idMenu;

				iTaskMarkNum = -1;  // ��Ȼ��ʾ����
				
				m_bExecuteTask = false;  // ����ǰ���Ѿ�ִ��TaskID�����ٴ���ʾʱ����ִ��TaskID

				stackMenuLevel.pop();
				continue;
			}
			else // ���ջΪ�գ���˵���ǵ�һ�ν��룬�������в˵�
			{
				// ���ش������ڲ˵�ID
				return ERRENTRYMENUID;
			}
		}

		LogTimeCost("Run here 5 = ");//by for debug

		// ��������ʾ���в˵�
		CBinary menuTextId	= vctBinResult[0];
		g_pDisplay->Menu.Init(menuTextId);
		CBinary binWork;
		if(iNextNumber > 0)
		{
			binWork = vctBinResult[3];
		}
	
		char* chBuffer = (char*)binWork.GetBuffer();

		for(int i=0; i<iNextNumber; i++)
		{
			CBinary bNextID;			
			for(int iIndex = i*6; iIndex < i*6+6; iIndex++)
			{
				bNextID += chBuffer[iIndex];
				//bNextID += binWork.GetAt(iIndex);
			}
			
			vctBinCurMenuItem.push_back(bNextID);
		}

		// ȡ�����в˵���Ӧ���ı�ID
		if(!dataFile.Open(m_strFileMenuDb))
		{
			return NOMENUFILE;
		}

		vector<CBinary>::size_type nNext = vctBinCurMenuItem.size();

		LogTimeCost("Run here 6 = ");//by for debug

		for(vector<CBinary>::size_type j = 0; j < nNext; j++)
		{
			CBinary NextTextId;

			LogTimeCost("Run here 6.1 = ");//by for debug

			vctBinResult = dataFile.SearchId(vctBinCurMenuItem[j]);

			LogTimeCost("Run here 6.2 = ");//by for debug

			if(vctBinResult.empty())	// û���ҵ��˽ڵ�˵����
			{
				return ERRNODEMENU;
			}
			else
			{
				NextTextId = vctBinResult[0];
			}
			g_pDisplay->Menu.Add(NextTextId);

			LogTimeCost("Run here 6.3 = ");//by for debug
		}

		LogTimeCost("Run here 7 = ");//by for debug

		dataFile.Close();

		if(0 == iTaskMarkNum)  // ������һ��
		{
			menuLevel.structMenu.m_i16MenuSelected = -1;
			menuLevel.structMenu.m_i16MenuScreenFirstLineItem = 0;
		}
		else if(-1 == iTaskMarkNum) // ��ʾ����
		{
			//menuLevel.structMenu.m_i16MenuSelected = -1;
		}

		if(byFlag !=0)
		{
			g_pDisplay->Menu.SetFlag(byFlag);
			iSelected = g_pDisplay->Menu.Show(menuLevel.structMenu);
		}
		else
		{
			iSelected = g_pDisplay->Menu.Show(menuLevel.structMenu);
		}

		LogTimeCost("Run here 8 = ");//by for debug

		if(iSelected == -1) // ����
		{
			if(stackMenuLevel.empty())
			{
				return 0;
			}

			menuLevel = stackMenuLevel.top();
			idMenu = menuLevel.idMenu;

			stackMenuLevel.pop();

			iTaskMarkNum = -2; // ������һ��

			m_bExecuteTask = false;  // ����ʱ��ִ��TaskId
		}
		else
		{
			menuLevel.idMenu = idMenu;  // �˵����ID

			stackMenuLevel.push(menuLevel);
			idMenu = vctBinCurMenuItem[iSelected];

			iTaskMarkNum = 0;  // ������һ��

			m_bExecuteTask = true;
		}
	}
	
	return 0;
}

bool CMenuTree::IsTaskIdOk(unsigned int unTaskId)
{
	return true;//hpy
	
#if defined(_WIN32) & defined(_DEBUG)
	return true;
#endif
	if(CRunEnvironment::GetDemoMode())
	{
		return true;
	}

	//�ж�ID�Ƿ����� add by tom sun 20100406
	string strFilename;
	string strLanguage  = CRunEnvironment::GetLanguage();
	unsigned short iVehchileID  = CInformation::GetVehicleID();
	strFilename = CRunEnvironment::GetDiagnosisDirectory();
	char chEnd = *(strFilename.end()-1);
	if(chEnd != '\\' && chEnd != '/')
	{
		strFilename += '\\';
	}
	//strFilename += "license.dat";
	const char *pchFileName = NULL;
	pchFileName = strFilename.data();
	CSecurityBaseClass checkTaskId;
	checkTaskId.Initialize(pchFileName);
#ifndef _TASKIDTYPE
	bool bOk = checkTaskId.CheckFunctionIdLegal(iVehchileID,unTaskId);
#else
	bool bOk = checkTaskId.CheckFunctionIdLegal(unTaskId);
#endif
	if(!bOk)
	{
		char cMessage[256];		
		if(strLanguage == "CN")
		{
			if(iVehchileID == 0x0086)
				sprintf(cMessage,"�޴�Ȩ��,����Ҫ�˹���,����ϵ 800-820-0677!");
			else
				sprintf(cMessage,"�޴�Ȩ��,����Ҫ�˹���,����ϵ 400-830-5028!");
		}
		else if(strLanguage == "TW")
		{
			sprintf(cMessage,"�o�˙��ޣ�Ո�M���ؽ��N��");
		}
		else
		{
			sprintf(cMessage,"No authority,Please contact your local dealer!");
			
		}
		g_pDisplay->MessageBox(cMessage, "Error");
	}
	//End add by tom sun 20100406

	return bOk;
}



#define DT_VEHICLE_SELECT_MENU		(char)1

int CMenuTree::ShowMenu(CBinary binIDMenu,unsigned char byFlag)
{
	if(!CNewVsIndicator::IsNewVehicleSelection())
	{
		return Org_ShowMenu(binIDMenu, byFlag);
	}
	
	CCommunicationWithDisplay* p_tx_rx = NULL;
	g_pDisplay->GetSendAndReceiveHandle(&p_tx_rx);
	
	bool b_1st_run = true;
	unsigned int un_task_ret = 0;

	while(true)
	{
		LogTimeCost("Here go 1 = ");//by for debug
		p_tx_rx->Init(DT_VEHICLE_SELECT_MENU);	//	function code
		LogTimeCost("Here go 2 = ");//by for debug
		p_tx_rx->Add((char)0);	//	DataDrv flag
		LogTimeCost("Here go 3 = ");//by for debug
		if(b_1st_run)
		{
			p_tx_rx->Add((char)0);	//	non-first-run flag
			p_tx_rx->Add((char)(binIDMenu.GetSize()+1));// length 
			p_tx_rx->Add(binIDMenu);	// item ID
			p_tx_rx->Add((char)byFlag);	// item flag
			b_1st_run = false;
			LogTimeCost("Here go 4 = ");//by for debug
		}
		else
		{
			p_tx_rx->Add((char)1);				//	non-first-run flag
			p_tx_rx->Add((char)1);				//	length
			p_tx_rx->Add((char)un_task_ret);	//	result of task
			LogTimeCost("Here go 5 = ");//by for debug
		}
		LogTimeCost("Here go 6 = ");//by for debug
		p_tx_rx->SendDataToDisplayWaitResponsion();
		LogTimeCost("Here go 7 = ");//by for debug

		char* p_ch = p_tx_rx->GetBuffer();
		p_ch += 4;	//	offset 4 bytes head: 2-bytes-length + func.code + DBG
		
		if(p_ch[0]==3)		//	Do Diag
		{
			p_ch++;
			
			unsigned int un_task_id;
			if(p_ch[0]==2)	// length of TaskID
			{
				un_task_id = (unsigned char)p_ch[1]*256 + (unsigned char)p_ch[2];  // TaskID
				p_ch += 3;
			}
			else/* if(p_ch[0]==4)*/
			{
				un_task_id = (unsigned char)p_ch[1]*256*256*256 + (unsigned char)p_ch[2]*256*256
					+ (unsigned char)p_ch[3]*256 + (unsigned char)p_ch[4];
				p_ch += 5;
			}

			CBinary bin_slt_itm;
			bin_slt_itm.WriteBuffer(p_ch,6);
			p_ch += 6;

			/*
			 *	retrieve and set selected VSFE here	JZ
			 */
#define S_MENU_CHOOSED		"MENU_CHOOSED"
			if(strncmp(p_ch,S_MENU_CHOOSED,strlen(S_MENU_CHOOSED))==0)
			{
				string s_vsfe = p_ch + strlen(S_MENU_CHOOSED);
				CRunEnvironment::SetMenuChoosedDir(s_vsfe);
			}

			if(IsTaskIdOk(un_task_id)&&m_pfnTask)
			{
#ifndef _TASKIDTYPE
				un_task_ret = (*m_pfnTask)((unsigned short)un_task_id,bin_slt_itm);
#else
				un_task_ret = (*m_pfnTask)(un_task_id,bin_slt_itm);
#endif
			}

		}
		else/* if(p_ch[0]==1)*/	//	VSFE over / err value from Display
		{
			break;
		}
	}

	return 0;
}

#ifndef _TASKIDTYPE
/************************************************************
��    �ܣ����ûص�����
����˵����unsigned short (*pfnTask) (short iTaskId) �ص�����ָ��
�� �� ֵ����ǰ�Ļص�����ָ��
˵    ������
************************************************************/
void* CMenuTree::SetTaskCallBackFunction(short (*pfnTask) (short iTaskId,  CBinary idSelectedText))
{
	void* PreTaskFunc = (void*)(*m_pfnTask);
	m_pfnTask = pfnTask;
	return PreTaskFunc;
}

#else
/************************************************************
��    �ܣ����ûص�����
����˵����unsigned short (*pfnTask) (short iTaskId) �ص�����ָ��
�� �� ֵ����ǰ�Ļص�����ָ��
˵    ������
//add by scf for 4�ֽ�����ID2007.8.2
************************************************************/
void* CMenuTree::SetTaskCallBackFunction(unsigned int  (*pfnTask) (unsigned int  iTaskId,  CBinary idSelectedText))
{
	void* PreTaskFunc = (void*)(*m_pfnTask);
	m_pfnTask = pfnTask;
	return PreTaskFunc;
}

#endif

#if 0
//by DEBUG FOR MENU DISPLAY TIME
void LogTimeInit(void)
{
	string LogFileName = CRunEnvironment::GetDiagnosisDirectory();
	LogFileName += "\\TimeCostLog.TXT";
	m_fp = fopen(LogFileName.c_str(),"a+b");	// open this file for logging

	if( !m_fp )
	{
		return;
	}
}
void GetTimeStart(void)
{
	m_TimeStart = 0;
	m_TimeStart = GetTickCount();
}
void GetTimeCost(void)
{
	m_TimeCost = 0;
	m_TimeCost = GetTickCount() - m_TimeStart;
}
void LogTimeCost(const string strLog)
{
	char time[20] = {0};
	string strTmp;
	strTmp += strLog;
	
	sprintf(time,"%d",GetTickCount());
	strTmp += time;
	strTmp += "ms\r\n";

	if(m_fp)
	{
		fwrite(strTmp.c_str(),sizeof(char),strTmp.length(),m_fp);
		fflush(m_fp);
	}

	return;
}
void LogTimeDestroy(void)
{	
	fclose(m_fp);
	m_fp = NULL;
}

#else

void LogTimeInit(void)
{

}
void GetTimeStart(void)
{

}
void GetTimeCost(void)
{

}
void LogTimeCost(const string strLog)
{

}
void LogTimeDestroy(void)
{	

}

#endif
