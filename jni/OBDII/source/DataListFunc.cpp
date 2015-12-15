// DataListFunc.cpp: implementation of the CDataListFunc class.
//
//////////////////////////////////////////////////////////////////////
#include <time.h>
#include "DiagStd.h"
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
#include "sysstr.h"
#include "CommonTools.h"
#include "DrivenStd.h"
#include "LogCat.h"
#include "DataListFunc.h"
#include "AutoLog.h"

//extern void DebugPrint(char *fmt,...);
//extern CAutoLog * g_SystemAutoLog;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static const char* TAG = "CDataListFunc";

CDataListFunc::CDataListFunc()
{

}

CDataListFunc::~CDataListFunc()
{

}

short CDataListFunc::GetDataListVecItem()
{
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;
	CBinaryGroup vecItemID;
	CBinary binITEM_SUP=NULL;
	short iNum=0;
	short i=0;
	short iIndex=1;
	CDatabase dbData;
	unsigned char bDisFlag=0x00;
	unsigned char bType=0x00;

	if(Demo_Flag==true)
	{
		return 1;
	}
	
	Flag_DataList=false;
	if(!CommonTools->GetDataBase(dbData,"DATALIST.DB"))
	{
		dbData.Close();
		return 0;
	}
	binIndex.Add(bBrand_ID);
	binIndex+=binCommECU_ID;
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	binIndex.Add(0x00);

	CommLayer->InitHisData();
	vecItemID.clear();
	vecDataListIndex.clear();
	if(bBrand_ID==0x01)
	{
		OBDIIDataScan();
	}
	while(1)
	{
		CommonTools->UpdateBinIndex16(binIndex,iIndex++);
		if(!CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binIndex))
			break;
		bDisFlag=vecItemInfo[12][0];
		bType=vecItemInfo[13][0];
		if(bDisFlag==0x01)
		{
			if(bType&0x01)
			{
				// 判断CMD_ID是否存在
				// SCAN_TYPE
				if(ScanDataItem(vecItemInfo))
				{
					vecItemID.push_back(binIndex);
				}
			}
		}
	}

	iNum=vecItemID.size();
	if(iNum>0) // Check ITEM_SUP
	{
		for(i=0;i<iNum;i++)
		{
			binIndex=vecItemID[i];
			if(CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binIndex))
			{
				binITEM_SUP=vecItemInfo[5];
				if(CommonTools->IsCBinaryAvailable(binITEM_SUP))
				{
					if(CommonTools->GetIndexPosInVector(vecItemID,binITEM_SUP)>=0)
					{
						vecDataListIndex.push_back(binIndex);
					}
				}
				else
				{
					vecDataListIndex.push_back(binIndex);
				}
			}
		}
	}
	
	if(vecDataListIndex.size())
		Flag_DataList=true;

	dbData.Close();
	CommLayer->InitHisData();

	return 1;
}

short CDataListFunc::ScanDataItem(CBinaryGroup vecItemInfo)
{
	CBinary binScanType=NULL;
	unsigned char bType=0x00;
	short iRet=0;
	CBinaryGroup vecCmdInfo;
	unsigned char bSID=0x00;
	unsigned char bPID=0x00;
	CBinary binPID=NULL;
	CBinary binCmdInfo=NULL;
	CBinary binReceData=NULL;
	CBinary binPOSs=NULL;
	CBinary binMASKs=NULL;
	short iLen=0;

	binScanType=vecItemInfo[4];
	binPOSs=vecItemInfo[7];
	binMASKs=vecItemInfo[8];
	if(!CommonTools->IsCBinaryAvailable(binScanType))
		return 1;
	bType=binScanType[0];
	if(bType==0x00)
		return 1;
	if(bType==0x01)
	{
		iRet=CommLayer->ReadAndUpdateHisData(vecItemInfo[3],vecReceData);
		if(iRet!=Rece_Positive)
		{
			return 0;
		}
		if(!CommonTools->CheckCtrlPosAvail(vecReceData,binPOSs))
		{
			return 0;
		}
	}
	if((bType==0x02)||(bType==0x04))
	{
		if(!CommLayer->GetCommandInfo(vecItemInfo[3],vecCmdInfo))
		{
			return 0;
		}
		bSID=vecCmdInfo[1][0];
		if((bBrand_ID==0x01)&&(bSID==0x08)) // OBDII的Mode 0x08暂时不处理
		{
			return 0;
		}
		if(bSID==0x06)
		{
			bSID=0x06;
		}
		binPID=vecCmdInfo[2];
		bPID=binPID[0];
		binPID.SetAt(0,bPID&0xE0);
		binCmdInfo=NULL;
		binCmdInfo.Add(0x00);
		binCmdInfo.Add(bSID);
		binCmdInfo+=binPID;
		binCmdInfo.Add(0x01);
		iRet=CommLayer->ReadAndUpdateHisDataCmd(binCmdInfo,vecReceData);
		if(iRet!=Rece_Positive)
		{
			return 0;
		}
		binReceData=vecReceData[0];
		iLen=binReceData.GetSize();
		if(iLen<5)
			return 0;
		if(!CheckPIDSupp(binReceData,bPID,iLen-4))
		{
			return 0;
		}
		if((bBrand_ID==0x01)&&(bSID==0x01))
		{
			iRet=CommLayer->ReadAndUpdateHisData(vecItemInfo[3],vecReceData);
			if(iRet!=Rece_Positive)
			{
				return 0;
			}
			if(!CommonTools->CheckCtrlPosAvail(vecReceData,binPOSs))
			{
				return 0;
			}
			if(bType==0x04)
			{
				if(!CommonTools->CheckCtrlPosAvailFF(vecReceData,binPOSs))
				{
					return 0;
				}
			}
		}
		if((bBrand_ID==0x01)&&(bSID==0x02))
		{
			if(OBDIICheckFreezeData()) // OBDII Freeze Data 当有数据的时候，才去扫相关的数据流
			{
				iRet=CommLayer->ReadAndUpdateHisData(vecItemInfo[3],vecReceData);
				if(iRet!=Rece_Positive)
				{
					return 0;
				}
				if(!CommonTools->CheckCtrlPosAvail(vecReceData,binPOSs))
				{
					return 0;
				}
			}
		}
	}
	if(bType==0x03)
	{
		vector<unsigned short> veciPos;
		unsigned char bData=0x00;
		unsigned char bMask=0x00;

		if(!CommLayer->GetCommandInfo(vecItemInfo[3],vecCmdInfo))
		{
			return 0;
		}
		bSID=vecCmdInfo[1][0];
		binPID=vecCmdInfo[2];
		bPID=binPID[0];
		binPID.SetAt(0,bPID&0xE0);
		binCmdInfo=NULL;
		binCmdInfo.Add(0x00);
		binCmdInfo.Add(bSID);
		binCmdInfo+=binPID;
		binCmdInfo.Add(0x01);
		iRet=CommLayer->ReadAndUpdateHisDataCmd(binCmdInfo,vecReceData);
		if(iRet!=Rece_Positive)
		{
			return 0;
		}
		binReceData=vecReceData[0];
		iLen=binReceData.GetSize();
		if(iLen<5)
			return 0;
		if(!CheckPIDSupp(binReceData,bPID,iLen-4))
		{
			return 0;
		}
		iRet=CommLayer->ReadAndUpdateHisData(vecItemInfo[3],vecReceData);
		if(iRet!=Rece_Positive)
		{
			return 0;
		}
		binReceData=vecReceData[0];
		iLen=binReceData.GetSize();
		if(CommonTools->StrBinToVecInt(binPOSs,veciPos)<=0)
			return 0;
		if(iLen<=veciPos[0])
			return 0;
		bData=binReceData[veciPos[0]];
		bMask=binMASKs[0];
		if((bData&bMask)!=bMask)
			return 0;
	}

	return 1;
}

short CDataListFunc::CheckPIDSupp(CBinary binData,unsigned char bPID,short iPos)
{
	unsigned char bTemp=0x00;
	unsigned short iBitPos=0;
	unsigned short iBytePos=0;

	if(bPID==0x00)
		return 0;

	bTemp=bPID%0x20;
	iBytePos=(bTemp-1)/8;
	iBitPos=(bTemp-1)%8;

	if(binData.GetSize()<(iBytePos+iPos+1))
		return 0;

	bTemp=binData[iBytePos+iPos];

	if((bTemp>>(7-iBitPos))&0x01)
		return 1;

	return 0;
}

short CDataListFunc::OBDIIDataScan()
{
	CBinary binCmdInfo=NULL;
	CBinary binReceCmd=NULL;
	short i=0;
	short iRet=0;

	// Mode 0x01
	binCmdInfo=NULL;
	binCmdInfo.Add(0x00);
	binCmdInfo.Add(0x01);
	binCmdInfo.Add(0x00);
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
	
	// Mode 0x02
	binCmdInfo=NULL;
	binCmdInfo.Add(0x00);
	binCmdInfo.Add(0x02);
	binCmdInfo.Add(0x00);
	binCmdInfo.Add(0x00);
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

	// Mode 0x06
	binCmdInfo=NULL;
	binCmdInfo.Add(0x00);
	binCmdInfo.Add(0x06);
	binCmdInfo.Add(0x00);
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
	
// 	// Mode 0x08
// 	binCmdInfo=NULL;
// 	binCmdInfo.Add(0x00);
// 	binCmdInfo.Add(0x08);
// 	binCmdInfo.Add(0x00);
// 	binCmdInfo.Add(0x01);
// 	for(i=0;i<8;i++)
// 	{
// 		binCmdInfo.SetAt(2,i*0x20);
// 		iRet=CommLayer->SendReceiveCmd(binCmdInfo,vecReceData);
// 		CommLayer->UpdateHisReceDataCmd(binCmdInfo,iRet,vecReceData);
// 		if(iRet==Rece_Positive)
// 		{
// 			binReceCmd=vecReceData[0];
// 			if((binReceCmd[binReceCmd.GetSize()-1]&0x01)!=0x01)
// 				break;
// 		}
// 		else
// 		{
// 			break;
// 		}
// 	}
// 	for(;i<8;i++)
// 	{
// 		binCmdInfo.SetAt(2,i*0x20);
// 		iRet=Rece_Error;
// 		vecReceData.clear();
// 		CommLayer->UpdateHisReceDataCmd(binCmdInfo,iRet,vecReceData);
// 	}

	// Mode 0x09
	binCmdInfo=NULL;
	binCmdInfo.Add(0x00);
	binCmdInfo.Add(0x09);
	binCmdInfo.Add(0x00);
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

	return 1;
}

void CDataListFunc::DataListFuncInit()
{
	FirstTimeInit=false;
	fCalResult=0.0;
}

short CDataListFunc::DataListShow(CBinary binTitle)
{
	CBinaryGroup vecDSItemID;
	CBinaryGroup vevDSGrpStrID;
	short iNum=0;
	short iRet=0;
	short i=0;
	short iSelect=0;

	iNum=GetDSGrpStrIDs(vevDSGrpStrID);
	if(!iNum)
	{
		if(!GetDSGrpItems(vecDSItemID,0x00))
			return 0;
		iRet=DataListFlash(binTitle,vecDSItemID);
	}
	else
	{
		while(1)
		{
			iSelect=CommonTools->MenuAddItem(binTitle,vevDSGrpStrID);
			if(iSelect<0)
				break;
			if(GetDSGrpItems(vecDSItemID,iSelect+1))
			{
				iRet=DataListFlash(vevDSGrpStrID[iSelect],vecDSItemID);
			}
		}
	}

	return iRet;
}

short CDataListFunc::SystemInfoShow(CBinary binTitle,CBinaryGroup vecAllItemID)
{
	CDatabase dbData;
	short i=0;
	short iNum=0;
	vector<string> vecAllItemName;
	vector<string> vecAllItemUnit;
	string StringValue="";

	CDisplay::GetInstance()->MessageBox(STRID_COMMUNICATING,binTitle,adsMB_NoButton);
	
	iNum=GetItemTxtInfo(vecAllItemID,vecAllItemName,vecAllItemUnit);

	if(!iNum)
	{
		CDisplay::GetInstance()->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x09",6),binTitle);
		return 0;
	}

	if(!CommonTools->GetDataBase(dbData,"DATALIST.DB"))
	{
		dbData.Close();
		return 0;
	}
	CommLayer->InitHisData();
	CDisplay::GetInstance()->VehicleInfo.Init(binTitle);
	for(i=0;i<iNum;i++)
	{
		if(!ReadDataAndCalculate(dbData,vecAllItemID[i],StringValue))
		{
			StringValue="*****";
		}
		CDisplay::GetInstance()->VehicleInfo.Add(vecAllItemName[i],StringValue);
	}
	CDisplay::GetInstance()->VehicleInfo.Show();

	dbData.Close();
	return 1;
}

short CDataListFunc::DataListFlash(CBinary binTitle,CBinaryGroup vecAllItemID)
{
	/*LOG_DEBUG(TAG, "****DataListFlash****");*/
	CMultiSelectShow::CSelectedItemData StreamSelected;
	vector<string> vecAllItemName;
	vector<string> vecAllItemUnit;
	vector<string> vecSelItemName;
	vector<string> vecSelItemUnit;
	vector<CBinary> vecSelItemID;
	string StringValue;
	short iAllItemNum=0;
	short i=0;
	short j=0;
	bool Flag_Success=false;
	CDatabase dbData;

	if(!vecAllItemID.size())
		return 0;
	vecAllItemName.clear();
	vecAllItemUnit.clear();
	iAllItemNum=GetItemTxtInfo(vecAllItemID,vecAllItemName,vecAllItemUnit);

	while(true)
	{
		CDisplay::GetInstance()->MultiSelect.Init(binTitle);
		for(i=0;i<iAllItemNum;i++)
		{
			StringValue=vecAllItemName[i];
			CDisplay::GetInstance()->MultiSelect.Add(vecAllItemName[i]);
		}
		CDisplay::GetInstance()->MultiSelect.SetFlag(true);
		CDisplay::GetInstance()->MultiSelect.AddMsg(adsGetTextString(CBinary("\x00\xFF\x00\x00\x00\x08",6)));
		if(!CDisplay::GetInstance()->MultiSelect.Show(StreamSelected))
		{
			break;
		}
		else
		{
			DataListFuncInit();
			vecSelItemName.clear();
			vecSelItemUnit.clear();
			vecSelItemID.clear();
			for(i=0;i<iAllItemNum;i++)
			{
				if(StreamSelected.IsSelected(i))
				{
					vecSelItemID.push_back(vecAllItemID[i]);
					vecSelItemName.push_back(vecAllItemName[i]);
					vecSelItemUnit.push_back(vecAllItemUnit[i]);
				}
			}
			if(!vecSelItemName.size()) // 在不选的情况下，全选
			{
				for(i=0;i<iAllItemNum;i++)
				{
					vecSelItemID.push_back(vecAllItemID[i]);
					vecSelItemName.push_back(vecAllItemName[i]);
					vecSelItemUnit.push_back(vecAllItemUnit[i]);
				}
			}

			//初始化
			unsigned short uiTop = 0;
			unsigned short uiAll = vecSelItemID.size();
			unsigned short uiNum = uiAll;//uiNum保存当前屏的数据流条数
			short uiBottom=uiTop+uiNum;
			unsigned char iCountFlag = 0;
			unsigned char uiFlag;
			/*bool bChanged = false;
			vector<string> vSaveValue;*/
			//vSaveValue.reserve((uiNum < vSaveValue.max_size()) ? uiNum : vSaveValue.max_size());
			if(!CommonTools->GetDataBase(dbData,"DATALIST.DB"))
			{
				dbData.Close();
				return 0;
			}
			while(true)
			{
				CommLayer->InitHisData();
				CDisplay::GetInstance()->DataStream.Init(uiTop,uiAll,adsGetTextString(binTitle));
				uiBottom=uiTop+uiNum;
				if(uiBottom>uiAll)
					uiBottom = uiAll;

				Flag_Success=false;
				/*bChanged = false;*/
				for(i=uiTop;i<uiBottom;i++)
				{
					if(!iCountFlag) //第一次进入时先显示---
					{
						/*LOG_DEBUG(TAG, "****The first show****");*/
						iCountFlag=1;
						for(i=uiTop;i<uiBottom;i++)
						{
							/* 初始化数据流的值,所有的数据流值都初始为 "---" */
							/*vSaveValue.push_back("---");*/
							CDisplay::GetInstance()->DataStream.Add(vecSelItemName[i],"---",vecSelItemUnit[i]);
						}
						Flag_Success=true;
						break;
					}
					else
					{
						uiFlag = CDisplay::GetInstance()->DataStream.AcceptMsg();

						if(1 == uiFlag || 2 == uiFlag)//1:下翻 2:上翻
						{
							/*LOG_DEBUG(TAG, "****Page up or down, show the data stream was left****");*/
							iCountFlag = 0;
							for(j=i;j<uiBottom;j++)//将还没通讯的显示完
							{
								CDisplay::GetInstance()->DataStream.Add(vecSelItemName[i],"---",vecSelItemUnit[i]);
							}
							Flag_Success=true;
							break;
						}
						if(ReadDataAndCalculate(dbData,vecSelItemID[i],StringValue))
						{
							Flag_Success=true;
						}
/*
						 如果有一条数据流的值发生了改变，则更新显示
						if (StringValue != vSaveValue[i])
						{
							vSaveValue[i] = StringValue;
							bChanged = true;
						}
						LOG_DEBUG(TAG, "Item :%s %s %s", vecSelItemName[i].c_str(), StringValue.c_str(), vecSelItemUnit[i].c_str());*/
						CDisplay::GetInstance()->DataStream.Add(vecSelItemName[i],StringValue,vecSelItemUnit[i]);
					}
					
				}
				if(Flag_Success==false)
				{
					dbData.Close();
					return 0;
				}

				if (CDisplay::GetInstance()->DataStream.Show(uiTop, uiNum) == 0x0b)
				{
					LOG_DEBUG(TAG, "return from data stream");
					DataListEndComm();
					break;
				}
			}
			dbData.Close();
		}
	}

	return 1;
}

short CDataListFunc::GetDSGrpStrIDs(CBinaryGroup &vecItemStrID)
{
	CBinary binTxtID=NULL;
	CBinary binItemID=NULL;
	CBinaryGroup vecItemInfo;
	
	vecItemStrID.clear();

	binItemID.Add(bBrand_ID);
	binItemID+=binCommECU_ID;
	binItemID.Add(0x01);
	binItemID.Add(0x00);
	binItemID.Add(0x01);
	
	while(1)
	{
		if(!CommonTools->ReadDBFile(vecItemInfo,"DSGRP.DB",binItemID))
			break;
		binTxtID=vecItemInfo[0];
		CommonTools->GetTxtIndex(binTxtID);
		vecItemStrID.push_back(binTxtID);
		binItemID.SetAt(5,binItemID[5]+1);
	}

	return vecItemStrID.size();
}

short CDataListFunc::GetDSGrpItems(CBinaryGroup &vecItemID,unsigned char bGrp)
{
	short i=0;
	CBinary binItemID=NULL;
	CBinaryGroup vecItemInfo;
	CBinary binDSID=NULL;
	unsigned short iIndex=0;

	vecItemID.clear();
	if(bGrp==0) //All Data
	{
		for(i=0;i<vecDataListIndex.size();i++)
		{
			vecItemID.push_back(vecDataListIndex[i]);
		}
	}
	else
	{
		binItemID.Add(bBrand_ID);
		binItemID+=binCommECU_ID;
		binItemID.Add(bGrp);
		binItemID.Add(0x00);
		binItemID.Add(0x01);
		iIndex=1;
		while(1)
		{
			binItemID.SetAt(6,(iIndex>>8)&0xFF);
			binItemID.SetAt(7,iIndex&0xFF);
			if(!CommonTools->ReadDBFile(vecItemInfo,"DSGRP.DB",binItemID))
				break;
			binDSID=vecItemInfo[1];
			if(Demo_Flag==true)
			{
				vecItemID.push_back(binDSID);
			}
			else
			if(CommonTools->GetIndexPosInVector(vecDataListIndex,binDSID)>=0)
			{
				vecItemID.push_back(binDSID);
			}
			iIndex++;
		}
	}

	return vecItemID.size();
}

short CDataListFunc::GetItemTxtInfo(CBinaryGroup vecItemID,vector<string> &vecItemName,vector<string> &vecItemUnit)
{
	CDatabase dbData;
	short i=0;
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;
	CBinary binTxtIndex=NULL;
	
	if(!CommonTools->GetDataBase(dbData,"DATALIST.DB"))
	{
		dbData.Close();
		return 0;
	}
	for(i=0;i<vecItemID.size();i++)
	{
		binIndex=vecItemID[i];
		if(CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binIndex))
		{
			binTxtIndex=vecItemInfo[1];
			CommonTools->GetTxtIndex(binTxtIndex);
			vecItemName.push_back(adsGetTextString(binTxtIndex));
			binTxtIndex=vecItemInfo[2];
			if(CommonTools->IsCBinaryAvailable(binTxtIndex))
			{
				CommonTools->GetTxtIndex(binTxtIndex);
				vecItemUnit.push_back(adsGetTextString(binTxtIndex));
			}
			else
			{
				vecItemUnit.push_back("");
			}
		}
	}
	dbData.Close();

	return vecItemName.size();
}

short CDataListFunc::DataListInitComm()
{
	return 1;
}

short CDataListFunc::DataListEndComm()
{
	FirstTimeInit=false;
	return 1;
}

short CDataListFunc::ReadDataAndCalculate(CDatabase &dbData,CBinary binItemIndex,string &StringValue)
{
	if (CRunEnvironment::GetDemoMode())
	{
		srand(clock());
		int value = rand() % 256;
		char szValue[16] = {0};
		sprintf(szValue, "%d", value);
		StringValue = szValue;
		return 1;
	}

	vector<CBinary> vecItemInfo;
	CBinary binCmdID=NULL;
	CBinary binData=NULL;
	short iRet=0;
	
	if(FirstTimeInit==false)
	{
		FirstTimeInit=true;
		DataListInitComm(); //数据流初始化通讯(动态数据流)
	}

	StringValue="*****";
	
	if(!CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binItemIndex))
		return 0;
	binCmdID=vecItemInfo[3];
	iRet=CommLayer->ReadAndUpdateHisData(binCmdID,vecReceData);
	if(iRet==Rece_Error)
	{
		StringValue="*****";
		return 0;
	}
	else if(iRet<0)
	{
		StringValue="*****";
	}
	else
	{
		if(!CommonTools->GetCtrlData(vecReceData,vecItemInfo[7],vecItemInfo[8],binData))
		{
			StringValue="*****";
			return 1;
		}
	}
	if(!GetStringValue(vecItemInfo[11][0],vecItemInfo[6],binData,StringValue))
	{
		StringValue="*****";
		return 1;
	}

	return 1;
}

short CDataListFunc::GetStringValue(unsigned char bType,CBinary binConvID,CBinary binData,string &strResult)
{
	short iRet=0;

	strResult="";
	switch(bType)
	{
	case 0: // General
		iRet=DisType0(binData,binConvID,strResult);
		break;
	case 1: // Hex Data
		iRet=DisType1(binData,strResult);
		break;
	case 2: // ASCII
		iRet=DisType2(binData,strResult);
		break;
	case 3: // DTC PCBU
		iRet=DisType3(binData,strResult);
		break;
	case 4: // Date Year.Month.Day
		break;
	case 5: // Time hr:min:sec
		break;
	default:
		break;
	}

	return iRet;
}

short CDataListFunc::DisType0(CBinary binData,CBinary binConvID,string &strResult)
{
	unsigned char bMethod=0x00;
	CBinary binIndex;
	CBinaryGroup vecItemInfo;
	CDatabase dbData;
	bool Flag_Success=false;
	string strValue="";
	CBinary binLBound=NULL;
	CBinary binUBound=NULL;
	CBinary binTemp=NULL;

	if(!CommonTools->GetDataBase(dbData,"CONVERSION.DB"))
	{
		dbData.Close();
		return 0;
	}
	binIndex.Add(bBrand_ID);
	binIndex+=binConvID;
	binIndex.Add(0x00);

	binTemp=binData;
	
	while(1)
	{
		if(!CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binIndex))
			break;
		bMethod=vecItemInfo[0][0];
		binLBound=vecItemInfo[1];
		binUBound=vecItemInfo[2];
		if((bMethod==CONV_STATUS_BIT_1)||(bMethod==CONV_STATUS_BIT_N))
		{
			binData=NULL;
			binData.Add(binTemp[0]&binLBound[0]);
		}
		if(DataInScope(binData,binLBound,binUBound))
		{
			Flag_Success=true;
			switch(bMethod)
			{
			case CONV_LINEARITY:
				Conv_Linearity(vecItemInfo,binData,strValue);
				break;
			case CONV_STATUS:
				Conv_Status(vecItemInfo,binData,strValue);
				break;
			case CONV_STATUS_BIT_1:
				Conv_Status(vecItemInfo,binData,strValue);
				break;
			case CONV_STATUS_BIT_N:
				Conv_Status(vecItemInfo,binData,strValue);
				break;
			case CONV_DISPLAY:
				Conv_Display(vecItemInfo,binData,strValue);
				break;
			case CONV_SPE1:
				Conv_Spe1(vecItemInfo,binData,strValue);
				break;
			case CONV_SPE2:
				Conv_Spe2(vecItemInfo,binData,strValue);
				break;
			case CONV_SPE3:
				Conv_Spe3(vecItemInfo,binData,strValue);
				break;
			case CONV_SPE5:
				Conv_Spe5(vecItemInfo,binData,strValue);
				break;
			default:
				break;
			}
			if(bMethod==CONV_STATUS_BIT_N)
			{
				if(strResult.size()>0)
				{
					strResult+="\n";
					strResult+=strValue;
				}
				else
				{
					strResult=strValue;
				}
			}
			else
			{
				strResult=strValue;
				break;
			}
		}
		binIndex.SetAt(5,binIndex[5]+1);
	}

	dbData.Close();
	if(Flag_Success==false)
		return 0;

	return 1;
}

short CDataListFunc::DataInScope(CBinary binData,CBinary binMin,CBinary binMax)
{
	bool Flag_Match=true;
	short i=0;
	short iNum=0;
	bool Flag=false;

	if(CommonTools->IsCBinaryAvailable(binMin))
	{
		iNum=binData.GetSize();
		if(iNum<binMin.GetSize())
		{
			for(i=iNum;i<binMin.GetSize();i++)
			{
				binData.Add(0x00);
			}
		}
		else if(iNum>binMin.GetSize())
		{
			for(i=binMin.GetSize();i<iNum;i++)
			{
				binMin.Add(0x00);
			}
		}
		iNum=binData.GetSize();
		for(i=iNum-1;i>=0;i--)
		{
			if(binData[i]>binMin[i])
			{
				Flag=true;
				break;
			}
			else if(binData[i]==binMin[i])
			{
				Flag=true;
			}
			else
			{
				Flag=false;
				break;
			}
		}
		if(Flag==false)
			return 0;
	}
	if(CommonTools->IsCBinaryAvailable(binMax))
	{
		iNum=binData.GetSize();
		if(iNum>binMax.GetSize())
		{
			for(i=binMax.GetSize();i<iNum;i++)
			{
				binMax.Add(0x00);
			}
		}
		else if(iNum<binMax.GetSize())
		{
			for(i=iNum;i<binMax.GetSize();i++)
			{
				binData.Add(0x00);
			}
		}
		iNum=binData.GetSize();
		for(i=iNum-1;i>=0;i--)
		{
			if(binData[i]>binMax[i])
			{
				Flag=false;
				break;
			}
			else if(binData[i]==binMax[i])
			{
				Flag=true;
			}
			else
			{
				Flag=true;
				break;
			}
		}
		if(Flag==false)
			return 0;
	}

	return 1;
}

string CDataListFunc::GetNumDisFormat(unsigned char bNumOfDec)
{
	string strResult="";
	char Str_Str[100];

	switch(bNumOfDec)
	{
	case 0x00:
		sprintf(Str_Str,"%.0f",fCalResult);
		break;
	case 0x01:
		sprintf(Str_Str,"%.1f",fCalResult);
		break;
	case 0x02:
		sprintf(Str_Str,"%.2f",fCalResult);
		break;
	case 0x03:
		sprintf(Str_Str,"%.3f",fCalResult);
		break;
	case 0x04:
		sprintf(Str_Str,"%.4f",fCalResult);
		break;
	case 0x05:
		sprintf(Str_Str,"%.5f",fCalResult);
		break;
	case 0x06:
		sprintf(Str_Str,"%.6f",fCalResult);
		break;
	case 0x07:
		sprintf(Str_Str,"%.7f",fCalResult);
		break;
	case 0x08:
		sprintf(Str_Str,"%.8f",fCalResult);
		break;
	default:
		sprintf(Str_Str,"%.9f",fCalResult);
		break;
	}
	strResult=Str_Str;

	return strResult;
}

short CDataListFunc::DisType1(CBinary binData,string &strResult)
{
	char strstr[200];
	short iLen=0;
	short i=0;
	short j=0;
	
	memset(strstr,sizeof(strstr),0);
	
	iLen=binData.GetSize();
	if((iLen<1)||(iLen>99))
	{
		return 0;
	}
	for(i=iLen-1;i>=0;i--)
	{
		sprintf(strstr+j*2,"%02X",binData[i]);
		j++;
	}
	
	strResult=strstr;
	
	return 1;
}

short CDataListFunc::DisType2(CBinary binData,string &strResult)
{
	char strstr[200];
	short iLen=0;
	short i=0;
	short j=0;
	
	memset(strstr,sizeof(strstr),0);
	
	iLen=binData.GetSize();
	if((iLen<1)||(iLen>199))
	{
		return 0;
	}
	for(i=iLen-1;i>=0;i--)
	{
		sprintf(strstr+j,"%C",binData[i]);
		j++;
	}

	strResult=strstr;
	
	return 1;
}

short CDataListFunc::DisType3(CBinary binData,string &strResult)
{
	char strstr[20];
	short iLen=0;
	unsigned char bTemp=0;

	memset(strstr,sizeof(strstr),0);
	iLen=binData.GetSize();
	if(iLen!=2)
	{
		return 0;
	}
	bTemp=binData[1];
	switch(bTemp&0xC0)
	{
	case 0x00:
		sprintf(strstr,"P%02X%02X",bTemp&0x3F,binData[0]);
		break;
	case 0x40:
		sprintf(strstr,"C%02X%02X",bTemp&0x3F,binData[0]);
		break;
	case 0x80:
		sprintf(strstr,"B%02X%02X",bTemp&0x3F,binData[0]);
		break;
	case 0xC0:
		sprintf(strstr,"U%02X%02X",bTemp&0x3F,binData[0]);
		break;
	default:
		break;
	}

	strResult=strstr;

	return 1;
}

short CDataListFunc::Conv_Linearity(CBinaryGroup vecItemInfo,CBinary binData,string &strResult)
{
	unsigned char bSigned=0x00;
	double fParam=0.0;
	double fOffset=0.0;
	double fTemp=0.0;
	short i=0;
	short j=0;
	short iNum=0;
	unsigned char bNumOfDec=0x00;
	signed char sVal=0;
	CBinary binTxtID=NULL;

	strResult="";
	if(CommonTools->IsCBinaryAvailable(vecItemInfo[3]))
	{
		bSigned=vecItemInfo[3][0];
	}
	fParam=CommonTools->StrBinToFLOAT(vecItemInfo[4]);
	if(CommonTools->IsCBinaryAvailable(vecItemInfo[8]))
	{
		fOffset=CommonTools->StrBinToFLOAT(vecItemInfo[8]);
	}
	if(CommonTools->IsCBinaryAvailable(vecItemInfo[9]))
	{
		bNumOfDec=vecItemInfo[9][0];
	}
	fCalResult=0.0;
	iNum=binData.GetSize();
	for(i=iNum-1;i>=0;i--)
	{
		if((i==(iNum-1))&&(bSigned==0x01))
		{
			sVal=binData[i];
			fTemp=sVal*fParam;
		}
		else
		{
			fTemp=binData[i]*fParam;
		}
		if(i>0)
		{
			for(j=0;j<i;j++)
			{
				fTemp=fTemp*256;
			}
		}
		fCalResult+=fTemp;
	}
	fCalResult+=fOffset;

	binTxtID=vecItemInfo[10];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}
	strResult+=GetNumDisFormat(bNumOfDec);
	binTxtID=vecItemInfo[11];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}
	return 1;
}

short CDataListFunc::Conv_Status(CBinaryGroup vecItemInfo,CBinary binData,string &strResult)
{
	CBinary binTxtID=NULL;

	binTxtID=vecItemInfo[10];
	strResult=CommonTools->GetTxtString(binTxtID);
	return 1;
}

short CDataListFunc::Conv_Display(CBinaryGroup vecItemInfo,CBinary binData,string &strResult)
{
	char Str_Str[100];
	char Str_Format[100];
	char ch;
	char Str_Temp[20];
	short iByteNum=0;
	short i=0;
	short j=0;
	short k=0;
	short Str_Len=0;
	CBinary binStrID1=NULL;
	CBinary binStrID2=NULL;
	
	iByteNum=binData.GetSize();
	
	vecItemInfo[12].ReadBuffer(Str_Format);
	Str_Len=strlen(Str_Format);
	memset(Str_Str,0,sizeof(Str_Str));
	i=0;
	k=0;
	while(i<Str_Len)
	{
		ch=Str_Format[i++];
		if(ch=='%')
		{
			memset(Str_Temp,0,sizeof(Str_Temp));
			j=0;
			Str_Temp[j++]=ch;
			while(i<Str_Len)
			{
				ch=Str_Format[i];
				if(ch=='%')
				{
					break;
				}
				else
				{
					Str_Temp[j++]=ch;
					i++;
				}
			}
		}
		if(strlen(Str_Temp)>1)
		{
			if(CommonTools->IsBufOverFlow(binData,k)==false)
				sprintf(Str_Str+strlen(Str_Str),Str_Temp,binData[k++]);
		}
	}

	strResult="";
	binStrID1=vecItemInfo[10];
	binStrID2=vecItemInfo[11];
	if(CommonTools->IsCBinaryAvailable(binStrID1))
	{
		strResult+=CommonTools->GetTxtString(binStrID1);
	}
	strResult+=Str_Str;
	if(CommonTools->IsCBinaryAvailable(binStrID2))
	{
		strResult+=CommonTools->GetTxtString(binStrID2);
	}
	
	return 1;
}

double CDataListFunc::GetCalculateResult()
{
	return fCalResult;
}


short CDataListFunc::OBDIICheckFreezeData()
{
	CBinary binSendCmd=NULL;
	CBinary binReceData=NULL;
	short iRet=0;
	
	binSendCmd=CBinary("\x00\x02\x02\x00\x01",5);
	iRet=CommLayer->ReadAndUpdateHisDataCmd(binSendCmd,vecReceData);
	if(iRet!=Rece_Positive)
		return 0;
	binReceData=vecReceData[0];
	if(binReceData.GetSize()<5)
		return 0;
	if((binReceData[3]==0x00)&&(binReceData[4]==0x00))
		return 0;
	
	return 1;
}

short CDataListFunc::Conv_Spe1(CBinaryGroup vecItemInfo,CBinary binData,string &strResult)
{
	long lParam=0;
	long lTemp=0;
	short i=0;
	short j=0;
	short iNum=0;
	unsigned char bNumOfDec=0x00;
	CBinary binTxtID=NULL;
	long lCalResult=0;
	
	strResult="";
	lParam=CommonTools->StrBinToFLOAT(vecItemInfo[4]);
	if(CommonTools->IsCBinaryAvailable(vecItemInfo[9]))
	{
		bNumOfDec=vecItemInfo[9][0];
	}
	lCalResult=0;
	iNum=binData.GetSize();
	for(i=iNum-1;i>=0;i--)
	{
		lTemp=binData[i]*lParam;
		if(i>0)
		{
			for(j=0;j<i;j++)
			{
				lTemp=lTemp*256;
			}
		}
		lCalResult+=lTemp;
	}
	lParam=CommonTools->StrBinToFLOAT(vecItemInfo[5]);
	lCalResult+=lParam;
	lParam=CommonTools->StrBinToFLOAT(vecItemInfo[6]);
	lCalResult=lCalResult%lParam;

	fCalResult=lCalResult;

	binTxtID=vecItemInfo[10];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}
	strResult+=GetNumDisFormat(0);
	binTxtID=vecItemInfo[11];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}

	return 1;
}

short CDataListFunc::Conv_Spe2(CBinaryGroup vecItemInfo,CBinary binData,string &strResult)
{
	unsigned char bSigned=0x00;
	double fParam=0.0;
	double fTemp=0.0;
	short i=0;
	short j=0;
	short iNum=0;
	unsigned char bNumOfDec=0x00;
	signed char sVal=0;
	CBinary binTxtID=NULL;
	
	strResult="";
	if(CommonTools->IsCBinaryAvailable(vecItemInfo[3]))
	{
		bSigned=vecItemInfo[3][0];
	}
	fCalResult=0.0;
	fParam=1.0;
	iNum=binData.GetSize();
	for(i=iNum-1;i>=0;i--)
	{
		if((i==(iNum-1))&&(bSigned==0x01))
		{
			sVal=binData[i];
			fTemp=sVal*fParam;
		}
		else
		{
			fTemp=binData[i]*fParam;
		}
		if(i>0)
		{
			for(j=0;j<i;j++)
			{
				fTemp=fTemp*256;
			}
		}
		fCalResult+=fTemp;
	}
	if(fCalResult==0.0)
	{
		return 0;
	}
	fCalResult=1/fCalResult;

	fParam=CommonTools->StrBinToFLOAT(vecItemInfo[4]);
	fCalResult=fCalResult*fParam;
	fParam=CommonTools->StrBinToFLOAT(vecItemInfo[5]);
	fCalResult=fCalResult+fParam;

	if(CommonTools->IsCBinaryAvailable(vecItemInfo[9]))
	{
		bNumOfDec=vecItemInfo[9][0];
	}
	
	binTxtID=vecItemInfo[10];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}
	strResult+=GetNumDisFormat(bNumOfDec);
	binTxtID=vecItemInfo[11];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}
	return 1;
}

short CDataListFunc::Conv_Spe3(CBinaryGroup vecItemInfo,CBinary binData,string &strResult)
{
	long lParam=0;
	long lTemp=0;
	double fParam=0;
	short i=0;
	short j=0;
	short iNum=0;
	unsigned char bNumOfDec=0x00;
	CBinary binTxtID=NULL;
	long lCalResult=0;
	
	strResult="";
	lCalResult=0;
	iNum=binData.GetSize();
	for(i=iNum-1;i>=0;i--)
	{
		lTemp=binData[i]*1;
		if(i>0)
		{
			for(j=0;j<i;j++)
			{
				lTemp=lTemp*256;
			}
		}
		lCalResult+=lTemp;
	}
	lParam=CommonTools->StrBinToFLOAT(vecItemInfo[6]);
	lCalResult=lCalResult%lParam;
	fParam=CommonTools->StrBinToFLOAT(vecItemInfo[4]);
	fCalResult=lCalResult*fParam;
	fParam=CommonTools->StrBinToFLOAT(vecItemInfo[5]);
	fCalResult=lCalResult+fParam;

	if(CommonTools->IsCBinaryAvailable(vecItemInfo[9]))
	{
		bNumOfDec=vecItemInfo[9][0];
	}
		
	binTxtID=vecItemInfo[10];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}
	strResult+=GetNumDisFormat(bNumOfDec);
	binTxtID=vecItemInfo[11];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}
	
	return 1;
}

short CDataListFunc::Conv_Spe5(CBinaryGroup vecItemInfo,CBinary binData,string &strResult)
{
	unsigned char bSigned=0x00;
	double fParam=0.0;
	double fTemp=0.0;
	short i=0;
	short j=0;
	short iNum=0;
	unsigned char bNumOfDec=0x00;
	signed char sVal=0;
	CBinary binTxtID=NULL;
	
	strResult="";
	if(CommonTools->IsCBinaryAvailable(vecItemInfo[3]))
	{
		bSigned=vecItemInfo[3][0];
	}
	fParam=CommonTools->StrBinToFLOAT(vecItemInfo[4]);
	if(CommonTools->IsCBinaryAvailable(vecItemInfo[9]))
	{
		bNumOfDec=vecItemInfo[9][0];
	}
	fCalResult=0.0;
	iNum=binData.GetSize();
	for(i=iNum-1;i>=0;i--)
	{
		if((i==(iNum-1))&&(bSigned==0x01))
		{
			sVal=binData[i];
			fTemp=sVal*1;
		}
		else
		{
			fTemp=binData[i]*1;
		}
		if(i>0)
		{
			for(j=0;j<i;j++)
			{
				fTemp=fTemp*256;
			}
		}
		fCalResult+=fTemp;
	}
	fCalResult+=fParam;
	fParam=CommonTools->StrBinToFLOAT(vecItemInfo[5]);
	if(fParam==0.0)
	{
		return 0;
	}
	fCalResult=fCalResult/fParam;
	
	binTxtID=vecItemInfo[10];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}
	strResult+=GetNumDisFormat(bNumOfDec);
	binTxtID=vecItemInfo[11];
	if(CommonTools->IsCBinaryAvailable(binTxtID))
	{
		strResult+=CommonTools->GetTxtString(binTxtID);
	}
	return 1;
}
