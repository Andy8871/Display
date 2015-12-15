// VInfo.cpp: implementation of the CVInfo class.
//
//////////////////////////////////////////////////////////////////////
#include <android/log.h>
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
#include "CommonTools.h"
#include "DrivenStd.h"

#include "BasicDiagnosticUnit.h"
#include "VInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVInfo::CVInfo()
{

}

CVInfo::~CVInfo()
{

}

short CVInfo::StartShow()
{
	vector<CBinaryGroup> vecModelGrpInfo;
	CBinary binIndex("\x00\x00\x00\x01",4);
	CBinaryGroup vecItemInfo;
	CBinary binTitle("\x00\xFF\x04\x00\x00\x00",6);
	CBinaryGroup vecMenuItem;
	short iSelect=0;
	CBasicDiagnosticUnit BasicDiagUnit;
	
	if(!CommonTools->ReadDBFile(vecItemInfo,"BID.DB",binIndex))
	{
		__android_log_write(ANDROID_LOG_DEBUG, "CVInfo",
							"Unable to read file BID.DB");
		return 0;
	}
	bBrand_ID=vecItemInfo[0][0];

	binTitle.SetAt(5,bBrand_ID);

	if(bBrand_ID==0x01) // OBDII
	{
		BasicDiagUnit.OBDIIShow();
	}
	else
	{
		if(!GetSelectMenu(vecMenuItem))
			return 0;

		while(1)
		{
			iSelect=CommonTools->MenuAddItem(binTitle,vecMenuItem);
			if(iSelect<0)
				break;
			switch(vecMenuItem[iSelect][5])
			{
			case 0x01:
				vecModelGrpInfo.clear();
				ShowVehicleInfo(vecModelGrpInfo);
				break;
			default:
				break;
			}
		}
	}

	return 1;
}

short CVInfo::GetSelectMenu(CBinaryGroup &vecMenuItem)
{
	CBinary binIndex=NULL;

	vecMenuItem.clear();
	binIndex=CBinary("\x00\xFF\x03\x00\x00\x01",6);

	vecMenuItem.push_back(binIndex);

	return vecMenuItem.size();
}

short CVInfo::ShowVehicleInfo(vector<CBinaryGroup> vecModelGrpInfo)
{
	vector<CBinaryGroup> vecOrgModelGrpInfo;
	CBinaryGroup vecItemIndex;
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;
	vector<string> vecItemName;
	CDatabase dbData;
	CBinary binTxtID=NULL;
	string strTxt="";
	CBinary binTitle("\x00\xFF\x00\x00\x00\x01",6);
	short i=0;
	short iLevelNum=0;
	short iItemNum=0;
	CBinaryGroup vecModelInfo;
	CBinary binOriginalItemNum=NULL;
	CBinary binSelItem=NULL;
	bool Flag_MultSelect=false;
	string strValue="";
	short iSelLevel=-1;
	short iSelectNum=0;

	vecOrgModelGrpInfo.clear();
	binVehicleID=NULL;
	vecItemName.clear();
	if(!CommonTools->GetDataBase(dbData,"LEVELNAME.DB"))
	{
		dbData.Close();
		return 0;
	}
	binIndex.Add(bBrand_ID);
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	while(1)
	{
		binIndex.SetAt(5,binIndex[5]+1);
		if(!CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binIndex))
			break;
		binTxtID=vecItemInfo[1];
		strTxt=CommonTools->GetTxtString(binTxtID);
		vecItemName.push_back(strTxt);

	}
	dbData.Close();
	iLevelNum=vecItemName.size();
	if(!iLevelNum)
	{
		return 0;
	}
	for(i=0;i<iLevelNum;i++)
	{
		if(vecModelGrpInfo.size()>i)
		{
			vecModelInfo=vecModelGrpInfo[i];
			vecOrgModelGrpInfo.push_back(vecModelInfo);
		}
		else
		{
			vecModelInfo.clear();
			vecOrgModelGrpInfo.push_back(vecModelInfo);
		}
	}
	if(!GetModelGrpInfo(vecOrgModelGrpInfo,vecModelGrpInfo,vecItemIndex))
		return 0;
	if(!GetVehicleID(vecItemIndex))
		return 0;

	while(1)
	{
		CDisplay::GetInstance()->ActiveTest.Init(binTitle);
		CDisplay::GetInstance()->ActiveTest.SetFlag(0x8F);
		CDisplay::GetInstance()->ActiveTest.AddMsg(adsGetTextString(CBinary("\x00\xFF\x00\x00\x00\x02",6)));
		CDisplay::GetInstance()->ActiveTest.AddButton(CBinary("\x00\xFF\x00\x00\x00\x03",6),CActiveTestShow::BS_MASK); // Exit
		if(binVehicleID.GetSize())
			CDisplay::GetInstance()->ActiveTest.AddButton(CBinary("\x00\xFF\x00\x00\x00\x04",6),CActiveTestShow::BS_MASK); // Next
		else
			CDisplay::GetInstance()->ActiveTest.AddButton(CBinary("\x00\xFF\x00\x00\x00\x04",6),CActiveTestShow::BS_LOCK); // Next
		CDisplay::GetInstance()->ActiveTest.AddButton(CBinary("\x00\xFF\x00\x00\x00\x06",6),CActiveTestShow::BS_MASK); // Clear
		for(i=0;i<iLevelNum;i++)
		{
			vecModelInfo=vecModelGrpInfo[i];
			iItemNum=vecModelInfo.size();
			if(iItemNum==0)
			{
				strValue="";
			}
			else if(iItemNum==1)
			{
				binSelItem=vecModelInfo[0];
				strValue=CommonTools->GetTxtString(binSelItem);
			}
			else
			{
				strValue=adsGetTextString(CBinary("\x00\xFF\x00\x00\x00\x05",6));
			}
			CDisplay::GetInstance()->ActiveTest.Add(vecItemName[i],strValue,"");
		}
		iSelectNum=CDisplay::GetInstance()->ActiveTest.Show(iSelLevel);
		if((iSelLevel>=0)&&(iSelLevel<iLevelNum))
		{
			vecItemInfo=vecModelGrpInfo[iSelLevel];
			iItemNum=vecItemInfo.size();
			if(iItemNum>1)
			{
				SelectLevelInfo(vecItemName[iSelLevel],iSelLevel,vecModelGrpInfo,vecItemIndex);
			}
		}
		switch(iSelectNum) // 点击了按钮
		{
		case 0: // Exit
			return 1;
		case 1: // Next
#ifdef _DEBUG
			char strVehicleID[20];
			sprintf(strVehicleID,"%02X%02X%02X%02X",binVehicleID[0],binVehicleID[1],binVehicleID[2],binVehicleID[3]);
			CDisplay::GetInstance()->MessageBox(strVehicleID,"Vehicle ID");
#endif
			ShowSystemList();
			break;
		case 2:
			if(!GetModelGrpInfo(vecOrgModelGrpInfo,vecModelGrpInfo,vecItemIndex))
				return 0;
			binVehicleID=NULL;
			break;
		default:
			break;
		}
	}

	return 1;
}

short CVInfo::GetModelGrpInfo(vector<CBinaryGroup> vecOrgModelGrpInfo,vector<CBinaryGroup> &vecModelGrpInfo,CBinaryGroup &vecItemIndex)
{
	unsigned int iIndex=0;
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;
	CBinaryGroup vecModelInfo;
	CDatabase dbData;
	short iLevelNum=0;
	short i=0;
	CBinary binItem=NULL;
	bool Flag_Match=false;
	
	vecModelGrpInfo.clear();
	vecItemIndex.clear();
	iLevelNum=vecOrgModelGrpInfo.size();
	for(i=0;i<iLevelNum;i++)
	{
		vecModelInfo.clear();
		vecModelGrpInfo.push_back(vecModelInfo);
	}
	if(!CommonTools->GetDataBase(dbData,"VINFO.DB"))
	{
		dbData.Close();
		return 0;
	}
	binIndex.Add(bBrand_ID);
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	iIndex=1;
	while(1)
	{
		if(!CommonTools->UpdateBinIndex32(binIndex,iIndex++))
			break;
		if(!CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binIndex))
			break;
		Flag_Match=true;
		for(i=0;i<iLevelNum;i++)
		{
			binItem=vecItemInfo[4+i];
			if(!ExistInVecGrp(binItem,vecOrgModelGrpInfo,i,0))
			{
				Flag_Match=false;
				break;
			}
		}
		if(Flag_Match==true)
		{
			vecItemIndex.push_back(binIndex);
			for(i=0;i<iLevelNum;i++)
			{
				binItem=vecItemInfo[4+i];
				if(!CommonTools->IsCBinaryAvailable(binItem))
					break;
				vecModelInfo=vecModelGrpInfo[i];
				if(CommonTools->GetIndexPosInVector(vecModelInfo,binItem)<0)
				{
					vecModelInfo.push_back(binItem);
					vecModelGrpInfo[i]=vecModelInfo;
				}
			}
		}
	}
	dbData.Close();

	return 1;
}

short CVInfo::GetVehicleID(CBinaryGroup vecItemIndex)
{
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;

	if(vecItemIndex.size()==1)
	{
		binIndex=vecItemIndex[0];
		if(!CommonTools->ReadDBFile(vecItemInfo,"VINFO.DB",binIndex))
			return 0;
		binVehicleID=vecItemInfo[0];
	}

	return 1;
}

short CVInfo::ExistInVecGrp(CBinary binItem,vector<CBinaryGroup> vecGrpInfo,short iIndex,short iSelLevel)
{
	CBinaryGroup vecInfo;

	if(iIndex>iSelLevel)
	if(!CommonTools->IsCBinaryAvailable(binItem)) // 无效时，认为成功
		return 1;
	if(vecGrpInfo.size()<=iIndex)
		return 0;
	vecInfo=vecGrpInfo[iIndex];
	if(!vecInfo.size()) // 未选择时，认为成功
		return 1;
	if(CommonTools->GetIndexPosInVector(vecInfo,binItem)<0)
		return 0;

	return 1;
}

short CVInfo::SelectLevelInfo(string strTitle,short iLevel,vector<CBinaryGroup> &vecModelGrpInfo,CBinaryGroup &vecItemIndex)
{
	CBinary binIndex=NULL;
	vector<string> vecItemName;
	string strItemName="";
	CBinaryGroup vecModelInfo;
	CBinaryGroup vecTempIndex;
	CBinaryGroup vecItemInfo;
	vector<CBinaryGroup> vecNewModelGrpInfo;
	CBinary binItemInfo=NULL;
	short i=0,j=0;
	short iReturn=0;
	CDatabase dbData;
	bool Flag_Match=false;
	short iLevelNum=0;
	CBinary binItem=NULL;
	CMenuShow::CMenuStruct mstMenuStruct;

	vecItemName.clear();
	vecModelInfo=vecModelGrpInfo[iLevel];
	iLevelNum=vecModelGrpInfo.size();
	CDisplay::GetInstance()->Menu.Init(strTitle);
	for(i=0;i<vecModelInfo.size();i++)
	{
		strItemName=CommonTools->GetTxtString(vecModelInfo[i]);
		CDisplay::GetInstance()->Menu.Add(strItemName);
	}
	iReturn=CDisplay::GetInstance()->Menu.Show(mstMenuStruct);
	if(iReturn<0)
	{
		return 1;
	}
	iReturn=mstMenuStruct.m_i16MenuSelected;
	binItemInfo=vecModelInfo[iReturn];
	vecModelInfo.clear();
	vecModelInfo.push_back(binItemInfo);
	vecModelGrpInfo[iLevel]=vecModelInfo;

	vecNewModelGrpInfo.clear();
	for(i=0;i<iLevelNum;i++)
	{
		vecModelInfo.clear();
		vecNewModelGrpInfo.push_back(vecModelInfo);
	}
	vecTempIndex.clear();
	if(!CommonTools->GetDataBase(dbData,"VINFO.DB"))
	{
		dbData.Close();
		return 0;
	}
	for(j=0;j<vecItemIndex.size();j++)
	{
		binIndex=vecItemIndex[j];
		if(!CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binIndex))
			break;
		Flag_Match=true;
		for(i=0;i<iLevelNum;i++)
		{
			binItem=vecItemInfo[4+i];
			if(!ExistInVecGrp(binItem,vecModelGrpInfo,i,iLevel))
			{
				Flag_Match=false;
				break;
			}
		}
		if(Flag_Match==true)
		{
			vecTempIndex.push_back(binIndex);
			for(i=0;i<iLevelNum;i++)
			{
				binItem=vecItemInfo[4+i];
				if(!CommonTools->IsCBinaryAvailable(binItem))
					break;
				vecModelInfo=vecNewModelGrpInfo[i];
				if(CommonTools->GetIndexPosInVector(vecModelInfo,binItem)<0)
				{
					vecModelInfo.push_back(binItem);
					vecNewModelGrpInfo[i]=vecModelInfo;
				}
			}
		}
	}
	dbData.Close();

	vecModelGrpInfo.clear();
	for(i=0;i<iLevelNum;i++)
	{
		vecModelInfo=vecNewModelGrpInfo[i];
		vecModelGrpInfo.push_back(vecModelInfo);
	}
	vecItemIndex=vecTempIndex;

	GetVehicleID(vecItemIndex);

	return 1;
}

short CVInfo::ShowSystemList()
{
	short iSelect=0;
	CBinaryGroup vecSysNameID;
	vector<CBinaryGroup> vecGrpSystemID;

	if(!GetAllSystemInfo(vecSysNameID,vecGrpSystemID))
		return 0;
	while(1)
	{
		iSelect=CommonTools->MenuAddItem(CBinary("\x00\xFF\x00\x00\x00\x07",6),vecSysNameID);
		if(iSelect<0)
			break;
		SystemEnter(vecSysNameID[iSelect],vecGrpSystemID[iSelect]);
	}

	return 1;
}

short CVInfo::GetAllSystemInfo(CBinaryGroup &vecSysNameID,vector<CBinaryGroup> &vecGrpSystemID)
{
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;
	CBinaryGroup vecSystemID;
	CDatabase dbData;
	CBinary binGrpID=NULL;
	unsigned char bPreGrpID=0x00;
	unsigned char bCurGrpID=0x00;
	CBinary binTxtID=NULL;

	vecSysNameID.clear();
	vecGrpSystemID.clear();
	if(!CommonTools->GetDataBase(dbData,"ECULIST.DB"))
	{
		dbData.Close();
		return 0;
	}
	binIndex.Add(bBrand_ID);
	binIndex+=binVehicleID;
	binIndex.Add(0x00);
	while(1)
	{
		binIndex.SetAt(5,binIndex[5]+1);
		if(!CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binIndex))
			break;
		binGrpID=vecItemInfo[0];
		binTxtID=vecItemInfo[4];
		if(CommonTools->IsCBinaryAvailable(binGrpID))
		{
			bCurGrpID=binGrpID[0];
		}
		else
		{
			bCurGrpID=0x00;
		}
		if((bCurGrpID==0x00)||(bPreGrpID!=bCurGrpID))
		{
			bPreGrpID=bCurGrpID;
			CommonTools->GetTxtIndex(binTxtID);
			vecSysNameID.push_back(binTxtID);
			vecSystemID.clear();
			vecSystemID.push_back(binIndex);
			vecGrpSystemID.push_back(vecSystemID);
		}
		else
		{
			vecSystemID=vecGrpSystemID[vecGrpSystemID.size()-1];
			vecSystemID.push_back(binIndex);
			vecGrpSystemID[vecGrpSystemID.size()-1]=vecSystemID;
		}
	}
	dbData.Close();
	return vecSysNameID.size();
}

short CVInfo::SystemEnter(CBinary binTitle,CBinaryGroup vecSystemID)
{
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;
	short i=0;
	bool Flag_Enter=false;
	CBasicDiagnosticUnit BasicDiagUnit;

	CDisplay::GetInstance()->MessageBox(STRID_CONNECTING_ECU,STRID_INFORMATION,adsMB_NoButton);//Connecting ECU...
	Flag_Enter=false;
	for(i=0;i<vecSystemID.size();i++)
	{
		binIndex=vecSystemID[i];
		if(!CommonTools->ReadDBFile(vecItemInfo,"ECULIST.DB",binIndex))
			continue;
		binCurECU_ID=vecItemInfo[1];
		bCurBUS_ID=vecItemInfo[2][0];
		if(BasicDiagUnit.Enter(CDiagnosisUnit::DUF_NORMAL_DIAGNOSIS))
		{
			Flag_Enter=true;
			break;
		}
	}
	if(Flag_Enter==false)
	{
		CDisplay::GetInstance()->MessageBox(STRID_CONNECT_ECU_FAILED,STRID_ERROR);
	}

	return 1;
}
