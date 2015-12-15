// BasicDiagnosticUnit.cpp: implementation of the CBasicDiagnosticUnit class.
//
//////////////////////////////////////////////////////////////////////

//#include "DiagStd.h"
#include "CommWithEcu.h"
#include "Display.h"
#include "adsStd.h"
#include "MenuTree.h"
#include "Binary.h"
#include "Database.h"
#include "information.h"
#include "EcuCommException.h"
#include "DiagnosisUnit.h"
#include "RunEnvironment.h"

#include "BasicDiagnosticUnit.h"
#include "AutoLog.h"
#include "DrivenStd.h"
#include "CommLayer.h"
#include "DataListFunc.h"
#include "DTCFunc.h"
#include "sysstr.h"
#include "LogCat.h"
CBinary binVehicleID=NULL;
CBinary binECUID=NULL;
CBinary binCommECU_ID=NULL;
unsigned char bCommBUS_ID=0x00;
CBinary binCurECU_ID=NULL;
unsigned char bCurBUS_ID=0x00;
CCommLayer *CommLayer=NULL;
CDataListFunc *DataListFunc=NULL;
CDTCFunc *DTCFunc=NULL;
CCommonTools *CommonTools=NULL;
CBinaryGroup vecReceData;

extern	CCommWithEcu*	g_pCommEcu;	    //全局通信接口
//extern	CDisplay *	g_pDisplay; 		//全局显示接口
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//LogCat tag
const char* TAG = "CBasicDiagnosticUnit";

CBasicDiagnosticUnit::CBasicDiagnosticUnit()
{

}

CBasicDiagnosticUnit::~CBasicDiagnosticUnit()
{

}

/********************************************************************************
功    能：	扫描系统是否存在
参数说明：	无
返 回 值：	成功—0 不成功—返回错误代码
说    明：	无
********************************************************************************/
short CBasicDiagnosticUnit::ScanSystemExist (void)
{
	if(!EnterSystem())
		return 0;
	
	ExitSystem();

	return 1;
}

short CBasicDiagnosticUnit::QuickReadTroubleCode()
{
	return 1;

}

short CBasicDiagnosticUnit::QuickClearTroubleCode()
{
	return 1;

}

void CBasicDiagnosticUnit::OnEnter(unsigned short uiWay)
{
	if(CommLayer==NULL)
		CommLayer=new CCommLayer;
	if(DataListFunc==NULL)
		DataListFunc=new CDataListFunc;
	if(DTCFunc==NULL)
		DTCFunc=new CDTCFunc;

	return ;
}

void CBasicDiagnosticUnit::OnExit(unsigned short uiWay)
{
	if(CommLayer!=NULL)
	{
		delete CommLayer;
		CommLayer=NULL;
	}
	if(DataListFunc!=NULL)
	{
		delete DataListFunc;
		DataListFunc=NULL;
	}
	if(DTCFunc!=NULL)
	{
		delete DTCFunc;
		DTCFunc=NULL;
	}
	return ;
}

short CBasicDiagnosticUnit::NormalDiagnosis()
{
	short iRet = 0;

	if ( !EnterSystem() )//进系统返回错误码
	{
		return 0;
	}

	iRet = MenuAndProcess();
	ExitSystem();

	return 1;
}

short CBasicDiagnosticUnit::EnterSystem ()
{
	vector<CBinary> ECUIDInfo;
	CBinary CBIndex=NULL;
	short iRet=0;

	binCommECU_ID=binCurECU_ID;
	bCommBUS_ID=bCurBUS_ID;

	if(!CommLayer->GetEnterCmdID())
		return iRet;

	iRet=CommLayer->ChangeCommStatus(Comm_Status_NA,Comm_Status_General);

	return iRet;
}

void CBasicDiagnosticUnit::ExitSystem ()
{
	CommLayer->ChangeCommStatus(Comm_Status_General,Comm_Status_NA);
	return;
}

short CBasicDiagnosticUnit::MenuAndProcess()
{
	CBinaryGroup vecFuncMenuID;
	short iSelect=0;

	GetFuncInfo();

	if(!GetFuncMenuInfo(vecFuncMenuID))
		return 0;

	while(1)
	{
		iSelect=CommonTools->MenuAddItem(binSysTxtIndex,vecFuncMenuID);
		if(iSelect<0)
			break;
		switch(vecFuncMenuID[iSelect][5])
		{
		case 0x02:
			DTCFunc->DTCFuncShow(vecFuncMenuID[iSelect]);
			break;
		case 0x05:
			DataListFunc->DataListShow(vecFuncMenuID[iSelect]);
			break;
		default:
			break;
		}
	}

	return 1;
}

short CBasicDiagnosticUnit::GetFuncInfo()
{
	DataListFunc->GetDataListVecItem();
	DTCFunc->CheckDTCFunc();
	return 1;
}

short CBasicDiagnosticUnit::GetFuncMenuInfo(CBinaryGroup &vecFuncMenuID)
{
	vecFuncMenuID.clear();

	if(DTCFunc->Flag_DTC==true)
	{
		vecFuncMenuID.push_back(CBinary("\x00\xFF\x01\x00\x00\x02",6));
	}
	if(DataListFunc->Flag_DataList==true)
	{
		vecFuncMenuID.push_back(CBinary("\x00\xFF\x01\x00\x00\x05",6));
	}

	return vecFuncMenuID.size();
}

short CBasicDiagnosticUnit::SetSysNameID(CBinary binIndex)
{
	binSysTxtIndex=binIndex;
	return 1;
}

short CBasicDiagnosticUnit::OBDIIShow()
{
	short iSelect=0;
	CBinaryGroup vecMenuID;
	
	vecMenuID.clear();
	vecMenuID.push_back(CBinary("\x00\xFF\x03\x00\x00\x05",6));

	while(1)
	{
		iSelect=CommonTools->MenuAddItem(CBinary("\x00\xFF\x04\x00\x00\x01",6),vecMenuID);
		if(iSelect<0)
			break;
		OBDIIDiagnose();
	}
	
	return 1;
}

short CBasicDiagnosticUnit::GetOBDIISysStrs(string &strSys,unsigned char bECUID)
{
	char strECUID[10];

	strSys="";

	sprintf(strECUID," (&%02X)",bECUID);

	switch(bCommBUS_ID)
	{
	case P_KWP2000_ON_CAN:
		if(bECUID==0xE8)
		{
			strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x07",6))+strECUID;
		}
		else if(bECUID==0xE9)
		{
			strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x08",6))+strECUID;
		}
		else
		{
			strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x06",6))+strECUID;
		}
		break;
	case P_ED_ON_CAN:
		if(bECUID==0x11)
		{
			strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x07",6))+strECUID;
		}
		else if(bECUID==0x1D)
		{
			strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x08",6))+strECUID;
		}
		else
		{
			strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x06",6))+strECUID;
		}
		break;
	case P_KWP2000_ON_K_LINE:
	case P_ED_ISO_9141_K_LINE:
		if(bECUID==0x11)
		{
			strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x07",6))+strECUID;
		}
		else if(bECUID==0x18)
		{
			strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x08",6))+strECUID;
		}
		else
		{
			strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x06",6))+strECUID;
		}
		break;
	case P_ED_VPW:
	case P_ED_PWM:
		strSys=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x06",6))+strECUID;
		break;
	default:
		break;
	}

	return strSys.size();
}

short CBasicDiagnosticUnit::OBDIIDiagnose()
{
	short iSelect=0;
	CBinary binECUIDs=NULL;
	vector<string> vecSysStr;
	short iNum=0;
	short i=0;
	string strECUName="";

	OnEnter(1);
	binCurECU_ID=CBinary("\xE0\x00\x00\x01",4);
	binCommECU_ID=binCurECU_ID;
	for(i=0;i<1;i++)
	{
		if(CommLayer->OBDIICommSet_15765(binECUIDs)>0)
		{
			binCurECU_ID=CBinary("\xE0\x00\x00\x02",4);
			binCommECU_ID=binCurECU_ID;
			break;
		}
		/*if(CommLayer->OBDIICommSet_KLine(binECUIDs)>0)
		{
			if(bCurBUS_ID==P_KWP2000_ON_K_LINE)
			{
				CommLayer->Link_Info[Pos_ECUAddress]=CBinary("\x33",1);
				CommLayer->Link_Info[Pos_PackageHeader]=CBinary("\x05",1);
			}
			break;
		}*/
		if(CommLayer->OBDIICommSet_J1850_VPW(binECUIDs)>0)
			break;
		/*if(CommLayer->OBDIICommSet_J1850_PWM(binECUIDs)>0)
			break;*/
	}

	bCommBUS_ID=bCurBUS_ID;

	CommLayer->OBDIISetIdle();

	iNum=binECUIDs.GetSize();
	if(!binECUIDs.GetSize())
	{
		CDisplay::GetInstance()->MessageBox(CBinary("\x00\xFF\x00\x00\x00\x11",6),STRID_ERROR);
	}

	CommLayer->bCommStatus=Comm_Status_General;

	for(i=0;i<iNum;i++)
	{
		if(GetOBDIISysStrs(strECUName,binECUIDs[i]))
		{
			vecSysStr.push_back(strECUName);
		}
	}
	
	/*for (int i = 0; i < vecSysStr.size(); ++i)
	{
		LOG_I(TAG, "[OBDIIDiagnose:%d]Systemt name--->%s", __LINE__,vecSysStr[i].c_str());
	}
*/
	while(1)
	{
		iSelect=CommonTools->MenuAddItemStr(adsGetTextString(CBinary("\x00\xFF\x04\x00\x00\x01",6)),vecSysStr);
		if(iSelect<0)
			break;
		CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x10",6)),vecSysStr[iSelect],adsMB_NoButton);
		CommLayer->OBDIIResetFilter(binECUIDs[iSelect]);
		DataListFunc->GetDataListVecItem();
		OBDIIFuncShow(vecSysStr[iSelect]);
	}

	OBDIIExitSystem();
	if(!Demo_Flag)
		CCommWithEcu::GetInstance()->KeepLink();

	return 1;
}

short CBasicDiagnosticUnit::OBDIIExitSystem()
{
	if(bCommBUS_ID==P_KWP2000_ON_K_LINE)
	{
		CommLayer->SendReceiveCmd(CBinary("\x00\x82\x01",3),vecReceData);
	}

	return 1;
}

short CBasicDiagnosticUnit::OBDIIFuncShow(string strTitle)
{
	short iSelect=0;
	CBinaryGroup vecItemStrID;
	vector<string> vecFuncItemStr;
	CBinaryGroup vecDSItemID;
	

	//LOG_I(TAG, "[OBDIIFuncShow394]titile--->%s", strTitle.c_str());
	if(!GetFuncItemStr(vecItemStrID,vecFuncItemStr))
		return 0;
	
	// 先显示系统状态
	DataListFunc->GetDSGrpItems(vecDSItemID,1);
	DataListFunc->SystemInfoShow(vecItemStrID[1],vecDSItemID);

	/*for (int i = 0; i < vecFuncItemStr.size(); ++i)
	{
		LOG_I(TAG, "[OBDIIFuncShow404]Function item%d--->%s", i, vecFuncItemStr[i].c_str());
	}*/
	while(1)
	{
		iSelect=CommonTools->MenuAddItemStr(strTitle,vecFuncItemStr);
		if(iSelect<0)
			break;
		switch(iSelect)
		{
		case 0:
			DTCFunc->DTCFuncShow(CBinary("\x00\xFF\x01\x00\x00\x02",6));
			break;
		case 1:
			DataListFunc->GetDSGrpItems(vecDSItemID,iSelect);
			if(!vecDSItemID.size())
			{
				CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
				break;
			}
			DataListFunc->SystemInfoShow(vecItemStrID[iSelect],vecDSItemID);
			break;
		case 2:
			DataListFunc->GetDSGrpItems(vecDSItemID,iSelect);
			if(!vecDSItemID.size())
			{
				CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
				break;
			}
			OBDIIReadnissCode(vecItemStrID[iSelect],vecDSItemID);
			break;
		case 3:
			if(DataListFunc->OBDIICheckFreezeData())
			{
				DataListFunc->GetDSGrpItems(vecDSItemID,iSelect);
				DataListFunc->DataListFlash(vecItemStrID[iSelect],vecDSItemID);
			}
			else
			{
				CDisplay::GetInstance()->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x11",6),vecDSItemID[iSelect]);
			}
			break;
		case 4:
			DataListFunc->GetDSGrpItems(vecDSItemID,iSelect);
			if(!vecDSItemID.size())
			{
				CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
				break;
			}
			DataListFunc->DataListFlash(vecItemStrID[iSelect],vecDSItemID);
			break;
		case 5:
			if((bCommBUS_ID==P_KWP2000_ON_CAN)||(bCommBUS_ID==P_ED_ON_CAN))
			{
				CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
			}
			else
			{
				OBDIIO2MonitorTest(vecFuncItemStr[iSelect]);
			}
			break;
		case 6:
			OBDIIOnBoardTest(vecFuncItemStr[iSelect]);
			break;
		case 7:
			CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
			break;
		case 8:
			DataListFunc->GetDSGrpItems(vecDSItemID,iSelect);
			if(!vecDSItemID.size())
			{
				CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
				break;
			}
			if((bCommBUS_ID==P_KWP2000_ON_CAN)||(bCommBUS_ID==P_ED_ON_CAN))
			{
				OBDIISysInfo_CAN(vecFuncItemStr[iSelect],vecDSItemID);
			}
			else
			{
				OBDIISysInfo_NoCAN(vecFuncItemStr[iSelect],vecDSItemID);
			}
			//DataListFunc->SystemInfoShow(vecItemStrID[iSelect],vecDSItemID);
			break;
		default:
			break;
		}
	}

	return 1;
}

short CBasicDiagnosticUnit::GetFuncItemStr(CBinaryGroup &vecItemStrID,vector<string> &vecItemStr)
{
	short i=0;
	CBinary binTxtID=NULL;
	string strItemName;
	CBinaryGroup vecDsGrpID;

	vecItemStrID.clear();
	vecItemStr.clear();
	binTxtID=CBinary("\x00\xFF\x01\x00\x00\x02",6);
	strItemName=adsGetTextString(binTxtID);
	vecItemStrID.push_back(binTxtID);
	vecItemStr.push_back(strItemName);

	DataListFunc->GetDSGrpStrIDs(vecDsGrpID);

	for(i=0;i<vecDsGrpID.size();i++)
	{
		binTxtID=vecDsGrpID[i];
		strItemName=adsGetTextString(binTxtID);
		vecItemStrID.push_back(binTxtID);
		vecItemStr.push_back(strItemName);
		if(vecItemStr.size()>8) break;
	}

	return vecItemStr.size();
}

short CBasicDiagnosticUnit::OBDIIOnBoardTest(string strTitle)
{
	CBinaryGroup vecDSItemID;
	CBinaryGroup vecItemInfo;
	string strItemName="";
	vector<string> vecItemStr;
	short iNum=0;
	short i=0;
	short iSelect=0;

	DataListFunc->GetDSGrpItems(vecDSItemID,6);
	if(!vecDSItemID.size())
	{
		CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
		return 1;
	}

	vecItemStr.clear();
	iNum=vecDSItemID.size();
	for(i=0;i<iNum;i++)
	{
		if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",vecDSItemID[i]))
			continue;
		strItemName=CommonTools->GetTxtString(vecItemInfo[1]);
		vecItemStr.push_back(strItemName);
	}

	while(1)
	{
		iSelect=CommonTools->MenuAddItemStr(strTitle,vecItemStr);
		if(iSelect<0)
			break;
		if((bCommBUS_ID==P_KWP2000_ON_CAN)||(bCommBUS_ID==P_ED_ON_CAN))
		{
			OBDIIOnBoardTestShow(vecItemStr[iSelect],vecDSItemID[iSelect]);
		}
		else
		{
			OBDIIOnBoardTestShow1(vecItemStr[iSelect],vecDSItemID[iSelect]);
		}
	}

	return 1;
}

short CBasicDiagnosticUnit::OBDIIOnBoardTestShow(string strTitle,CBinary binItemID)
{
	CBinaryGroup vecItemInfo;
	CBinary binCmdID=NULL;
	short iRet=0;
	CBinary binReceData=NULL;
	short iLen=0;
	short iNum=0;
	short i=0;
	short iSelect=0;
	unsigned char bTestID=0x00;
	unsigned char bScalID=0x00;
	vector<string> vecItemStr;
	string strItemName="";
	string strValue="";
	string strUnit="";
	CBinary binConvID=NULL;
	CBinary binPOSs=NULL;
	CBinary binMASKs=NULL;
	CBinary binData=NULL;
	unsigned short iValueCur=0;
	unsigned short iValueMin=0;
	unsigned short iValueMax=0;

	CDisplay::GetInstance()->MessageBox(STRID_COMMUNICATING,STRID_INFORMATION,adsMB_NoButton);

	if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",binItemID))
	{
		return 0;
	}
	binCmdID=vecItemInfo[3];
	iRet=CommLayer->SendReceive(binCmdID,vecReceData);
	if(iRet!=Rece_Positive)
	{
		CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
		//CDisplay::GetInstance()->MessageBox(STRID_COMMUNICATION_FAILED,STRID_ERROR);
		return 0;
	}
	binReceData=vecReceData[0];
	iLen=binReceData.GetSize();
	iNum=(iLen-1)/9;
	if(!iNum)
	{
		CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
		return 0;
	}
	for(i=0;i<iNum;i++)
	{
		bTestID=binReceData[2+i*9];
		if(!GetTestIDStr(bTestID,strItemName))
			return 0;
		vecItemStr.push_back(strItemName);
	}

	while(1)
	{
		iSelect=CommonTools->MenuAddItemStr(strTitle,vecItemStr);
		if(iSelect<0)
			break;
		while(1)
		{
			CDisplay::GetInstance()->DataStream.Init(vecItemStr[iSelect]);
			bScalID=binReceData[3+iSelect*9];
			strItemName=vecItemStr[iSelect];
			strValue="";
			strUnit="";
			CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);
			if(!GetTestConvInfo(bScalID,vecItemInfo))
				break;
			strUnit=CommonTools->GetTxtString(vecItemInfo[2]);
			binConvID=vecItemInfo[6];
			binPOSs=vecItemInfo[7];
			binMASKs=vecItemInfo[8];
			strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0A",6));
			vecReceData.clear();
			binData=NULL;
			binData.Add(binReceData[4+iSelect*9]);
			binData.Add(binReceData[5+iSelect*9]);
			vecReceData.push_back(binData);
			strValue="*****";
			if(CommonTools->GetCtrlData(vecReceData,binPOSs,binMASKs,binData))
			{
				iValueCur=binData[1]*256+binData[0];
				DataListFunc->GetStringValue(0x00,binConvID,binData,strValue);
			}
			CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);
			strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0B",6));
			vecReceData.clear();
			binData=NULL;
			binData.Add(binReceData[6+iSelect*9]);
			binData.Add(binReceData[7+iSelect*9]);
			vecReceData.push_back(binData);
			strValue="*****";
			if(CommonTools->GetCtrlData(vecReceData,binPOSs,binMASKs,binData))
			{
				iValueMin=binData[1]*256+binData[0];
				DataListFunc->GetStringValue(0x00,binConvID,binData,strValue);
			}
			CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);
			strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0C",6));
			vecReceData.clear();
			binData=NULL;
			binData.Add(binReceData[8+iSelect*9]);
			binData.Add(binReceData[9+iSelect*9]);
			vecReceData.push_back(binData);
			strValue="*****";
			if(CommonTools->GetCtrlData(vecReceData,binPOSs,binMASKs,binData))
			{
				iValueMax=binData[1]*256+binData[0];
				DataListFunc->GetStringValue(0x00,binConvID,binData,strValue);
			}
			CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);
			strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0D",6));
			if((iValueCur>=iValueMin)&&(iValueCur<=iValueMax))
				strValue=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0E",6));
			else
				strValue=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0F",6));
			strUnit="";
			CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);
			if(CDisplay::GetInstance()->DataStream.Show()==0x0b)
			{
				break;
			}
		}
	}

	return 1;
}

short CBasicDiagnosticUnit::OBDIIOnBoardTestShow1(string strTitle,CBinary binItemID)
{
	CBinaryGroup vecItemInfo;
	CBinary binCmdID=NULL;
	short iRet=0;
	CBinary binReceData=NULL;
	short iLen=0;
	short i=0;
	unsigned char bComID=0x00;
	unsigned char bLimitType=0x00;
	string strItemName="";
	string strValue="";
	string strUnit="";
	CBinary binConvID=NULL;
	CBinary binPOSs=NULL;
	CBinary binMASKs=NULL;
	CBinary binData=NULL;
	unsigned short iValueCur=0;
	unsigned short iValueMin=0;
	unsigned short iValueMax=0;
	char strID[10];

	CDisplay::GetInstance()->MessageBox(STRID_COMMUNICATING,STRID_INFORMATION,adsMB_NoButton);

	if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",binItemID))
	{
		return 0;
	}
	binCmdID=vecItemInfo[3];
	iRet=CommLayer->SendReceive(binCmdID,vecReceData);
	if(iRet!=Rece_Positive)
	{
		CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
		//CDisplay::GetInstance()->MessageBox(STRID_COMMUNICATION_FAILED,STRID_ERROR);
		return 0;
	}
	binReceData=vecReceData[0];
	iLen=binReceData.GetSize();
	if(iLen<7)
	{
		CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
		return 0;
	}
	
	bComID=binReceData[2]&0x7F;
	bLimitType=(binReceData[2]>>7)&0x01;
	binConvID=vecItemInfo[6];
	binPOSs=vecItemInfo[7];
	binMASKs=vecItemInfo[8];
	sprintf(strID,"$%02X",bComID);
	strUnit=CommonTools->GetTxtString(vecItemInfo[2]);

	while(1)
	{
		CDisplay::GetInstance()->DataStream.Init(strTitle);
		strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x12",6))+strID;
		CDisplay::GetInstance()->DataStream.Add(strItemName,"","");


		strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0A",6));
		vecReceData.clear();
		binData=binReceData;
		vecReceData.push_back(binData);
		strValue="*****";
		if(CommonTools->GetCtrlData(vecReceData,binPOSs,binMASKs,binData))
		{
			iValueCur=binData[1]*256+binData[0];
			DataListFunc->GetStringValue(0x00,binConvID,binData,strValue);
		}
		CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);

		strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0B",6));
		vecReceData.clear();
		binData=binReceData;
		binData.SetAt(3,binData[5]);
		binData.SetAt(4,binData[6]);
		vecReceData.push_back(binData);
		strValue="*****";
		if(bLimitType)
		{
			if(CommonTools->GetCtrlData(vecReceData,binPOSs,binMASKs,binData))
			{
				iValueMin=binData[1]*256+binData[0];
				DataListFunc->GetStringValue(0x00,binConvID,binData,strValue);
			}
		}
		CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);

		strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0C",6));
		strValue="*****";
		if(!bLimitType)
		{
			if(CommonTools->GetCtrlData(vecReceData,binPOSs,binMASKs,binData))
			{
				iValueMax=binData[1]*256+binData[0];
				DataListFunc->GetStringValue(0x00,binConvID,binData,strValue);
			}
		}
		CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);

		strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0D",6));
		if(bLimitType)
		{
			if(iValueCur>=iValueMin)
				strValue=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0E",6));
			else
				strValue=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0F",6));
		}
		else
		{
			if(iValueCur<=iValueMax)
				strValue=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0E",6));
			else
				strValue=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0F",6));
		}
		CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,"");
		if(CDisplay::GetInstance()->DataStream.Show()==0x0b)
		{
			break;
		}
	}

	return 1;
}

short CBasicDiagnosticUnit::GetTestIDStr(unsigned char bID,string &strItem)
{
	CBinaryGroup vecItemInfo;
	CBinary binItemID=NULL;
	unsigned short iIndex=0;

	iIndex=bID+1;
	binItemID.Add(bBrand_ID);
	binItemID+=binCommECU_ID;
	binItemID.Add(0x09);
	binItemID.Add(0x00);
	binItemID.Add(0x00);
	binItemID.SetAt(6,(iIndex>>8)&0xFF);
	binItemID.SetAt(7,iIndex);

	if(!CommonTools->ReadDBFile(vecItemInfo,"DSGRP.DB",binItemID))
	{
		return 0;
	}
	binItemID=vecItemInfo[1];
	if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",binItemID))
	{
		return 0;
	}
	binItemID=vecItemInfo[1];

	strItem=CommonTools->GetTxtString(binItemID);

	return 1;
}

short CBasicDiagnosticUnit::GetTestConvInfo(unsigned char bID,CBinaryGroup &vecItemInfo)
{
	CBinary binItemID=NULL;
	unsigned short iIndex=0;
	
	iIndex=bID+1;
	binItemID.Add(bBrand_ID);
	binItemID+=binCommECU_ID;
	binItemID.Add(0x0A);
	binItemID.Add(0x00);
	binItemID.Add(0x00);
	binItemID.SetAt(6,(iIndex>>8)&0xFF);
	binItemID.SetAt(7,iIndex);
	
	if(!CommonTools->ReadDBFile(vecItemInfo,"DSGRP.DB",binItemID))
	{
		return 0;
	}
	binItemID=vecItemInfo[1];
	if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",binItemID))
	{
		return 0;
	}

	return 1;
}

short CBasicDiagnosticUnit::OBDIIO2MonitorTest(string strTitle)
{
	CBinaryGroup vecDSItemID;
	CBinaryGroup vecItemInfo;
	CBinary binSensorID=NULL;
	string strItemName="";
	vector<string> vecItemStr;
	short iNum=0;
	short i=0;
	short iSelect=0;
	
	DataListFunc->GetDSGrpItems(vecDSItemID,5);
	if(!vecDSItemID.size())
	{
		CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
		return 1;
	}
	
	vecItemStr.clear();
	iNum=vecDSItemID.size();
	for(i=0;i<iNum;i++)
	{
		if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",vecDSItemID[i]))
			continue;
		strItemName=CommonTools->GetTxtString(vecItemInfo[1]);
		vecItemStr.push_back(strItemName);
		binSensorID.Add(vecItemInfo[8][0]);
	}
	
	while(1)
	{
		iSelect=CommonTools->MenuAddItemStr(strTitle,vecItemStr);
		if(iSelect<0)
			break;
		OBDIIO2MonitorTestShow(vecItemStr[iSelect],binSensorID[iSelect]);
	}
	
	return 1;
}

short CBasicDiagnosticUnit::OBDIIO2MonitorTestShow(string strTitle,unsigned char bSensorID)
{
	CBinary binIndex=NULL;
	CBinary binDSListID=NULL;
	CBinaryGroup vecItemInfo;
	CBinaryGroup vecCmdInfo;
	CBinaryGroup vecItemID;
	vector<string> vecItemStr;
	CBinary binPIDs=NULL;
	unsigned short iIndex=1;
	CBinary binCmdInfo=NULL;
	CBinary binReceCmd=NULL;
	short i=0;
	short iRet=0;
	short iLen=0;
	unsigned char bSID=0x00;
	unsigned char bPID=0x00;
	CBinary binPID=NULL;
	short iSelect=0;

	CDisplay::GetInstance()->MessageBox(STRID_COMMUNICATING,STRID_INFORMATION,adsMB_NoButton);

	vecItemID.clear();
	vecItemStr.clear();
	
	binCmdInfo=NULL;
	binCmdInfo.Add(0x00);
	binCmdInfo.Add(0x05);
	binCmdInfo.Add(0x00);
	binCmdInfo.Add(bSensorID);
	binCmdInfo.Add(0x01);
	for(i=0;i<8;i++)
	{
		binCmdInfo.SetAt(2,i*0x20);
		iRet=CommLayer->SendReceiveCmd(binCmdInfo,vecReceData);
		CommLayer->UpdateHisReceDataCmd(binCmdInfo,iRet,vecReceData);
		if(iRet==Rece_Positive)
		{
			binReceCmd=vecReceData[0];
			if((binReceCmd[binReceCmd.GetSize()-1]&0x01)!=0x01)
				break;
		}
		else
		{
			break;
		}
	}
	for(;i<8;i++)
	{
		binCmdInfo.SetAt(2,i*0x20);
		iRet=Rece_Error;
		vecReceData.clear();
		CommLayer->UpdateHisReceDataCmd(binCmdInfo,iRet,vecReceData);
	}

	binIndex.Add(bBrand_ID);
	binIndex+=binCommECU_ID;
	binIndex.Add(0x09);
	binIndex.Add(0x00);
	binIndex.Add(0x00);

	while(1)
	{
		binIndex.SetAt(6,(iIndex>>8)&0xFF);
		binIndex.SetAt(7,iIndex&0xFF);
		if(!CommonTools->ReadDBFile(vecItemInfo,"DSGRP.DB",binIndex))
			break;
		binDSListID=vecItemInfo[1];
		if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",binDSListID))
			break;
		if(!CommLayer->GetCommandInfo(vecItemInfo[3],vecCmdInfo))
		{
			break;
		}
		bSID=vecCmdInfo[1][0];
		binPID=vecCmdInfo[2];
		if(binPID.GetSize()!=2)
			break;
		bPID=binPID[0];
		binPID.SetAt(0,bPID&0xE0);
		binPID.SetAt(1,bSensorID);
		binCmdInfo=NULL;
		binCmdInfo.Add(0x00);
		binCmdInfo.Add(bSID);
		binCmdInfo+=binPID;
		binCmdInfo.Add(0x01);
		iRet=CommLayer->ReadAndUpdateHisDataCmd(binCmdInfo,vecReceData);
		if(iRet==Rece_Positive)
		{
			binReceCmd=vecReceData[0];
			iLen=binReceCmd.GetSize();
			if(iLen>5)
			{
				if(DataListFunc->CheckPIDSupp(binReceCmd,bPID,iLen-4))
				{
					vecItemID.push_back(binDSListID);
					vecItemStr.push_back(CommonTools->GetTxtString(vecItemInfo[1]));
					binPIDs.Add(bPID);
				}
			}
		}
		iIndex++;
	}
	if(!vecItemID.size())
	{
		CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
		return 0;
	}
	
	string strItemName="";
	string strValue="";
	string strUnit="";
	CBinary binData=NULL;
	CBinary binConvID=NULL;
	unsigned short iValueCur=0;
	unsigned short iValueMin=0;
	unsigned short iValueMax=0;

	while(1)
	{
		iSelect=CommonTools->MenuAddItemStr(strTitle,vecItemStr);
		if(iSelect<0)
			break;
		if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",vecItemID[iSelect]))
			continue;
		binCmdInfo=NULL;
		binCmdInfo.Add(0x00);
		binCmdInfo.Add(0x05);
		binCmdInfo.Add(binPIDs[iSelect]);
		binCmdInfo.Add(bSensorID);
		binCmdInfo.Add(0x01);
		iRet=CommLayer->SendReceiveCmd(binCmdInfo,vecReceData);
		if(iRet!=Rece_Positive)
		{
			CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
			continue;
		}
		binReceCmd=vecReceData[0];
		iLen=binReceCmd.GetSize();
		if(iLen<4)
		{
			CDisplay::GetInstance()->MessageBox(adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x09",6)),strTitle);
			continue;
		}
		binConvID=vecItemInfo[6];
		strUnit=CommonTools->GetTxtString(vecItemInfo[2]);
		while(1)
		{
			CDisplay::GetInstance()->DataStream.Init(vecItemStr[iSelect]);
			strItemName=vecItemStr[iSelect];
			CDisplay::GetInstance()->DataStream.Add(strItemName,"","");

			strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0A",6));
			binData=NULL;
			binData.Add(binReceCmd[3]);
			iValueCur=binData[0];
			DataListFunc->GetStringValue(0x00,binConvID,binData,strValue);
			CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);
			
			strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0B",6));
			if(iLen<5)
			{
				strValue="*****";
			}
			else
			{
				binData=NULL;
				binData.Add(binReceCmd[4]);
				iValueMin=binData[0];
				DataListFunc->GetStringValue(0x00,binConvID,binData,strValue);
			}
			CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);
			
			strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0C",6));
			if(iLen<6)
			{
				strValue="*****";
			}
			else
			{
				binData=NULL;
				binData.Add(binReceCmd[5]);
				iValueMax=binData[0];
				DataListFunc->GetStringValue(0x00,binConvID,binData,strValue);
			}
			CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,strUnit);

			strItemName=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0D",6));
			if(iLen<6)
			{
				strValue=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0E",6));
			}
			else
			{
				if((iValueCur>=iValueMin)&&(iValueCur<=iValueMax))
					strValue=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0E",6));
				else
					strValue=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x0F",6));
			}
			CDisplay::GetInstance()->DataStream.Add(strItemName,strValue,"");

			if(CDisplay::GetInstance()->DataStream.Show()==0x0b)
			{
				break;
			}
		}
	}

	return 1;
}

short CBasicDiagnosticUnit::OBDIISysInfo_NoCAN(string strTitle,CBinaryGroup vecItemID)
{
	short i=0,j=0;
	CBinary binItemID=NULL;
	CBinaryGroup vecItemInfo;
	CBinaryGroup vecCMDInfo;
	CBinaryGroup vecReceDataTemp;
	string strItemName="";
	string strValue="";
	unsigned char bSID=0x00;
	unsigned char bPID=0x00;
	unsigned char bReceNum=0x00;
	CBinary binCmdInfo=NULL;
	CBinary binReceData=NULL;
	CBinary binData=NULL;
	short iRet=0;
	short iNum=0;
	char strIndex[10];

	CDisplay::GetInstance()->MessageBox(STRID_COMMUNICATING,STRID_INFORMATION,adsMB_NoButton);
	
	CommLayer->InitHisData();
	CDisplay::GetInstance()->VehicleInfo.Init(strTitle);
	for(i=0;i<vecItemID.size();i++)
	{
		binItemID=vecItemID[i];
		if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",binItemID))
			continue;
		if(!CommLayer->GetCommandInfo(vecItemInfo[3],vecCMDInfo))
			continue;
		//先发奇数PID获得会帧个数
		bSID=vecCMDInfo[1][0];
		bPID=vecCMDInfo[2][0];
		binCmdInfo=NULL;
		binCmdInfo.Add(0x00);
		binCmdInfo.Add(bSID);
		binCmdInfo.Add(bPID-1);
		binCmdInfo.Add(0x01);
		iRet=CommLayer->ReadAndUpdateHisDataCmd(binCmdInfo,vecReceData);
		if(iRet!=Rece_Positive)
			continue;
		binReceData=vecReceData[0];
		if(binReceData.GetSize()<3)
			continue;
		bReceNum=binReceData[2];
		binCmdInfo=NULL;
		binCmdInfo.Add(0x00);
		binCmdInfo.Add(bSID);
		binCmdInfo.Add(bPID);
		binCmdInfo.Add(bReceNum);
		iRet=CommLayer->ReadAndUpdateHisDataCmd(binCmdInfo,vecReceData);
		if(iRet!=Rece_Positive)
			continue;
		strItemName=CommonTools->GetTxtString(vecItemInfo[1]);
		switch(bPID)
		{
		case 0x02:
		case 0x0A:
			strValue="*****";
			if(CommonTools->GetCtrlData(vecReceData,vecItemInfo[7],vecItemInfo[8],binData))
			{
				DataListFunc->GetStringValue(vecItemInfo[11][0],vecItemInfo[6],binData,strValue);
			}
			CDisplay::GetInstance()->VehicleInfo.Add(strItemName,strValue);
			break;
		case 0x04:
			iNum=vecReceData.size()/4;
			for(j=0;j<iNum;j++)
			{
				vecReceDataTemp.clear();
				vecReceDataTemp.push_back(vecReceData[j*4]);
				vecReceDataTemp.push_back(vecReceData[1+j*4]);
				vecReceDataTemp.push_back(vecReceData[2+j*4]);
				vecReceDataTemp.push_back(vecReceData[3+j*4]);
				strValue="*****";
				if(CommonTools->GetCtrlData(vecReceDataTemp,vecItemInfo[7],vecItemInfo[8],binData))
				{
					DataListFunc->GetStringValue(vecItemInfo[11][0],vecItemInfo[6],binData,strValue);
				}
				if(iNum>1)
				{
					sprintf(strIndex," #%d",j+1);
					strItemName=CommonTools->GetTxtString(vecItemInfo[1]);
					strItemName+=strIndex;
				}
				CDisplay::GetInstance()->VehicleInfo.Add(strItemName,strValue);
			}
			break;
		case 0x06:
			iNum=vecReceData.size();
			for(j=0;j<iNum;j++)
			{
				vecReceDataTemp.clear();
				vecReceDataTemp.push_back(vecReceData[j]);
				strValue="*****";
				if(CommonTools->GetCtrlData(vecReceDataTemp,vecItemInfo[7],vecItemInfo[8],binData))
				{
					DataListFunc->GetStringValue(vecItemInfo[11][0],vecItemInfo[6],binData,strValue);
				}
				if(iNum>1)
				{
					sprintf(strIndex," #%d",j+1);
					strItemName=CommonTools->GetTxtString(vecItemInfo[1]);
					strItemName+=strIndex;
				}
				CDisplay::GetInstance()->VehicleInfo.Add(strItemName,strValue);
			}
			break;
		case 0x08:
			strValue="*****";
			if(CommonTools->GetCtrlData(vecReceDataTemp,vecItemInfo[7],vecItemInfo[8],binData))
			{
				DataListFunc->GetStringValue(vecItemInfo[11][0],vecItemInfo[6],binData,strValue);
				CDisplay::GetInstance()->VehicleInfo.Add(strItemName,strValue);
			}
			break;
		default:
			break;
		}
	}
	CDisplay::GetInstance()->VehicleInfo.Show();

	return 1;
}

short CBasicDiagnosticUnit::OBDIISysInfo_CAN(string strTitle,CBinaryGroup vecItemID)
{
	short i=0,j=0,k=0;
	CBinary binItemID=NULL;
	CBinaryGroup vecItemInfo;
	CBinaryGroup vecCMDInfo;
	CBinaryGroup vecReceDataTemp;
	string strItemName="";
	string strValue="";
	unsigned char bSID=0x00;
	unsigned char bPID=0x00;
	unsigned char bReceNum=0x00;
	CBinary binCmdInfo=NULL;
	CBinary binReceData=NULL;
	CBinary binData=NULL;
	short iRet=0;
	short iNum=0;
	char strIndex[10];

	CDisplay::GetInstance()->MessageBox(STRID_COMMUNICATING,STRID_INFORMATION,adsMB_NoButton);
	
	CommLayer->InitHisData();
	CDisplay::GetInstance()->VehicleInfo.Init(strTitle);
	for(i=0;i<vecItemID.size();i++)
	{
		binItemID=vecItemID[i];
		if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",binItemID))
			continue;
		if(!CommLayer->GetCommandInfo(vecItemInfo[3],vecCMDInfo))
			continue;
		bSID=vecCMDInfo[1][0];
		bPID=vecCMDInfo[2][0];
		if(!CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",binItemID))
			continue;
		iRet=CommLayer->ReadAndUpdateHisData(vecItemInfo[3],vecReceData);
		if(iRet!=Rece_Positive)
			continue;
		strItemName=CommonTools->GetTxtString(vecItemInfo[1]);
		switch(bPID)
		{
		case 0x02:
		case 0x0A:
			strValue="*****";
			if(CommonTools->GetCtrlData(vecReceData,vecItemInfo[7],vecItemInfo[8],binData))
			{
				DataListFunc->GetStringValue(vecItemInfo[11][0],vecItemInfo[6],binData,strValue);
			}
			CDisplay::GetInstance()->VehicleInfo.Add(strItemName,strValue);
			break;
		case 0x04:
			binReceData=vecReceData[0];
			iNum=(binReceData.GetSize()-3)/16;
			for(j=0;j<iNum;j++)
			{
				binData=NULL;
				binData.Add(0x49);
				binData.Add(0x04);
				binData.Add(iNum);
				for(k=0;k<16;k++)
				{
					binData.Add(binReceData[3+k+j*16]);
				}
				vecReceDataTemp.clear();
				vecReceDataTemp.push_back(binData);
				strValue="*****";
				if(CommonTools->GetCtrlData(vecReceDataTemp,vecItemInfo[7],vecItemInfo[8],binData))
				{
					DataListFunc->GetStringValue(vecItemInfo[11][0],vecItemInfo[6],binData,strValue);
				}
				if(iNum>1)
				{
					sprintf(strIndex," #%d",j+1);
					strItemName=CommonTools->GetTxtString(vecItemInfo[1]);
					strItemName+=strIndex;
				}
				CDisplay::GetInstance()->VehicleInfo.Add(strItemName,strValue);
			}
			break;
		case 0x06:
			binReceData=vecReceData[0];
			iNum=(binReceData.GetSize()-3)/4;
			for(j=0;j<iNum;j++)
			{
				binData=NULL;
				binData.Add(0x49);
				binData.Add(0x06);
				binData.Add(iNum);
				for(k=0;k<4;k++)
				{
					binData.Add(binReceData[3+k+j*4]);
				}
				vecReceDataTemp.clear();
				vecReceDataTemp.push_back(binData);
				strValue="*****";
				if(CommonTools->GetCtrlData(vecReceDataTemp,vecItemInfo[7],vecItemInfo[8],binData))
				{
					DataListFunc->GetStringValue(vecItemInfo[11][0],vecItemInfo[6],binData,strValue);
				}
				if(iNum>1)
				{
					sprintf(strIndex," #%d",j+1);
					strItemName=CommonTools->GetTxtString(vecItemInfo[1]);
					strItemName+=strIndex;
				}
				CDisplay::GetInstance()->VehicleInfo.Add(strItemName,strValue);
			}
			break;
		case 0x08:
			strValue="*****";
			if(CommonTools->GetCtrlData(vecReceData,vecItemInfo[7],vecItemInfo[8],binData))
			{
				DataListFunc->GetStringValue(vecItemInfo[11][0],vecItemInfo[6],binData,strValue);
				CDisplay::GetInstance()->VehicleInfo.Add(strItemName,strValue);
			}
			break;
		default:
			break;
		}
	}
	CDisplay::GetInstance()->VehicleInfo.Show();

	return 1;
}

short CBasicDiagnosticUnit::OBDIIReadnissCode(CBinary binTitle,CBinaryGroup vecItemID)
{
	CBinary binItemID=NULL;
	CBinaryGroup vecItemInfo;
	CBinaryGroup vecCmdInfo;
	CBinaryGroup vecDSItemID01;
	CBinaryGroup vecDSItemID41;
	CBinaryGroup vecMenuID;
	short i=0;
	unsigned char bPID=0x00;
	short iSelect=0;

	vecDSItemID01.clear();
	vecDSItemID41.clear();
	for(i=0;i<vecItemID.size();i++)
	{
		binItemID=vecItemID[i];
		if(CommonTools->ReadDBFile(vecItemInfo,"DATALIST.DB",binItemID))
		{
			if(CommLayer->GetCommandInfo(vecItemInfo[3],vecCmdInfo))
			{
				bPID=vecCmdInfo[2][0];
				if(bPID==0x01)
				{
					vecDSItemID01.push_back(binItemID);
				}
				if(bPID==0x41)
				{
					vecDSItemID41.push_back(binItemID);
				}
			}
		}
	}
	if((vecDSItemID01.size()>0)&&(vecDSItemID41.size()>0))
	{
		vecMenuID.clear();
		vecMenuID.push_back(CBinary("\x00\xFF\x05\x00\x00\x15",6));
		vecMenuID.push_back(CBinary("\x00\xFF\x05\x00\x00\x16",6));
		while(1)
		{
			iSelect=CommonTools->MenuAddItem(binTitle,vecMenuID);
			if(iSelect<0)
				break;
			if(iSelect==0)
			{
				DataListFunc->DataListFlash(vecMenuID[0],vecDSItemID01);
			}
			else
			{
				DataListFunc->DataListFlash(vecMenuID[1],vecDSItemID41);
			}
		}
	}
	else
	{
		DataListFunc->DataListFlash(binTitle,vecItemID);
	}

	return 1;
}
