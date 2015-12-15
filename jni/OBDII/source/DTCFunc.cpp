// DTCFunc.cpp: implementation of the CDTCFunc class.
//
//////////////////////////////////////////////////////////////////////

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

#include "DTCFunc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDTCFunc::CDTCFunc()
{

}

CDTCFunc::~CDTCFunc()
{

}

void CDTCFunc::InitDTCFunc()
{
	Flag_DTC=false;
	vecDTCInfo.clear();
	return;
}

short CDTCFunc::CheckDTCFunc()
{
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;

	Flag_DTC=false;

	binIndex.Add(bBrand_ID);
	binIndex+=binCommECU_ID;
	binIndex.Add(0x01);
	binIndex.Add(0x02);
	binIndex.Add(0x00);

	if(CommonTools->ReadDBFile(vecItemInfo,"DTC.DB",binIndex))
	{
		Flag_DTC=true;
	}

	return 1;
}

short CDTCFunc::GetDTCFuncMenu(CBinaryGroup &vecFuncMenuID)
{
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;

	vecFuncMenuID.clear();

	binIndex.Add(bBrand_ID);
	binIndex+=binCommECU_ID;
	binIndex.Add(0x01);
	binIndex.Add(0x02);
	binIndex.Add(0x00);
	
	if(CommonTools->ReadDBFile(vecItemInfo,"DTC.DB",binIndex))
	{
		vecFuncMenuID.push_back(CBinary("\x00\xFF\x01\x00\x00\x03",6));
	}

	binIndex.SetAt(6,0x03);
	if(CommonTools->ReadDBFile(vecItemInfo,"DTC.DB",binIndex))
	{
		vecFuncMenuID.push_back(CBinary("\x00\xFF\x01\x00\x00\x04",6));
	}

	return vecFuncMenuID.size();
}

short CDTCFunc::DTCFuncShow(CBinary binTitle)
{
	CBinaryGroup vecFuncMenu;
	short iSelect=0;

	if(!GetDTCFuncMenu(vecFuncMenu))
		return 0;
	while(1)
	{
		iSelect=CommonTools->MenuAddItem(binTitle,vecFuncMenu);
		if(iSelect<0)
			break;
		switch(vecFuncMenu[iSelect][5])
		{
		case 0x03:
			ReadDTCFunc(vecFuncMenu[iSelect]);
			break;
		case 0x04:
			ClearDTCFunc(vecFuncMenu[iSelect]);
			break;
		default:
			break;
		}
	}
	return 1;
}

short CDTCFunc::ReadDTCFunc(CBinary binTitle)
{
	vector<CBinary> vecDTCLibindex;
	CBinary CBTitleIndex=NULL;
	short iDTCNum=0;
	short i=0;
	vector<string> vecStrCode;
	vector<string> vecStrDes;
	vector<string> vecStrStatus;

	if(Demo_Flag==true)
	{
		CDisplay::GetInstance()->MessageBox(STRID_NOFAULTCODEPRESENT,STRID_INFORMATION);
		return 1;
	}

	CDisplay::GetInstance()->MessageBox(STRID_READINGFAULTCODE,STRID_INFORMATION,adsMB_NoButton);
	Flag_Success=false;
	iDTCNum=ReadDTCCode();
	if(Flag_Success==false)
	{
		CDisplay::GetInstance()->MessageBox(STRID_READFAULTCODE_FAILED,STRID_INFORMATION);
		return 0;
	}
	iDTCNum=GetVecDTCStr(vecStrCode,vecStrDes,vecStrStatus);
	if(!iDTCNum)
	{
		CDisplay::GetInstance()->MessageBox(STRID_NOFAULTCODEPRESENT,STRID_INFORMATION);
		return 0;
	}
	CDisplay::GetInstance()->TroubleCode.SetTroubleCodeCallBackFunction(NULL);
	CDisplay::GetInstance()->TroubleCode.Init(binTitle);
	for(i=0;i<iDTCNum;i++)
	{
		CDisplay::GetInstance()->TroubleCode.Add(vecStrCode[i],vecStrDes[i],vecStrStatus[i]);
	}
	CDisplay::GetInstance()->TroubleCode.Show();

	return 1;
}

short CDTCFunc::ReadDTCCode()
{
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;
	unsigned char bGrp=0x00;
	short i=0;
	short iNum=0;
	short iRet=0;

	vecDTCInfo.clear();
	CommLayer->InitHisData();
	
	binIndex.Add(bBrand_ID);
	binIndex+=binCommECU_ID;
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	binIndex.Add(0x00);

	bGrp=0x01;
	while(1)
	{
		binIndex.SetAt(5,bGrp);
		binIndex.SetAt(6,0x01);
		if(CommonTools->ReadDBFile(vecItemInfo,"DTC.DB",binIndex))
		{
			iNum=GetDTCNum(vecItemInfo);
			if(iNum==0)
			{
				Flag_Success|=true;
			}
			if(iNum)
			{
				binIndex.SetAt(5,bGrp);
				binIndex.SetAt(6,0x02);
				if(!CommonTools->ReadDBFile(vecItemInfo,"DTC.DB",binIndex))
					break;
				GetDTCCodeHandle(vecItemInfo,iNum);
			}
		}
		else
		{
			binIndex.SetAt(5,bGrp);
			binIndex.SetAt(6,0x02);
			if(!CommonTools->ReadDBFile(vecItemInfo,"DTC.DB",binIndex))
				break;
			GetDTCCodeHandle(vecItemInfo,iNum);
		}
		bGrp++;
	}

	CommLayer->InitHisData();

	return vecDTCInfo.size();
}

short CDTCFunc::GetDTCNum(CBinaryGroup vecItemInfo)
{
	short iNum=0;
	short iRet=0;
	CBinary binReceData=NULL;
	CBinary binPos=NULL;
	vector<unsigned short> veciPos;
	unsigned short iPos=0;
	
	binPos=vecItemInfo[2];
	if(!CommonTools->StrBinToVecInt(binPos,veciPos))
		return -1;

	iRet=CommLayer->ReadAndUpdateHisData(vecItemInfo[0],vecReceData);
	if(iRet==Rece_Positive)
	{
		iPos=veciPos[0];
		binReceData=vecReceData[0];
		if(binReceData.GetSize()>iPos)
		{
			iNum=binReceData[iPos];
		}
		else
		{
			iNum=-1;
		}
	}
	else
	{
		iNum=-1;
	}

	return iNum;
}

short CDTCFunc::GetDTCCodeHandle(CBinaryGroup vecItemInfo,short iNum)
{
	unsigned char bMethod=0x00;
	CBinary binPos=NULL;
	CBinary binInterval=NULL;
	unsigned short iInterval=0;
	unsigned short iStatusPos=0;
	short iRet=0;
	CBinary binReceData=NULL;
	short i=0,j=0,k=0;
	vector<unsigned short> veciPos;
	short iCodeLen=0;
	CBinary binDTCCode=NULL;
	CBinary binStatusPos=NULL;
	CBinary binStatusMask=NULL;

	iRet=CommLayer->ReadAndUpdateHisData(vecItemInfo[0],vecReceData);
	if(iRet!=Rece_Positive)
		return 0;
	//	return -1;

	Flag_Success|=true;

	bMethod=vecItemInfo[1][0];
	binPos=vecItemInfo[3];
	binInterval=vecItemInfo[4];
	binStatusPos=vecItemInfo[5];
	binStatusMask=vecItemInfo[6];
	iCodeLen=CommonTools->StrBinToVecInt(binPos,veciPos);
	if(!iCodeLen)
		return 0;
	switch(bMethod)
	{
	case 0x01:
		if(!CommonTools->IsCBinaryAvailable(binInterval))
			break;
		iInterval=CommonTools->StrBinToUINT16(binInterval);
		for(i=0;i<vecReceData.size();i++)
		{
			binReceData=vecReceData[i];
			iNum=(binReceData.GetSize()-veciPos[0])/iInterval;
			for(j=0;j<iNum;j++)
			{
				binDTCCode=NULL;
				for(k=0;k<iCodeLen;k++)
				{
					binDTCCode.Add(binReceData[veciPos[0]+k+j*iInterval]);
				}
				if(CommonTools->IsCBinaryAvailable(binStatusPos))
				{
					iStatusPos=CommonTools->StrBinToUINT16(binStatusPos);
					if((binReceData[iStatusPos+j*iInterval]&binStatusMask[0])==binStatusMask[0])
					{
						InputVecDTCInfo(binDTCCode,vecItemInfo[7][0]);
					}
				}
				else
				{
					InputVecDTCInfo(binDTCCode,vecItemInfo[7][0]);
				}
			}
		}
		break;
	default:
		break;
	}

	return vecDTCInfo.size();
}

short CDTCFunc::InputVecDTCInfo(CBinary binCode,unsigned char bStatus)
{
	short i=0;
	short iNum=0;
	DTCINFO oneDTCInfo;
	CBinary binData=NULL;
	CBinary binStatus=NULL;
	short iPos=-1;
	bool Flag_All00=true;

	iNum=binCode.GetSize();
	for(i=0;i<iNum;i++)
	{
		if(binCode[i])
		{
			Flag_All00=false;
			break;
		}
	}
	if(Flag_All00==true)
	{
		return 1;
	}

	iNum=vecDTCInfo.size();
	for(i=0;i<iNum;i++)
	{
		oneDTCInfo=vecDTCInfo[i];
		binData=oneDTCInfo.binCode;
		binStatus=oneDTCInfo.binStatus;
		if(CommonTools->CBinaryCompare(binCode,binData))
		{
			iPos=i;
			break;
		}
	}
	if(iPos>=0)
	{
		if(CommonTools->GetBytePosInCBinary(binStatus,bStatus)>=0)
		{
			return 1;
		}
		else
		{
			binStatus.Add(bStatus);
		}
		oneDTCInfo.binStatus=binStatus;
		vecDTCInfo[iPos]=oneDTCInfo;
	}
	else
	{
		oneDTCInfo.binCode=binCode;
		binStatus=NULL;
		binStatus.Add(bStatus);
		oneDTCInfo.binStatus=binStatus;
		vecDTCInfo.push_back(oneDTCInfo);
	}

	return 1;
}

short CDTCFunc::GetVecDTCStr(vector<string> &vecStrCode,vector<string> &vecStrDes,vector<string> &vecStrStatus)
{
	CBinaryGroup vecItemInfo;
	DTCINFO oneDTCInfo;
	CBinary binCode=NULL;
	CBinary binStatus=NULL;
	short i=0,j=0;
	short iNum=0;
	char str[50];
	string strCode="";
	string strDes="";
	string strStatus="";
	bool Flag_DisBrands=false;
	CBinary binECUID=NULL;
	short iRet=0;

	vecStrCode.clear();
	vecStrDes.clear();
	vecStrStatus.clear();

	iNum=vecDTCInfo.size();
	for(i=0;i<iNum;i++)
	{
		oneDTCInfo=vecDTCInfo[i];
		binCode=oneDTCInfo.binCode;
		binStatus=oneDTCInfo.binStatus;
		if(GetDTCCodeInfo(binCode,vecItemInfo))
		{
			if(vecItemInfo[4][0])
			{
				vecItemInfo[1].ReadBuffer(str);
				strCode=str;
				strDes=CommonTools->GetTxtString(vecItemInfo[2]);
				strStatus="";
				for(j=0;j<binStatus.GetSize();j++)
				{
					if(strStatus.size()>0)
					{
						strStatus+=" & ";
					}
					strStatus+=GetStrDTCStatus(binStatus[j]);
				}
				vecStrCode.push_back(strCode);
				vecStrDes.push_back(strDes);
				vecStrStatus.push_back(strStatus);
			}
		}
		else
		{
			if(Flag_DisBrands==false)
			{
				Flag_DisBrands=true;
				iRet=ShowOBDIIBrands(binECUID);
			}
			if(binECUID.GetSize())
			{
				iRet=GetDTCCodeInfoByECUID(binECUID,binCode,vecItemInfo);
				if(iRet)
				{
					if(vecItemInfo[4][0])
					{
						vecItemInfo[1].ReadBuffer(str);
						strCode=str;
						strDes=CommonTools->GetTxtString(vecItemInfo[2]);
						strStatus="";
						for(j=0;j<binStatus.GetSize();j++)
						{
							if(strStatus.size()>0)
							{
								strStatus+=" & ";
							}
							strStatus+=GetStrDTCStatus(binStatus[j]);
						}
						vecStrCode.push_back(strCode);
						vecStrDes.push_back(strDes);
						vecStrStatus.push_back(strStatus);
					}
				}
			}
			if(!iRet)
			{
				strCode=GetStrDefaultCode(binCode);
				strDes=adsGetTextString(CBinary("\x00\xFF\x05\x00\x00\x18",6));
				strStatus="";
				for(j=0;j<binStatus.GetSize();j++)
				{
					if(strStatus.size()>0)
					{
						strStatus+=" & ";
					}
					strStatus+=GetStrDTCStatus(binStatus[j]);
				}
				vecStrCode.push_back(strCode);
				vecStrDes.push_back(strDes);
				vecStrStatus.push_back(strStatus);
			}
		}
	}

	return vecStrCode.size();
}

short CDTCFunc::GetDTCCodeInfo(CBinary binCode,CBinaryGroup &vecItemInfo)
{
	if(GetDTCCodeInfoByECUID(binCommECU_ID,binCode,vecItemInfo))
		return 1;
	if(GetDTCCodeInfoByECUID(CBinary("\x00\x00\x00\x00",4),binCode,vecItemInfo))
		return 1;
	
	return 0;
}

short CDTCFunc::GetDTCCodeInfoByECUID(CBinary binECUID,CBinary binCode,CBinaryGroup &vecItemInfo)
{
	CBinary binIndex=NULL;
	CBinary binData=NULL;
	unsigned short iIndex=0;
	CDatabase dbData;
	
	if(!CommonTools->GetDataBase(dbData,"DTCDES.DB"))
	{
		dbData.Close();
		return 0;
	}
	
	binIndex.Add(bBrand_ID);
	binIndex+=binECUID;
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	binIndex.Add(0x00);
	
	iIndex=1;
	while(1)
	{
		CommonTools->UpdateBinIndex16(binIndex,iIndex++);
		if(!CommonTools->GetVecInfoFromDB(dbData,vecItemInfo,binIndex))
			break;
		binData=vecItemInfo[0];
		if(CommonTools->CBinaryCompare(binCode,binData))
		{
			dbData.Close();
			return 1;
		}
	}
	
	dbData.Close();
	
	return 0;
}

short CDTCFunc::ShowOBDIIBrands(CBinary &binECUID)
{
	CBinary binIndex;
	CBinaryGroup vecItemInfo;
	CBinaryGroup vecECUIDs;
	CBinaryGroup vecTXTIDs;
	CBinary binTXTID=NULL;
	short iSelect=0;
	short iRet=0;

	binECUID=NULL;
	vecECUIDs.clear();
	vecTXTIDs.clear();
	vecTXTIDs.push_back(CBinary("\x00\xFF\x05\x00\x00\x17",6)); // Other
	binIndex.Add(bBrand_ID);
	CBinary temp = CBinary("\xF0\x00\x00\x01",4);
	binIndex += temp;
	binIndex.Add(0x03);
	while(1)
	{
		if(!CommonTools->ReadDBFile(vecItemInfo,"ECULIST.DB",binIndex))
			break;
		vecECUIDs.push_back(vecItemInfo[1]);
		binTXTID=vecItemInfo[4];
		CommonTools->GetTxtIndex(binTXTID);
		vecTXTIDs.push_back(binTXTID);
		binIndex.SetAt(5,binIndex[5]+1);
	}
	if(!vecECUIDs.size())
		return 0;

	CDisplay::GetInstance()->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x14",6),CBinary("\x00\xFF\x01\x00\x00\x03",6));

	while(1)
	{
		iSelect=CommonTools->MenuAddItem(CBinary("\x00\xFF\x04\x00\x00\x01",6),vecTXTIDs);
		if(iSelect<0)
			break;
		if(iSelect>0)
		{
			iRet=1;
			binECUID=vecECUIDs[iSelect-1];
		}
		break;
	}

	return iRet;
}

string CDTCFunc::GetStrDTCStatus(unsigned char bType)
{
	string strStatus="";

	switch(bType)
	{
	case 0x01: strStatus=adsGetTextString(CBinary("\x00\xFF\x02\x00\x00\x01",6)); break;
	case 0x02: strStatus=adsGetTextString(CBinary("\x00\xFF\x02\x00\x00\x02",6)); break;
	case 0x03: strStatus=adsGetTextString(CBinary("\x00\xFF\x02\x00\x00\x03",6)); break;
	case 0x04: strStatus=adsGetTextString(CBinary("\x00\xFF\x02\x00\x00\x04",6)); break;
	default:
		break;
	}

	return strStatus;
}

string CDTCFunc::GetStrDefaultCode(CBinary binCode)
{
	string strCode="";
	char strBuf[100];
	short i=0;
	short iLen=0;

	iLen=binCode.GetSize();

	memset(strBuf,0,sizeof(strBuf));
	if(iLen==2)
	{
		switch(binCode[0]&0xC0)
		{
		case 0x00: sprintf(strBuf,"P%02X%02X",binCode[0]&0x3F,binCode[1]); break;
		case 0x40: sprintf(strBuf,"C%02X%02X",binCode[0]&0x3F,binCode[1]); break;
		case 0x80: sprintf(strBuf,"B%02X%02X",binCode[0]&0x3F,binCode[1]); break;
		case 0xC0: sprintf(strBuf,"U%02X%02X",binCode[0]&0x3F,binCode[1]); break;
		default:
			break;
		}
	}
	else
	{
		for(i=0;i<iLen;i++)
		{
			sprintf(strBuf+i*2,"%02X",binCode[i]);
		}
	}

	strCode=strBuf;

	return strCode;
}

short CDTCFunc::ClearDTCFunc(CBinary binTitle)
{
	short ButtonID=0;
	short iDTCNum=0;
	
	ButtonID=CDisplay::GetInstance()->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x13",6),STRID_INFORMATION,adsMB_YesNo);//Do you want to erase all the fault code(s)?
	if(ButtonID==adsIDNO)
	{
		return 1;
	}
	CDisplay::GetInstance()->MessageBox(STRID_CLEARINGFAULTCODE,STRID_INFORMATION,adsMB_NoButton);//Erasing fault code(s)...
	
	if(Demo_Flag==true)
	{
		CDisplay::GetInstance()->MessageBox(STRID_CLEARFAULTCODE_OK,STRID_INFORMATION);
		return 1;
	}

	if(ClearDTCCode())
	{
		CDisplay::GetInstance()->MessageBox(STRID_CLEARFAULTCODE_OK,STRID_INFORMATION);
	}
	else
	{
		CDisplay::GetInstance()->MessageBox(STRID_CLEARFAULTCODE_FAILED,STRID_ERROR);
	}

	return 1;
}

short CDTCFunc::ClearDTCCode()
{
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;
	short iRet=0;
	
	Flag_Success=false;
	
	binIndex.Add(bBrand_ID);
	binIndex+=binCommECU_ID;
	binIndex.Add(0x00);
	binIndex.Add(0x03);
	binIndex.Add(0x00);

	CommLayer->InitHisData();

	while(1)
	{
		binIndex.SetAt(5,binIndex[5]+1);
		if(!CommonTools->ReadDBFile(vecItemInfo,"DTC.DB",binIndex))
			break;
		iRet=CommLayer->ReadAndUpdateHisData(vecItemInfo[0],vecReceData);
		if(iRet==Rece_Positive)
		{
			Flag_Success|=true;
		}
	}

	CommLayer->InitHisData();

	if(Flag_Success==false)
		return 0;

	return 1;
}
