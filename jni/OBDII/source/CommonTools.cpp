// CommonTools.cpp: implementation of the CCommonTools class.
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
#include "AutoLog.h"
#include "DrivenStd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommonTools::CCommonTools()
{

}

CCommonTools::~CCommonTools()
{

}

/*************************************************************************
*功 能  :       获得指定文件下指定Index的信息
*参 数  :       返回的信息，文件名，索引Index
*返回值 :       -1 异常， 0 没有找到， 1 找到
*注意   :
**************************************************************************/
short CCommonTools::ReadDBFile(vector<CBinary> &ucInfo, string FileName, CBinary binId)
{
	CDatabase vipIndData;

	__android_log_print(ANDROID_LOG_DEBUG, "CCommonTools", "Open database file %s", FileName.c_str());
	if(!vipIndData.Open(FileName)) //打开文件
	{
		__android_log_print(ANDROID_LOG_DEBUG, "CCommonTools", "Unable to open file %s", FileName.c_str());
		//CDisplay::GetInstance()->MessageBox(CANNOT_OPEN_FILE,INFORMATION);
		return 0;
	}

	ucInfo.clear();

	ucInfo=vipIndData.SearchId(binId); //查找ID

	if(!ucInfo.size())//没有找到
	{
		vipIndData.Close();
		return 0;
	}

	vipIndData.Close();

	return 1;
}


short CCommonTools::GetDataBase(CDatabase &dbData,string FileName)
{
	if(!dbData.Open(FileName)) //打开文件
	{
		//CDisplay::GetInstance()->MessageBox(CANNOT_OPEN_FILE,INFORMATION);
		return 0;
	}
	
	return 1;
}

short CCommonTools::GetVecInfoFromDB(CDatabase &dbData,CBinaryGroup &ucInfo,CBinary binId)
{
	ucInfo.clear();
	
	ucInfo=dbData.SearchId(binId); //查找ID
	
	if(!ucInfo.size())//没有找到
	{
		return 0;
	}
	
	return 1;
}

string CCommonTools::GetTxtString(CBinary binTxtID)
{
	CBinary binIndex=NULL;
	string strTxt="";

	binIndex.Add(bBrand_ID);
	binIndex.Add(0x00);
	binIndex+=binTxtID;
	
	strTxt=adsGetTextString(binIndex);

	return strTxt;
}

short CCommonTools::GetTxtIndex(CBinary &binTxtID)
{
	CBinary binIndex=NULL;
	
	binIndex.Add(bBrand_ID);
	binIndex.Add(0x00);
	binIndex+=binTxtID;

	binTxtID=binIndex;
	return 1;
}

short CCommonTools::UpdateBinIndex16(CBinary &binIndex,unsigned short iIndex)
{
	short iLen=0;

	iLen=binIndex.GetSize();
	if(iLen<2)
		return 0;
	binIndex.SetAt(iLen-2,(iIndex>>8)&0xFF);
	binIndex.SetAt(iLen-1,iIndex&0xFF);

	return 1;
}

short CCommonTools::UpdateBinIndex32(CBinary &binIndex,unsigned int iIndex)
{
	short iLen=0;
	
	iLen=binIndex.GetSize();
	if(iLen<4)
		return 0;
	binIndex.SetAt(iLen-4,(iIndex>>24)&0xFF);
	binIndex.SetAt(iLen-3,(iIndex>>16)&0xFF);
	binIndex.SetAt(iLen-2,(iIndex>>8)&0xFF);
	binIndex.SetAt(iLen-1,iIndex&0xFF);

	return 1;
}

/*************************************************************************
*功 能  :       显示功能菜单
*参 数  :       菜单标题，菜单项目集合
*返回值 :       退出信息，或者菜单选择的序号
*说明   :       
**************************************************************************/
short CCommonTools::MenuAddItem(CBinary TitleTxt,vector<CBinary> AllMenuTxt)
{
	CMenuShow::CMenuStruct mstMenuStruct;
	short iRet=0;
	short MenuNum;
	CBinary MenuID;
	
	MenuNum=AllMenuTxt.size();
	
	CDisplay::GetInstance()->Menu.Init(TitleTxt);
	for(short i=0;i<MenuNum;i++)
	{
		MenuID=AllMenuTxt[i];
		CDisplay::GetInstance()->Menu.Add(MenuID);
	}
	
	iRet=CDisplay::GetInstance()->Menu.Show(mstMenuStruct);
	
	if(iRet == -1)
	{
		return iRet;
	}
	
	return mstMenuStruct.m_i16MenuSelected;
}

short CCommonTools::MenuAddItemStr(string TitleTxt,vector<string> AllMenuTxt)
{
	CMenuShow::CMenuStruct mstMenuStruct;
	short iRet=0;
	short MenuNum;
	
	MenuNum=AllMenuTxt.size();
	
	CDisplay::GetInstance()->Menu.Init(TitleTxt);
	for(short i=0;i<MenuNum;i++)
	{
		CDisplay::GetInstance()->Menu.Add(AllMenuTxt[i]);
	}
	
	iRet=CDisplay::GetInstance()->Menu.Show(mstMenuStruct);
	
	if(iRet == -1)
	{
		return iRet;
	}
	
	return mstMenuStruct.m_i16MenuSelected;
}

short CCommonTools::CBinaryCompare(CBinary binData1,CBinary binData2)
{
	short iResult=0;
	short iLen1=0,iLen2=0;

	iLen1=binData1.GetSize();
	iLen2=binData2.GetSize();

	if(iLen1==iLen2)
	{
		iResult=1;
		for(short i=0;i<iLen1;i++)
		{
			if(binData1[i]!=binData2[i])
			{
				iResult=0;
			}
		}
	}

	return iResult;
}

short CCommonTools::CheckCtrlPosAvail(CBinaryGroup vecReceCmd,CBinary binCtrlPos)
{
	short i=0;
	short iNum=0;
	vector<unsigned short> veciPos;
	short iResult=1;
	CBinary binData=NULL;
	unsigned short iPos;
	unsigned short iBytePos=0;
	unsigned short iFrmPos=0;

	iNum=StrBinToVecInt(binCtrlPos,veciPos);

	for(i=0;i<iNum;i++)
	{
		iPos=veciPos[i];
		iBytePos=iPos%1000;
		iFrmPos=iPos/1000;
		if(vecReceCmd.size()>iFrmPos)
		{
			binData=vecReceCmd[iFrmPos];
			if(binData.GetSize()<=iBytePos)
			{
				iResult=0;
				break;
			}
		}
		else
		{
			iResult=0;
			break;
		}
	}

	return iResult;
}

short CCommonTools::CheckCtrlPosAvailFF(CBinaryGroup vecReceCmd,CBinary binCtrlPos)
{
	short i=0;
	short iNum=0;
	vector<unsigned short> veciPos;
	short iResult=1;
	CBinary binData=NULL;
	unsigned short iPos;
	unsigned short iBytePos=0;
	unsigned short iFrmPos=0;
	
	iNum=StrBinToVecInt(binCtrlPos,veciPos);
	
	for(i=0;i<iNum;i++)
	{
		iPos=veciPos[i];
		iBytePos=iPos%1000;
		iFrmPos=iPos/1000;
		if(vecReceCmd.size()>iFrmPos)
		{
			binData=vecReceCmd[iFrmPos];
			if(binData.GetSize()<=iBytePos)
			{
				iResult=0;
				break;
			}
			if(binData[iBytePos]==0xFF)
			{
				iResult=0;
				break;
			}
		}
		else
		{
			iResult=0;
			break;
		}
	}
	
	return iResult;
}

short CCommonTools::GetCtrlData(CBinaryGroup vecReceCmd,CBinary binCtrlPos,CBinary binMasks,CBinary &binData)
{
	CBinary binReceData=NULL;
	CBinary binTemp=NULL;
	short iRet=0;
	short i=0,j=0;
	short iNum=0;
	vector<unsigned short> veciPos;
	unsigned short iPos=0;
	short iFrmPos=0;
	short iBytePos=0;
	unsigned char bData=0x00;
	unsigned char bMask=0x00;
	unsigned char bTemp=0x00;
	
	if(!StrBinToVecInt(binCtrlPos,veciPos))
		return 0;

	iNum=veciPos.size();
	binData=NULL;
	for(i=iNum-1;i>=0;i--)
	{
		iPos=veciPos[i];
		iFrmPos=iPos/1000;
		iBytePos=iPos%1000;
		if(iFrmPos<vecReceCmd.size())
		{
			binReceData=vecReceCmd[iFrmPos];
			if(iBytePos<binReceData.GetSize())
			{
				binData.Add(binReceData[iBytePos]);
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	if(IsCBinaryAvailable(binMasks))
	{
		if(iNum>binMasks.GetSize())
			return 0;
		iPos=0;
		bTemp=0x00;
		binTemp=NULL;
		for(i=0;i<iNum;i++)
		{
			bData=binData[i];
			bMask=binMasks[i];
			for(j=0;j<8;j++)
			{
				if(((bMask>>j)&0x01)==0x01)
				{
					if(((bData>>j)&0x01)==0x01)
					{
						bTemp+=(0x01<<(iPos%8));
					}
					iBytePos=iPos/8;
					if(iBytePos<binTemp.GetSize())
					{
						binTemp.SetAt(iBytePos,bTemp);
					}
					else
					{
						binTemp.Add(bTemp);
					}
					iPos++;
					if((iPos%8)==0)
					{
						bTemp=0x00;
					}
				}
			}
		}
		binData=binTemp;
	}
		
	return binData.GetSize();
}

short CCommonTools::IsExistDBFile(string FileName,CBinary binId)
{
	CDatabase vipIndData;
	vector<CBinary> ucInfo;

	if(!vipIndData.Open(FileName)) //打开文件
	{
		//CDisplay::GetInstance()->MessageBox(CANNOT_OPEN_FILE,INFORMATION);
		return 0;
	}

	ucInfo=vipIndData.SearchId(binId); //查找ID

	if(!ucInfo.size())//没有找到
	{
		vipIndData.Close();
		return 0;
	}

	vipIndData.Close();

	return 1;
}

/*************************************************************************
*功 能  :       获得CBinary在vector<CBinary>中的位置序号
*参 数  :       Info库，索引Index
*返回值 :       回复位置序号
*说明   :       
**************************************************************************/
short CCommonTools::GetIndexPosInVector(vector<CBinary> vInfo,CBinary CBIndex)
{
	short Num=0;
	short i=0;
	CBinary CBTemp;

	Num=vInfo.size();

	for(i=0;i<Num;i++)
	{
		CBTemp=vInfo[i];
		if(CBTemp==CBIndex)
			return i;
	}

	return -1;
}

short CCommonTools::GetFuncCmds(vector<CBinary> vecFuncIDs,string FileName)
{
	return 1;
}

short CCommonTools::IsCBinaryAvailable(CBinary CBInfo)
{
	short iRet=0;
 	short iLen=0;
	char* str;

	iLen=CBInfo.GetSize();
	if(!iLen)
		return 0;
	iRet=1;
	str=new char[iLen];
	CBInfo.ReadBuffer(str);
	if(strcmp(str,"NULL")==0)
	{
		iRet=0;
	}
	delete str;

// 	iLen=CBInfo.GetSize();
// 	if(iLen>0)
// 	{
// 		if((iLen==1)&&(CBInfo[0]==0x00))
// 			iRet=0;
// 		else
// 			iRet=1;
// 	}

	return iRet;
}

/*************************************************************************
*功 能  :       判断某个CBinary是否存在于vector<CBinary>中
*参 数  :       vector<CBinary>, CBinary
*返回值 :       不存在：-1，存在：返回存在的位置
*说明   :       
**************************************************************************/
short CCommonTools::IsExistInVecBin(vector<CBinary> vecBin,CBinary binItem)
{
	short iNum=0;
	short i=0;
	CBinary binTemp=NULL;

	iNum=vecBin.size();
	if(!iNum)
		return -1;
	for(i=0;i<iNum;i++)
	{
		binTemp=vecBin[i];
		if(binTemp==binItem)
			return i;
	}

	return -1;
}

short CCommonTools::StrBinToVecInt(CBinary binStr,vector<unsigned short> &vecInt)
{
	short iLen=0;
	char* strInt=NULL;
	char* StrItem=NULL;
	char c;
	short i=0;
	short j=0;
	unsigned short iPos=0;

	if(!IsCBinaryAvailable(binStr))
		return 0;

	vecInt.clear();
	
	iLen=binStr.GetSize();

	if(iLen<2)
		return 0;

	strInt=new char[iLen];
	memset(strInt,0,iLen);
	StrItem=new char[iLen];
	memset(StrItem,0,iLen);
	binStr.ReadBuffer(strInt);

	while(1)
	{
		c=strInt[i];
		if(c=='\0')
		{
			if(strlen(StrItem))
			{
				iPos=atoi(StrItem);
				vecInt.push_back(iPos);
			}
			break;
		}
		else if(c==',')
		{
			if(strlen(StrItem))
			{
				iPos=atoi(StrItem);
				vecInt.push_back(iPos);
				memset(StrItem,0,iLen);
				j=0;
			}
		}
		else
		{
			StrItem[j++]=c;
		}
		i++;
	}

	delete strInt;
	delete StrItem;

	return vecInt.size();
}

short CCommonTools::GetMaxPos(CBinary binPos1,CBinary binPos2,CBinary binPos3)
{
	short iMax=0;
	short i=0;

	for(i=0;i<binPos1.GetSize();i++)
	{
		if(binPos1[i]>iMax)
			iMax=binPos1[i];
	}
	
	for(i=0;i<binPos2.GetSize();i++)
	{
		if(binPos2[i]>iMax)
			iMax=binPos2[i];
	}
	
	for(i=0;i<binPos3.GetSize();i++)
	{
		if(binPos3[i]>iMax)
			iMax=binPos3[i];
	}

	return iMax;
}

bool CCommonTools::IsBufOverFlow(CBinary binCmd,unsigned short iPos)
{
	if(iPos<binCmd.GetSize())
		return false;

	return true;
}

short CCommonTools::GetDataByMasks(CBinary &binData,CBinary binMasks)
{
	short iByteNum=0;
	short i=0;
	unsigned char bTemp=0;

	iByteNum=binData.GetSize();

	if(IsCBinaryAvailable(binMasks))
	{
		if(iByteNum!=binMasks.GetSize())
			return 0;
		for(i=0;i<iByteNum;i++)
		{
			bTemp=binData[i]&binMasks[i];
			binData.SetAt(i,bTemp);
		}
	}

	return 1;
}

unsigned char CCommonTools::StrBinToBYTE(CBinary binStr)
{
	char *Str;
	short iLen=0;
	unsigned char iResult=0;

	iLen=binStr.GetSize();
	if(!iLen)
		return 0;
	Str=new char[iLen];
	memset(Str,0,iLen);
	binStr.ReadBuffer(Str);
	iResult=atoi(Str);

	delete Str;

	return iResult;
}

unsigned short CCommonTools::StrBinToUINT16(CBinary binStr)
{
	char *Str;
	short iLen=0;
	unsigned short iResult=0;

	iLen=binStr.GetSize();
	if(!iLen)
		return 0;
	Str=new char[iLen];
	memset(Str,0,iLen);
	binStr.ReadBuffer(Str);
	iResult=atoi(Str);

	delete Str;

	return iResult;
}

short CCommonTools::StrBinToINT16(CBinary binStr)
{
	char *Str;
	short iLen=0;
	short iResult=0;

	iLen=binStr.GetSize();
	if(!iLen)
		return 0;
	Str=new char[iLen];
	memset(Str,0,iLen);
	binStr.ReadBuffer(Str);
	iResult=atoi(Str);

	delete Str;

	return iResult;
}

unsigned int CCommonTools::StrBinToUINT32(CBinary binStr)
{
	char *Str;
	short iLen=0;
	unsigned int iResult=0;

	iLen=binStr.GetSize();
	if(!iLen)
		return 0;
	Str=new char[iLen];
	memset(Str,0,iLen);
	binStr.ReadBuffer(Str);
	iResult=atoi(Str);

	delete Str;

	return iResult;
}

double CCommonTools::StrBinToFLOAT(CBinary binStr)
{
	char *Str;
	short iLen=0;
	double iResult=0.0;

	iLen=binStr.GetSize();
	if(!iLen)
		return 0;
	Str=new char[iLen];
	memset(Str,0,iLen);
	binStr.ReadBuffer(Str);
	iResult=atof(Str);

	delete Str;

	return iResult;
}

short CCommonTools::MatchVersionID(CBinary binECUID,CBinary binData)
{
	short iLen=0;
	short i=0;
	short iRet=0;
	char *StrECUID;
	char *StrData;
	char c;

	iLen=binECUID.GetSize();
	StrECUID=new char[iLen*2+3];
	sprintf(StrECUID,"0x");
	for(i=0;i<iLen;i++)
	{
		sprintf(StrECUID+2+i*2,"%02x",binECUID[i]);
	}
	
	iLen=binData.GetSize();
	StrData=new char[iLen];
	binData.ReadBuffer(StrData);

	iRet=0;
	if(strlen(StrECUID)==iLen)
	{
		iRet=1;
		for(i=0;i<iLen;i++)
		{
			c=StrData[i];
			switch(c)
			{
			case '*':
				break;
			default:
				if(StrECUID[i]!=c)
					iRet=0;
				break;
			}
			if(iRet==0)
				break;
		}
	}

	delete StrECUID;
	delete StrData;

	return iRet;
}

short CCommonTools::GetSpecStepID(CBinary binStepID)
{
	short iRet=0;
	char* str=NULL;
	short iLen=0;

	iLen=binStepID.GetSize();
	str=new char[iLen];
	binStepID.ReadBuffer(str);
	if(strcmp(str,"break")==0)
	{
		iRet=Spec_Step_Break;
	}
	else if(strcmp(str,"end")==0)
	{
		iRet=Spec_Step_End;
	}
	else
	{
		iRet=StrBinToINT16(binStepID);
	}
	return iRet;
}

short CCommonTools::GetBytePosInCBinary(CBinary binData,unsigned char bData)
{
	short iPos=-1;
	short i=0;

	for(i=0;i<binData.GetSize();i++)
	{
		if(bData==binData[i])
		{
			iPos=i;
			break;
		}
	}

	return iPos;
}

short CCommonTools::StringToCBinary(string strData,CBinary &binData)
{
	short i=0;
	short iLen=0;

	binData=NULL;
	iLen=strData.size();

	for(i=0;i<iLen;i++)
	{
		binData.Add(strData[i]);
	}
	binData.Add(0x00);

	return 1;
}


