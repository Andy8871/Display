/******************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：MenuTree.cpp
	文档标识：007汽车诊断平台详细设计说明书
	摘    要：用于复杂菜单的显示。菜单树类（CMenuTree），用于显示大量菜单时使用，
			菜单树内容可以按显示内容分成多个类别，每一级菜单可以是这些类别中的任
			意一个，通过菜单生成工具生成菜单库(MENU.DB)。可以通过不同的根据入口
			值的不同，可在同一诊断车系中使用多个菜单树，亦可与菜单接口函数配合使
			用。菜单树允许嵌套调用。


	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2005.12.15  zhangsh		1.0			创建此类。                                           

******************************************************************************/

#include "menutree.h"
#include "Database.h"
#include "display.h"

#include "../../Security/include/SecurityBaseClass.h"
#include "RunEnvironment.h"
#include "Information.h"

#include "NewVsIndicator.h"


//define errors
#define NOMENUFILE	    	-1
#define ERRNODEMENU	    	-2
#define ERRNEXTIDNUM	    -3
#define ERRENTRYMENUID	    -4
#define ERRCALLBACKRESULT	-5

/*CDisplay * g_pDisplay = CDisplay::GetInstance();*/

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
功    能：显示菜单树
参数说明：CBinary idMenu 菜单入口ID
返 回 值：错误代码
说    明：1、按菜单树层次依次调用下级菜单；
		  2、如果是到最后一级菜单，执行完下行任务后，如还要
		     进入下一级时仍显示最后一级；
		  3、如存在下行任务调用设定的回调函数并传入任务ID；
		  4、回调函数返回值说明
								 0：进入下一级菜单；
								-1：显示本级菜单；
								-2：显示上一级菜单
************************************************************/
int CMenuTree::Org_ShowMenu (CBinary binIDMenu,unsigned char byFlag)
{
	CMenuLevel menuLevel;
	CDatabase  dataFile;
	CBinary	   idMenu = binIDMenu;

	vector<CBinary> vctBinCurMenuItem;		// 当前菜单列表
	vector<CBinary> vctBinResult;			// 查库返回的数组内容
		

	int iSelected        = 0;			    // 菜单的选中编号
	short iTaskMarkNum = 0;
		
	bool m_bExecuteTask  = true;
	
//	string strFileMenuDb = "menu.db";

	// 开始菜单点击操作
	while(true)
	{
		vctBinCurMenuItem.clear();  // 清空当前菜单列表
		vctBinResult.clear();       // 清空菜单的选中编号

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
		unsigned short iTaskID	   = (vctBinResult[1])[0]*256 + (unsigned char)(vctBinResult[1])[1];  // 下行任务ID
#else
		unsigned int iTaskID	   = (vctBinResult[1])[0]*256*256*256 + (vctBinResult[1])[1]*256*256+(vctBinResult[1])[2]*256+(unsigned char)(vctBinResult[1])[3];  // 下行任务ID
#endif
		unsigned short iNextNumber = (vctBinResult[2])[0]*256 + (unsigned char)(vctBinResult[2])[1];  // 下行菜单数

		// 如果有下行任务，先执行下行任务
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
				m_bExecuteTask = true;  // 需要执行任务ID
			}

			if(-1 == iTaskMarkNum) // 显示本级
			{
				m_bExecuteTask = true;  // 需要执行任务ID
			}			
		    else if(0 == iTaskMarkNum )  // 如果是最后一级菜单，如还要进入下一级时仍显示最后一级
			{
				if(0 == iNextNumber)
				{
					iTaskMarkNum = -1;
					m_bExecuteTask = false;  // 不需要执行任务ID
				}
			}
			
			if(-1 == iTaskMarkNum) // 显示本级
			{
				if(stackMenuLevel.empty())
				{					
					return ERRENTRYMENUID;  // 返回错误的入口菜单ID
				}	

				menuLevel = stackMenuLevel.top();
				idMenu = menuLevel.idMenu;

				stackMenuLevel.pop();
				continue;
			}
			else if(iTaskMarkNum == -2)  // 显示上级
			{
				if(stackMenuLevel.empty()) // 返回错误的入口菜单ID
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
		
		// 如果没有下行菜单(已经到最底层菜单)
		if(iNextNumber == 0)
		{
			// 先取得栈内容，再弹栈
			if(!stackMenuLevel.empty())
			{
				menuLevel = stackMenuLevel.top();
				idMenu = menuLevel.idMenu;

				iTaskMarkNum = -1;  // 仍然显示本级
				
				m_bExecuteTask = false;  // 由于前面已经执行TaskID，故再次显示时将不执行TaskID

				stackMenuLevel.pop();
				continue;
			}
			else // 如果栈为空，则说明是第一次进入，且无下行菜单
			{
				// 返回错误的入口菜单ID
				return ERRENTRYMENUID;
			}
		}

		LogTimeCost("Run here 5 = ");//by for debug

		// 下面是显示下行菜单
		CBinary menuTextId	= vctBinResult[0];
		CDisplay::GetInstance()->Menu.Init(menuTextId);
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

		// 取得下行菜单对应的文本ID
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

			if(vctBinResult.empty())	// 没有找到此节点菜单编号
			{
				return ERRNODEMENU;
			}
			else
			{
				NextTextId = vctBinResult[0];
			}
			CDisplay::GetInstance()->Menu.Add(NextTextId);

			LogTimeCost("Run here 6.3 = ");//by for debug
		}

		LogTimeCost("Run here 7 = ");//by for debug

		dataFile.Close();

		if(0 == iTaskMarkNum)  // 进入下一级
		{
			menuLevel.structMenu.m_i16MenuSelected = -1;
			menuLevel.structMenu.m_i16MenuScreenFirstLineItem = 0;
		}
		else if(-1 == iTaskMarkNum) // 显示本级
		{
			//menuLevel.structMenu.m_i16MenuSelected = -1;
		}

		if(byFlag !=0)
		{
			CDisplay::GetInstance()->Menu.SetFlag(byFlag);
			iSelected = CDisplay::GetInstance()->Menu.Show(menuLevel.structMenu);
		}
		else
		{
			iSelected = CDisplay::GetInstance()->Menu.Show(menuLevel.structMenu);
		}

		LogTimeCost("Run here 8 = ");//by for debug

		if(iSelected == -1) // 返回
		{
			if(stackMenuLevel.empty())
			{
				return 0;
			}

			menuLevel = stackMenuLevel.top();
			idMenu = menuLevel.idMenu;

			stackMenuLevel.pop();

			iTaskMarkNum = -2; // 返回上一级

			m_bExecuteTask = false;  // 返回时不执行TaskId
		}
		else
		{
			menuLevel.idMenu = idMenu;  // 菜单入口ID

			stackMenuLevel.push(menuLevel);
			idMenu = vctBinCurMenuItem[iSelected];

			iTaskMarkNum = 0;  // 进入下一级

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

	//判断ID是否被允许 add by tom sun 20100406
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
				sprintf(cMessage,"无此权限,如需要此功能,请联系 800-820-0677!");
			else
				sprintf(cMessage,"无此权限,如需要此功能,请联系 400-830-5028!");
		}
		else if(strLanguage == "TW")
		{
			sprintf(cMessage,"無此權限，請聯繫當地經銷商");
		}
		else
		{
			sprintf(cMessage,"No authority,Please contact your local dealer!");
			
		}
		CDisplay::GetInstance()->MessageBox(cMessage, "Error");
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
	CDisplay::GetInstance()->GetSendAndReceiveHandle(&p_tx_rx);
	
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
功    能：设置回调函数
参数说明：unsigned short (*pfnTask) (short iTaskId) 回调函数指针
返 回 值：以前的回调函数指针
说    明：无
************************************************************/
void* CMenuTree::SetTaskCallBackFunction(short (*pfnTask) (short iTaskId,  CBinary idSelectedText))
{
	void* PreTaskFunc = (void*)(*m_pfnTask);
	m_pfnTask = pfnTask;
	return PreTaskFunc;
}

#else
/************************************************************
功    能：设置回调函数
参数说明：unsigned short (*pfnTask) (short iTaskId) 回调函数指针
返 回 值：以前的回调函数指针
说    明：无
//add by scf for 4字节任务ID2007.8.2
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
