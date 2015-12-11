#include "SecurityBaseClass.h"
#include <memory.h>
#include <stdio.h>
#include "EncrypClass.h"
#include "RijndaelDataBase.h"
#include "des.h"
#include "CrcClass.h"
#include <math.h>
#include <string>
#include <stdlib.h>
using namespace std;

typedef struct 
{
	unsigned char	ucFlag[2+1];		      // 许可文件标志"LF"，含义为License Information Data File 字头
	unsigned char   aucVersion[4+1];	      //表示license 文件的版本
	unsigned char	ucCreateTime[8+1];      // 时间戳
	unsigned char	ucCreateData[8+1];      // 文件生成日期 如：20091104 表示2009年11月4号生成
	unsigned char	ucExpiredData[8+1];     // 文件失效日期 如：20101104 表示2010年11月4号失效
	unsigned char	ucUseCount[8+1];		  //使用次数
	unsigned char   aucSerialNumber[12+1];  //序列号
	unsigned char   aucLanguage[39+1];      //语言信息
	unsigned char   auDiagVersion[8+1];     //版本信息
	unsigned char   auVechicleCode[64+1];   //车系信息
	unsigned char	auReserved[21];       //预留字节
	//end add
}LICENSE_STRUCT_OLD;  //共

CSecurityBaseClass::CSecurityBaseClass()
{
//	memset(m_strLicenseFileName,0,sizeof(m_strLicenseFileName));
}

bool CSecurityBaseClass::Initialize(const char* pFilePath)
{
	int nPath = strlen(pFilePath);
	if(nPath < MAX_FILEPATH)
	{	
		//memcpy(m_strLicenseFileName,pFilePath,strlen(pFilePath));
		string strFilename = pFilePath;
		char chEnd = *(strFilename.end()-1);
		if(chEnd != '\\' && chEnd != '/')
		{
			strFilename += '\\';
		}
		strFilename += "license.dat";
		strcpy(m_strLicenseFileName,strFilename.data());
#if 1		
		FILE *m_pfText = NULL;
		CCrcClass crc;
		char mac_key[]="58073424";//Key
		char  szMac[9]="";

		unsigned long dwCrc32;
		unsigned long dwCrc32_1;
		crc.Init();
		crc.FileCrc(m_strLicenseFileName,4,dwCrc32);

		unsigned char mac[4];
		char *pBuf = new char[sizeof(LICENSE_STRUCT) + 16];
		if(!pBuf) 
		{
			//vCheckCode = (vCheckCode | CHECK_OTHER_ERROR);
			return FALSE;
		}

		memset(pBuf,0,sizeof(LICENSE_STRUCT) + 16);	

		try
		{
			m_pfText = fopen(m_strLicenseFileName, "rb");
			if(m_pfText)
			{
				fseek(m_pfText,-4,SEEK_END);
				int nRead = fread((unsigned char*)mac,1,4,m_pfText);	
			}
			else
			{
				return FALSE;
			}
			dwCrc32_1 = MAKEDW(mac[0],mac[1],mac[2],mac[3]);
			if(dwCrc32_1 != dwCrc32)
			{
				//vCheckCode = (vCheckCode | CHECK_OTHER_ERROR);
				if(pBuf)
				{
					delete []pBuf;
					pBuf = NULL;
				}
				fclose(m_pfText);
				return FALSE;
			}


			// 定位到开头
			fseek(m_pfText, 0, SEEK_SET);
			
			int result = fread(pBuf, sizeof(unsigned char), sizeof(LICENSE_STRUCT) + 16, m_pfText);
			if(sizeof(LICENSE_STRUCT) + 16 != result)
			{
				delete []pBuf;
				pBuf = NULL;
				fclose(m_pfText);
				return FALSE;
			}
		}
		catch(...)
		{
			delete []pBuf;
			pBuf = NULL;
			fclose(m_pfText);
			//vCheckCode = (vCheckCode | CHECK_OTHER_ERROR);
			return FALSE;
		}
		int nnn = sizeof(LICENSE_STRUCT);
		for(int m=0;m<2;m++)
		{
			char szTemp1[9] ="";
			char szTemp2[7] ="";
			memcpy(szTemp1,pBuf+sizeof(LICENSE_STRUCT)+m*8,8);
			stkdes_de(szTemp1,mac_key,szTemp2);
			if(m == 0)
			{
				memcpy(szMac+m*6,szTemp2,6);
			}
			else
			{
				memcpy(szMac+m*6,szTemp2,2);
			}
		}

		szMac[8] = 0;
		CEncrypClass en;
		en.KeyGen((DWORD)atol(szMac));
		en.DecodeBuffer((unsigned char*)pBuf,sizeof(LICENSE_STRUCT));
		
		//LICENSE_STRUCT *mLicenseData;
		//mLicenseData = (LICENSE_STRUCT*)pBuf;

		memcpy((BYTE*)&m_LicenseData,(BYTE*)pBuf,sizeof(LICENSE_STRUCT));
		fclose(m_pfText);
		if(pBuf)
		{
			delete[] pBuf;
			pBuf = NULL;
		}
#endif
		return TRUE;
	}
	else
		return FALSE;
}

void CSecurityBaseClass::TrimRightBlank(char *pP)
{
	int n = strlen(pP);
	for(int i=n-1;i>0;i--)
	{
		if(pP[i] == ' ')
			pP[i] = 0;
		else
			break;
	}
}

//#ifdef _DECRYP	
//校验许可文件
#if 0
int CSecurityBaseClass::CheckLisence(const char *pSerialNumber,const char *pchFileName,const char* pVecleName,const char* pLanguage,double fVersion)
{
#if 0
		return 0;
#else
	string strVersion;
	string strVechile;
	string strLanguage;
	string strSerail;
	FILE *m_pfText = NULL;

	char SerialNumber[17]="";
	char Language[17]="";
	char Version[17]="";
	char SoftWareCode[41]="";
    char mac_key[]="58073424";//Key
	char  szMac[9]="";

	//测试机直接跳过
//	if( pSerialNumber[0] == 'T' && pSerialNumber[1] == 'T' && pSerialNumber[3] == 'T' )
//		return 0;

	char   *pBuf = new char[193];
	if(!pBuf) return 1;
	memset(pBuf,0,193);	


	// 取得工作目录
	string strFilename = pchFileName;
	char chEnd = *(strFilename.end()-1);
	if(chEnd != '\\' && chEnd != '/')
	{
		strFilename += '\\';
	}
	strFilename += "license.dat";
//	const char *pchFileName = NULL;
 	pchFileName = strFilename.data();

	//打开文件
	m_pfText = fopen(pchFileName, "rb");
	if(m_pfText == NULL)
	{
		delete []pBuf;
		pBuf = NULL;
		return 2;
	}

	try
	{
		// 定位到开头
		fseek(m_pfText, 0, SEEK_SET);
		// 读取文件内容前192个字节
		int result = fread(pBuf, sizeof(unsigned char), 192, m_pfText);
		if(192 != result)
		{
			delete []pBuf;
			pBuf = NULL;
			fclose(m_pfText);
			return 2;
		}
	}
	catch(...)
	{
		delete []pBuf;
		pBuf = NULL;
		fclose(m_pfText);
		return 2;
	}
	CEncrypClass en;
	en.KeyGen((DWORD)85165476);
	en.DecodeBuffer((unsigned char*)pBuf,192);

	for(int m=0;m<2;m++)
	{
		char szTemp1[9] ="";
		char szTemp2[7] ="";
		memcpy(szTemp1,pBuf+112+64+m*8,8);
		stkdes_de(szTemp1,mac_key,szTemp2);
		if(m == 0)
		{
			memcpy(szMac+m*6,szTemp2,6);
		}
		else
		{
			memcpy(szMac+m*6,szTemp2,2);
		}
	}
	szMac[8] = 0;



//版本号比较
	char btVersion[13]={0};
	memset(btVersion,' ',9);
	for(int i =0;i<2;i++)
	{
		char pP[9]="";
		char pD[7]="";
		memcpy(pP,pBuf+56+64+i*8,8);
		pP[8] = 0;
		stkdes_de(pP,szMac,pD);
		pD[6] = 0;
		memcpy(btVersion+i*6,pD,6);
	}
	btVersion[12] = 0;
	double fVersion1 = atof((char*)btVersion);
	if(fVersion1 != fVersion)
	{
		delete []pBuf;
		pBuf = NULL;
		fclose(m_pfText);
		return 10;		
	}

	//车系比较
	char btSoftcode[31]={0};
	memset(btSoftcode,' ',31);
	for(i =0;i<5;i++)
	{
		char pP[9]="";
		char pD[7]="";
		memcpy(pP,pBuf+72+64+i*8,8);
		pP[8] = 0;
		stkdes_de(pP,szMac,pD);
		pD[6] = 0;
		memcpy(btSoftcode+i*6,pD,6);
	}
	btSoftcode[30] = 0;
	TrimRightBlank((char*)btSoftcode);

	if(strcmp((char*)btSoftcode,pVecleName) != 0)
	{
		delete []pBuf;
		pBuf = NULL;
		fclose(m_pfText);
		return 11;		
	}

	//语言比较
	bool bFlag = false;
	for(i =0;i<13;i++)
	{
		char pP[9]="";
		char pD[7]="";
		memcpy(pP,pBuf+16+i*8,8);
		pP[8] = 0;
		stkdes_de(pP,szMac,pD);
		pD[6] = 0;
		TrimRightBlank((char*)pD);
		if(strcmp(pLanguage,(char*)pD) == 0)
		{
			bFlag = true;
			break;
		}
	}
	if(!bFlag) 
	{	
		delete []pBuf;
		pBuf = NULL;
		fclose(m_pfText);
		return 5;
	}


//机器序列号比较
	char btSerail[13]={0};
	memset(btSerail,' ',13);
	for(i =0;i<2;i++)
	{
		char pP[9]="";
		char pD[7]="";
		memcpy(pP,pBuf+i*8,8);
		pP[8] = 0;
		stkdes_de(pP,szMac,pD);
		pD[6] = 0;
		memcpy(btSerail+i*6,pD,6);
	}
	btSerail[12] = 0;
	TrimRightBlank((char*)btSerail);
	if(strcmp(pSerialNumber,(char*)btSerail) != 0)
	{
		delete []pBuf;
		pBuf = NULL;
		fclose(m_pfText);
		return 9;		
	}

	if(pBuf)
	{
		delete []pBuf;
		pBuf = NULL;
	}
	fclose(m_pfText);
	return 0;
#endif
}
#else

unsigned short CSecurityBaseClass::CheckLisence(const char *pSerialNumber,/*const char *pchFileName,*/const char* pVecleName,const char* pLanguage,float fVersion,double nUsedCount,const char* szInitPwd,const char* szMCUID,int &nCount)
{
	string strVersion;
	string strVechile;
	string strLanguage;
	string strSerail;
	FILE *m_pfText = NULL;
	BYTE nRet = 0;
//	char SerialNumber[17]="";
//	char Language[17]="";
//	char Version[17]="";
//	char SoftWareCode[41]="";

	char	ucFlag[4+1]="";					//3	    // 可件标志"LF"含义为License Information Data File 头
	char    aucVersion[8+1]="";				//5	    //表示license 文件的版本
	char	ucCreateTime[8+1]="";				//9     // 时间戳
	char	ucCreateData[8+1]="";				//9     // 文件生成日期 如：20091104 表示2009年11月4号生成
	char	ucExpiredData[8+1]="";				//9	    // 文件失效日期 如：20101104 表示2010年11月4号失效
	char    aucSerialNumber[16+1]="";			//13	//序列号
	char    aucLanguage[64+1]="";				//40	//语言信息
	char    auDiagVersion[8+1]="";				//9		//版本信息
	char    auVechicleCode[64+1]="";			//65	//车系信息
	char    auInitPassWord[8+1]="";			//9		//初始密码
	char    auMCUiD[32+1]="";					//32	//MCU_ID
	unsigned long	dwTaskIdCount;					//2		TaskId数量
	unsigned long	dwTaskID[MAX_TASKID_NUMBER];	//400*4	//TASKID 数组	


    char mac_key[]="58073424";//Key
	char  szMac[9]="";
	WORD vCheckCode = CHECK_SUCCESS;

	if( pLanguage[0] == 0)
		vCheckCode = (vCheckCode | CHECK_LANGUAGE_ERROR);
	memcpy(aucLanguage,m_LicenseData.aucLanguage,sizeof(m_LicenseData.aucLanguage));
	if( strstr((char*)aucLanguage,pLanguage) == NULL)
		vCheckCode = (vCheckCode | CHECK_LANGUAGE_ERROR);  //语言交验失败
	
	memcpy(aucSerialNumber,m_LicenseData.aucSerialNumber,sizeof(m_LicenseData.aucSerialNumber));
	if(strcmp(pSerialNumber,(char*)aucSerialNumber) !=0 )
		vCheckCode = (vCheckCode | CHECK_SERIALNUMBER_ERROR);  //序列号校验失败

	memcpy(auVechicleCode,m_LicenseData.auVechicleCode,sizeof(m_LicenseData.auVechicleCode));
	if(strcmp(pVecleName,(char*)auVechicleCode) !=0 )
		vCheckCode = (vCheckCode | CHECK_VECHICLECODE_ERROR);  //车系代码校验失败

	memcpy(auDiagVersion,m_LicenseData.auDiagVersion,sizeof(m_LicenseData.auDiagVersion));
	float bbb = atof((char*)auDiagVersion);
	float aaa = fabs(fVersion - bbb);

	if(aaa > 0.0001)
		vCheckCode = (vCheckCode | CHECK_VERSION_ERROR);   //诊断版本校验失败

	memcpy(ucExpiredData,m_LicenseData.ucExpiredData,sizeof(m_LicenseData.ucExpiredData));
	if(strcmp((char*)ucExpiredData,"00000000") !=0 )
	{
		if(atol((char*)ucCreateData) > atol((char*)ucExpiredData) )
			vCheckCode = (vCheckCode | CHECK_USETIME_ERROR);  //过期
	}
#if 1
	memcpy(auInitPassWord,m_LicenseData.auInitPassWord,sizeof(auInitPassWord)-1);
 	if(strcmp((char*)auInitPassWord,"00000000") !=0 )
	{
		if(szInitPwd == NULL)
			vCheckCode = (vCheckCode | CHECK_INITPWD_ERROR);  //初始密码不符
		else{
			if(strncmp(szInitPwd,(char*)auInitPassWord,sizeof(auInitPassWord)-1) !=0 )
				vCheckCode = (vCheckCode | CHECK_INITPWD_ERROR);  //初始密码不符
		}
	}

	memcpy(auMCUiD,m_LicenseData.auMCUiD,sizeof(auMCUiD)-1);	
	if(memcmp((char*)auMCUiD,"00000000000000000000000000000000",32) !=0 )
	{
		if(szMCUID == NULL)
			vCheckCode = (vCheckCode | CHECK_INITPWD_ERROR);  //初始密码不符
		else{
			if(strcmp(szMCUID,(char*)auMCUiD) !=0 )
				vCheckCode = (vCheckCode | CHECK_MCUID_ERROR);  //MCUID不符
		}

	}
#endif
/*	
	int nLicenseCount = atoi( (char*)m_LicenseData.ucUseCount );
	if( nLicenseCount >= (int)0 )
	{
		if( (long)nUsedCount > atol((char*)m_LicenseData.ucUseCount) )
		{			
			vCheckCode = (vCheckCode | CHECK_USECOUNT_ERROR); //超过使用次数
		}
		else
		{
			nCount = atol((char*)m_LicenseData.ucUseCount) - nUsedCount ;
		}
	}
	if(pBuf)
	{
		delete []pBuf;
		pBuf = NULL;
	}
	fclose(m_pfText);
*/
	return vCheckCode;
}


int CSecurityBaseClass::GetProductStatus()
{
	int vCheckCode;
	char	auStatus[4]="";					//2		//状态表示 0:初始状态 1:正常状态(已激活)  2:冻结
	
	memcpy(auStatus,m_LicenseData.auStatus,sizeof(m_LicenseData.auStatus));	
	if(memcmp(auStatus,"0",1) == 0)
	{
		vCheckCode =  0;
	}
	else if(memcmp(auStatus,"1",1) == 0)
	{
		vCheckCode =  1;
	}
	else if(memcmp(auStatus,"9",1) == 0)
	{
		vCheckCode =  9;
	}
	else
		vCheckCode =  2;

	return vCheckCode;
}


bool CSecurityBaseClass::CheckFunctionIdLegal(unsigned short dwVehicleID,unsigned short dwFunctionId)
{
	int nCount = m_LicenseData.dwTaskIdCount;
	if(nCount>MAX_TASKID_NUMBER)
		return FALSE;
	for(int i=0;i<nCount;i++)
	{
		unsigned long dwId;
		dwId = (unsigned long)dwFunctionId|(unsigned long)dwVehicleID<<16;
		if(m_LicenseData.dwTaskID[i] == dwId)
		   return FALSE;
	}
	return TRUE;
}

bool CSecurityBaseClass::CheckFunctionIdLegal(unsigned long dwFunctionId)
{
	int nCount = m_LicenseData.dwTaskIdCount;
	
	if(nCount>MAX_TASKID_NUMBER)
		return FALSE;
	for(int i=0;i<nCount;i++)
	{
		if(m_LicenseData.dwTaskID[i] == dwFunctionId)
		   return FALSE;
	}
	return TRUE;
}

int CSecurityBaseClass::GetTryUseCount()
{
	char	ucUseCount[8]="";				//9     //使用次数
	memcpy(ucUseCount,m_LicenseData.ucUseCount,sizeof(m_LicenseData.ucUseCount));	
	return atol((char*)ucUseCount);
}

void CSecurityBaseClass::GetMcuId(char *szMcuId)
{
	//unsigned char	ucMcuId[33];				//     MCUID
	//memset(ucMcuId,0,sizeof(ucMcuId));
	if(szMcuId == NULL)
		return;
	memcpy(szMcuId,m_LicenseData.auMCUiD,sizeof(m_LicenseData.auMCUiD));	
	return ;
}
/*
	获取MacAddress 返回格式 如："AB0C87AC1FEE"
*/
void CSecurityBaseClass::GetMacAddress(unsigned char *szMacAddress)
{
	if(szMacAddress == NULL)
		return;
	memcpy(szMacAddress,m_LicenseData.auReserved,sizeof(m_LicenseData.auReserved));	
	return ;
}

int CSecurityBaseClass::CheckLisence(const char *pSerialNumber,const char *pchFileName,const char* pVecleName,const char* pLanguage,float fVersion,double nUsedCount)
{
	string strVersion;
	string strVechile;
	string strLanguage;
	string strSerail;
	FILE *m_pfText = NULL;
	int nRet = 0;
	char SerialNumber[17]="";
	char Language[17]="";
	char Version[17]="";
	char SoftWareCode[41]="";
    char mac_key[]="58073424";//Key
	char  szMac[9]="";

	string strFilename = pchFileName;
	char chEnd = *(strFilename.end()-1);
	if(chEnd != '\\' && chEnd != '/')
	{
		strFilename += '\\';
	}
	strFilename += "license.dat";
 	pchFileName = strFilename.data();

	//首先校验文件是否被修改

	CCrcClass crc;
	unsigned long dwCrc32;
	unsigned long dwCrc32_1;
	crc.Init();
	crc.FileCrc(pchFileName,4,dwCrc32);

	unsigned char mac[4];
//	unsigned char Signature[9];	
	char   *pBuf = new char[sizeof(LICENSE_STRUCT_OLD) + 16];
	if(!pBuf) 
	{
		nRet = 9;
		return nRet;
	}

	memset(pBuf,0,sizeof(LICENSE_STRUCT_OLD) + 16);	

	try
	{
		m_pfText = fopen(pchFileName, "rb");
		if(m_pfText)
		{
			fseek(m_pfText,-4,SEEK_END);
			int nRead = fread((unsigned char*)mac,1,4,m_pfText);	
		}
		else
		{
			return 9;
		}
		dwCrc32_1 = MAKEDW(mac[0],mac[1],mac[2],mac[3]);
		if(dwCrc32_1 != dwCrc32)
		{
			nRet = 1;   //文件校验不通过
			if(pBuf)
			{
				delete []pBuf;
				pBuf = NULL;
			}
			fclose(m_pfText);
			return nRet;
		}


		// 定位到开头
		fseek(m_pfText, 0, SEEK_SET);
		
		int result = fread(pBuf, sizeof(unsigned char), sizeof(LICENSE_STRUCT_OLD) + 16, m_pfText);
		if(sizeof(LICENSE_STRUCT_OLD) + 16 != result)
		{
			delete []pBuf;
			pBuf = NULL;
			fclose(m_pfText);
			return 9;
		}
	}
	catch(...)
	{
		delete []pBuf;
		pBuf = NULL;
		fclose(m_pfText);
		return 9;
	}

	for(int m=0;m<2;m++)
	{
		char szTemp1[9] ="";
		char szTemp2[7] ="";
		memcpy(szTemp1,pBuf+sizeof(LICENSE_STRUCT_OLD)+m*8,8);
		stkdes_de(szTemp1,mac_key,szTemp2);
		if(m == 0)
		{
			memcpy(szMac+m*6,szTemp2,6);
		}
		else
		{
			memcpy(szMac+m*6,szTemp2,2);
		}
	}

	szMac[8] = 0;
	CEncrypClass en;
	en.KeyGen((DWORD)atol(szMac));
	en.DecodeBuffer((unsigned char*)pBuf,sizeof(LICENSE_STRUCT_OLD));

	LICENSE_STRUCT_OLD *mLicenseData;
	mLicenseData = (LICENSE_STRUCT_OLD*)pBuf;

	if( pLanguage[0] == 0)
		nRet = 2;
	if( strstr((char*)m_LicenseData.aucLanguage,pLanguage) == NULL)
		nRet = 2;  //语言交验失败

	if(strcmp(pSerialNumber,(char*)m_LicenseData.aucSerialNumber) !=0 )
		nRet = 3;  //序列号校验失败

	if(strcmp(pVecleName,(char*)m_LicenseData.auVechicleCode) !=0 )
		nRet = 4;  //车系代码校验失败
	
	//if(abs(fVersion - atof((char*)m_LicenseData.auDiagVersion)) > 0.0001)
	float bbb = atof((char*)m_LicenseData.auDiagVersion);
	float aaa = fabs(fVersion - bbb);

	if(aaa > 0.0001)
		nRet = 5;   //诊断版本校验失败

	if(strcmp((char*)m_LicenseData.ucExpiredData,"00000000") !=0 )
	{
		if(atol((char*)m_LicenseData.ucCreateData) > atol((char*)m_LicenseData.ucExpiredData) )
			nRet = 6;  //过期
	}

	if( atol( (char*)m_LicenseData.ucUseCount ) != 0 )
	{
		if( (long)nUsedCount > atol((char*)m_LicenseData.ucUseCount) )
			nRet =  7; //超过使用次数
	}

	if(pBuf)
	{
		delete []pBuf;
		pBuf = NULL;
	}
	fclose(m_pfText);
	return nRet;
}
#endif
//#endif

#ifdef _ENCRYP	
//生成许可
bool CSecurityBaseClass::CreateLisence(const char *szSerialNumber,const char* szSoftWareCode,const char* szLanguage,double fVersion,char *pData)
{
	//CTime tTime=CTime::GetCurrentTime();
	CHAR szTime[9]="88888888";
    CHAR*  mac_key="58073424";
	//sprintf(szTime,"%02d%02d%02d%02d",tTime.GetDay(),tTime.GetHour(),tTime.GetMinute(),tTime.GetSecond());
	
	for(int si = 0;si<2;si++)
	{
		CHAR pD[9]="";
		CHAR pP[9]="";
		memcpy(pD,szTime+si*6,6);
		stkdes_en(pD,mac_key,pP);
	}

	int n = strlen(szSerialNumber);	
	if(n>12) return FALSE;
	CHAR *pInputBuf = new CHAR[13];
	memset(pInputBuf,0,13);	
	sprintf(pInputBuf,"%s",szSerialNumber);
	memset(pInputBuf+n,' ',12-n);
	for(int i=0; i<2; i++)
	{
		CHAR pD[9]="";
		CHAR pP[9]="";
		memcpy(pD,pInputBuf+i*6,6);
		//stkEncryptPin((CHAR*)pD,mac_key);
		stkdes_en(pD,szTime,pP);
		memcpy(pData+i*8,pP,8);
	}
	//多种语言，需要同时打包，中间用逗号隔开
	//最多支持5种语言

	int nPos   = 0;
	int nStart = 0;
	char clanguageArray[30]="";
	strcpy(clanguageArray,szLanguage);

	for(int j=0;j<5;j++)
	{	
		CHAR pD[7]="";
		CHAR pP[9]="";
		CHAR pT[7]="";
	   char *pdest;

		pdest = strchr( clanguageArray+nStart, ',');
		if(pdest != NULL)
		{
			nPos = pdest - clanguageArray;
			memcpy(pD,clanguageArray+nStart,nPos - nStart);
			nStart += nPos;
		}
		else
		{
			memset(pD,' ',6);
		}

		int n = strlen(pD);
		if(n>6) 
			return FALSE;
		sprintf(pT,"%s",pD);
		memset(pT+n,' ',6-n);
		stkdes_en(pT,szTime,pP);
		memcpy(pData+16+j*8,pP,8);
		nStart = nPos + 1;
	}

#if 0
	for(int j=0;j<5;j++)
	{	
		CHAR pD[7]="";
		CHAR pP[9]="";
		CHAR pT[7]="";
		strchr(szLanguage
		nPos = szLanguage.Find(",",nStart);
		if(nPos == -1 && nStart == 0) 
		{
			sprintf(pD,"%s",szLanguage);
		}
		else if(nPos == -1 && nStart != 0)
		{
			memset(pD,' ',6);
		}
		else
		{
			sprintf(pD,"%s",szLanguage.Mid(nStart,nPos-nStart));
		}
		int n = strlen(pD);
		if(n>6) 
			return FALSE;
		sprintf(pT,"%s",pD);
		memset(pT+n,' ',6-n);
		stkdes_en(pT,szTime,pP);
		memcpy(pData+16+j*8,pP,8);
		nStart = nPos + 1;
	}
#endif

	CHAR pVersion[9]="";
	sprintf(pVersion,"%.2f",fVersion);
	n = strlen(pVersion);
	memset(pVersion+n,' ',8-n);
	for(i = 0;i<2;i++)
	{
		CHAR pD[9]="";
		CHAR pP[9]="";
		memcpy(pD,pVersion+i*6,6);
		stkdes_en(pD,szTime,pP);
		memcpy(pData+56+i*8,pP,8);
	}

	n = strlen(szSoftWareCode);	
	if(n>30) return FALSE;
	CHAR pSoftCode[31]="";
	sprintf(pSoftCode,"%s",szSoftWareCode);
	memset(pSoftCode+n,' ',30-n);
	for(i=0; i<5; i++)
	{
		CHAR pD[9]="";
		CHAR pP[9]="";
		memcpy(pD,pSoftCode+i*6,6);
		stkdes_en(pD,szTime,pP);
		memcpy(pData+72+i*8,pP,8);
	}
	
	for(i = 0;i<2;i++)
	{
		CHAR pD[9]="";
		CHAR pP[9]="";
		memcpy(pD,szTime+i*6,6);
		stkdes_en(pD,mac_key,pP);
		memcpy(pData+112+i*8,pP,8);
	}
//	取随机数把后面的空间填满
	for(int s= 0;s<240;s++)
	{
		CHAR pStr[7] = "";
		for(i=0;i<6;i++)
		{
		   //srand( (unsigned)time( NULL ) );
		   int nRand = rand();	
		   int n = nRand%36;	
		   pStr[i] = pRand[n];
		}
		CHAR pEnstr[9]="";
		CHAR pP[9]="";
		sprintf(pEnstr,"%s",pStr);
		stkdes_en(pEnstr,szTime,pP);
		memcpy(pData+128+s*8,pP,8);
	}

	CEncrypClass en;
	en.KeyGen((DWORD)85165476);
	en.EncodeBuffer((unsigned char*)pData,128);

	if(pInputBuf)
	{
		delete []pInputBuf;
		pInputBuf = NULL;
	}
	return TRUE;
}	
#endif



//#ifdef _DECRYP	
//解密	
void CSecurityBaseClass::DecryptBuffer(const char *pInBuffer,int nInputLen,char *pOutBuffer)
{
		CRijndael oRijndael;
		//oRijndael.MakeKey("olkdddjrexklxuybbsamnbvrteo841dg", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32, 32);
		oRijndael.MakeKey("xbhqmhlhpblyjhp12n9y1rqwertyuiop", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32, 32);


		int nLen = nInputLen;
		int nOutLen = nLen;
		int nCount = nLen/BLOCK_SIZE;

		for(int i=0;i<nCount;i++)
		{	
			char *szDataIn = NULL;
			szDataIn = new char[BLOCK_SIZE];
			memcpy(szDataIn,pInBuffer,BLOCK_SIZE);
			char szDataOut[33];
			memset(szDataOut,0,33);
			oRijndael.DecryptBlock(szDataIn, szDataOut);
			if(i == nCount -1)
				memcpy(pOutBuffer+i*32,szDataOut,BLOCK_SIZE);
			else
				strcpy(pOutBuffer+i*32,szDataOut);
			if(szDataIn)
			{
				delete []szDataIn;
				szDataIn = NULL;
			}
		}
}
//#endif

#define DECRYPT_LEN 16
//#ifdef _ENCRYP	
//加密
void CSecurityBaseClass::EncryptData(const char *pInBuffer,int nInputLen, char *pOutBuffer)
{
#if 1
		CRijndael oRijndael;
		//oRijndael.MakeKey("olkdddjrexklxuyb", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
		oRijndael.MakeKey("xbhqmhlhpblyjhp12n9y1rqwertyuiop", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32, 32);

		int nLen = nInputLen;
		int nOutLen = nLen;
		int nCount = nLen/DECRYPT_LEN;
		int nRemain = nLen%DECRYPT_LEN;
		if(nRemain>0)
		{
			nOutLen = (nCount+1)*DECRYPT_LEN;
		}
		//char *pOutBuffer = NULL;
		//pOutBuffer = new char[nOutLen];
		
		memset(pOutBuffer,0,nOutLen);
		for(int i=0;i<nCount;i++)
		{	
			char szDataIn[DECRYPT_LEN+1];
			memset(szDataIn,0,DECRYPT_LEN+1);
			memcpy(szDataIn,pInBuffer+i*DECRYPT_LEN,DECRYPT_LEN);
			char szDataOut[DECRYPT_LEN+1];
			memset(szDataOut,0,DECRYPT_LEN+1);
			oRijndael.EncryptBlock(szDataIn, szDataOut);
			memcpy(pOutBuffer+i*DECRYPT_LEN,szDataOut,DECRYPT_LEN);
		}
		if(nRemain>0)
		{
			char *szDataIn1 = NULL;
			szDataIn1 = new char[nRemain+1];
			memset(szDataIn1,0,nRemain+1);
			memcpy(szDataIn1,pInBuffer+nCount*DECRYPT_LEN,nRemain);
			char szDataOut[DECRYPT_LEN+1];
			memset(szDataOut,0,DECRYPT_LEN+1);
			oRijndael.EncryptBlock(szDataIn1, szDataOut);
			memcpy(pOutBuffer+nCount*DECRYPT_LEN,szDataOut,DECRYPT_LEN);
			if(szDataIn1)
			{
				delete []szDataIn1;
				szDataIn1 = NULL;
			}
		}
		//pOut = pOutBuffer;
#else

#endif
}
//#endif


void CSecurityBaseClass::DecryptData(const char *pInBuffer,int nInputLen,char *pOutBuffer)
{
		CRijndael oRijndael;
		//oRijndael.MakeKey("olkdddjrexklxuyb", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
		oRijndael.MakeKey("xbhqmhlhpblyjhp12n9y1rqwertyuiop", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32, 32);

		int nLen = nInputLen;
		int nOutLen = nLen;
		int nCount = nLen/DECRYPT_LEN;

		for(int i=0;i<nCount;i++)
		{	
			char *szDataIn = NULL;
			szDataIn = new char[DECRYPT_LEN];
			memcpy(szDataIn,pInBuffer+i*DECRYPT_LEN,DECRYPT_LEN);
			char szDataOut[DECRYPT_LEN+1];
			memset(szDataOut,0,DECRYPT_LEN+1);
			oRijndael.DecryptBlock(szDataIn, szDataOut);
			if(i == nCount -1)
				memcpy(pOutBuffer+i*DECRYPT_LEN,szDataOut,DECRYPT_LEN);
			else
				strcpy(pOutBuffer+i*DECRYPT_LEN,szDataOut);
			if(szDataIn)
			{
				delete []szDataIn;
				szDataIn = NULL;
			}
		}
}
//#endif

//#ifdef _DECRYP	
//校验文件的合法性
bool CSecurityBaseClass::CheckObjectFile(const char *pchFileName)
{
	CCrcClass crc;
	unsigned long dwCrc32;
	unsigned long dwCrc32_1;
	crc.Init();
	crc.FileCrc(pchFileName,16,dwCrc32);
	
	FILE *m_pfText = NULL;
	unsigned char mac[16];
	memset(mac,0,16);
	unsigned char macEN[16];
	memset(macEN,0,16);
	//unsigned char Signature[16];
	m_pfText = fopen(pchFileName, "rb");
	if(m_pfText)
	{
			
	
		fseek(m_pfText,-16,SEEK_END);
		int nRead = fread((unsigned char*)macEN,1,16,m_pfText);	
		CRijndael oRijndael;
		oRijndael.MakeKey("olkdadjrlxuymnrt", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
		oRijndael.DecryptBlock((const  char*)macEN, (char*)mac);


		//fseek(m_pfText,-8,SEEK_END);
		//nRead = fread((unsigned char*)Signature,1,8,m_pfText);
		if(strncmp((char*)mac+8,"AUTOBOSS",8) !=0)
		{
			fclose(m_pfText);
			return false;
		}
		fclose(m_pfText);
	}
	else
	{
		return false;
	}
	dwCrc32_1 = MAKEDW(mac[0],mac[1],mac[2],mac[3]);
	if(dwCrc32_1 == dwCrc32)
		return true;
	else
		return false;
}
//#endif


unsigned long CSecurityBaseClass::GetNewMCUMainVersion(char * filedata)
{
	char Head_Key[9] = "ZAQWERFV";
	char plainText[9];
	char key[9];
	unsigned long ulVersion;
	//先解密前2k字节数据
	for(int i=0;i<256;i++)
	{
		stkdes_de(filedata+i*8,Head_Key,plainText);
		for(int j=0;j<8;j++)
			filedata[i*8+j] = plainText[j];
	}

	//校验是否是合法数据
	if(filedata[0] != 'A'
		||filedata[1] != 'U'
		||filedata[2] != 'T'
		||filedata[3] != 'O'
		||filedata[4] != 'B'
		||filedata[5] != 'O'
		||filedata[6] != 'S'
		||filedata[7] != 'S'
		)
	{
		ulVersion = 0;
	}

	else
	{
		int keyoffset = (unsigned char)filedata[10]*256 + (unsigned char)filedata[11];
		memcpy(key,filedata+keyoffset,8);
		DecryptString(key);
		
		//直接解密 0x800+0x400处的8个字节
		stkdes_de(filedata+2048+0x400,key,plainText);
		ulVersion = *((unsigned long *)plainText);
	}
	return ulVersion;

}

void CSecurityBaseClass:: DecryptString(char* str)
{
	 char* p = str;
	 int i = 0;
	 while(*p != '\0')
	  *p++ ^= i++;

	 *(str+1) -= 0x37;
	 *(str+2) -= 0x37;
	 *(str+4) -= 0x37;

	 *(str+0) -= 0x46;
	 *(str+3) -= 0x46;
	 *(str+6) -= 0x46;
	 *(str+7) -= 0x46;
	
	 //右移一个字节
	 for(i=7;i>0;i--)
	 {
		 *(str+i) = *(str+i-1);
	 }
	 *str = *(str+7) ;

}
