/*************************************************************
  Copyright (c) 2006, AUTOBOSS Ltd,.Co 
  All rights reserved.
 
  �ļ����ƣ�RunEnvironment.cpp
  �ĵ���ʶ�����л�����
  ժ    Ҫ������ȡ�ã���ʾ������Ŀ¼����ϳ�����Ŀ¼�����Դ���

  ��ʷ��¼��
  ----------------------------------------------------------------------------
  ʱ     ��   ����   �汾��	  ��    ��    ��    ��                               
  ----------------------------------------------------------------------------
  2006.1.9    sujer  1.0      ��������

****************************************************************/
#include "RunEnvironment.h"


string CRunEnvironment::m_strDisplayDirectory;
string CRunEnvironment::m_strDiagnosisDirectory;
string CRunEnvironment::m_strLanguage;
string CRunEnvironment::m_strDisplayVersion;
//add by scf 2007.2.13
string CRunEnvironment::m_strSerialNumber;
//add by scf 2007.10.09
bool CRunEnvironment::m_bDemoMode;
unsigned char CRunEnvironment::m_cScreenType;
unsigned char CRunEnvironment::m_ProuctType;

unsigned char CRunEnvironment::m_UnitType;//hpy add 20131126

string CRunEnvironment::m_ECUIDinfo = "";
bool CRunEnvironment::m_bNewMcuFlag = false;
bool CRunEnvironment::m_bDsPrintMode = false;
#ifdef DATA_DRV_EXT
bool    CRunEnvironment::m_cHeckMode;
string CRunEnvironment::m_menuChoosedDir = "";
unsigned int CRunEnvironment::m_CodePage;
#endif

#define MAX_LANGUAGE_ITEM 30 //��������

//;0=UTF8(default),1=MBCS,2=UNICODE
int CRunEnvironment::m_iDBFormat = 0;

LANGUAGE_ITEM LanguageItem[MAX_LANGUAGE_ITEM] = //hpy add
{
	//"CountryCode","Language",CodePage
	/*1*/{("CN"),("Chinese(Simplified)"),936},
	/*2*/{("TW"),("Chinese(Traditional)"),936},
	/*3*/{("EN"),("English"),936},
	/*4*/{("RU"),("Russian"),1251},
	/*5*/{("KR"),("Korean"),949},
	/*6*/{("TR"),("Turkish"),1254},//old 1252, modify by ben.peng 20130827
	/*7*/{("PO"),("Polish"),1250},//old 1252, modify by ben.peng 20130827
	/*8*/{("FR"),("French"),1252},
	/*9*/{("SP"),("Spanish"),1252},
	/*10*/{("HU"),("Hungarian"),1252},
	/*11*/{("JP"),("Japanese"),932},
	/*12*/{("IT"),("Italiano"),1252},
	/*13*/{("FR"),("Francais"),1252},
	/*14*/{("AR"),("Deutsch"),1252},
	/*15*/{("CZ"),("Czech Republic"),1250},
	/*16*/{("PT"),("Portugal"),1252},
	/*17*/{("SU"),("Farsi"),1256},
	/*18*/{("IN"),("Bahasa Indonesia"),936},
	/*19*/{("AA"),("Arabia"),1256},
	/*20*/{("TH"),("THAI"),874},
	/*21*/{("NE"),("Dutch"),1252},
	/*22*/{("VI"),("Vietnam"),1258}
};

/*-----------------------------------------------------------------------------
  ��    �ܣ�������ʾ������Ŀ¼
  ����˵����char* pDirectory-ָ����Ŀ¼��ָ��
  �� �� ֵ����
  ˵    ������
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetDisplayDirectory(const char* pDirectory)
{
	m_strDisplayDirectory = pDirectory;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ�ȡ����ʾ������Ŀ¼
  ����˵������
  �� �� ֵ������Ŀ¼����·����
  ˵    ������
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetDisplayDirectory()
{
	return m_strDisplayDirectory;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ�������ϳ�����Ŀ¼
  ����˵����char* pDirectory-ָ����Ŀ¼��ָ��
  �� �� ֵ����
  ˵    ������
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetDiagnosisDirectory(const char* pDirectory)
{
	m_strDiagnosisDirectory = pDirectory;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ�ȡ����ϳ�����Ŀ¼
  ����˵������
  �� �� ֵ������Ŀ¼����·����
  ˵    ������
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetDiagnosisDirectory()
{
	return m_strDiagnosisDirectory;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ��������Դ���
  ����˵����char* pLanguage-ָ���������Դ����ַ�����ָ��
  �� �� ֵ����
  ˵    ������
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetLanguage(const char* pLanguage)
{
	m_strLanguage = pLanguage;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ�ȡ�����Դ���
  ����˵������
  �� �� ֵ�����Դ����ַ���
  ˵    ������
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetLanguage()
{
	return m_strLanguage;
}


//add by scf 2007.2.13
/*-----------------------------------------------------------------------------
  ��    �ܣ����û������к�
  ����˵����char* pSerialNumber-ָ�����û������к��ַ�����ָ��
  �� �� ֵ����
  ˵    ������
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetSerialNumber(const char* pSerialNumber)
{
	m_strSerialNumber = pSerialNumber;
}


//add by scf 2007.10.09
/*-----------------------------------------------------------------------------
  ��    �ܣ�������ʾģʽ
  ����˵����bDemoMode-1:��ʾģʽ 0���⳵ģʽ
  �� �� ֵ����
  ˵    ������
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetDemoMode(bool bDemoMode)
{
	m_bDemoMode = bDemoMode;
}
#ifdef DATA_DRV_EXT
void CRunEnvironment::SetCheckMode(bool checkMode)
{
	m_cHeckMode = checkMode;
}

bool CRunEnvironment::GetCheckMode(void){
	return m_cHeckMode;
}
void CRunEnvironment::SetMenuChoosedDir(string& dir){
	m_menuChoosedDir = dir;
}
string CRunEnvironment::GetMenuChoosedDir(void){
	return m_menuChoosedDir;
}
void CRunEnvironment::SetCodePage(unsigned int nCodePage){
	m_CodePage = nCodePage;
}

unsigned int CRunEnvironment::GetCodePage(void){
	return m_CodePage;
}

void CRunEnvironment::SetDsPrintMode(bool bMode){
	m_bDsPrintMode = bMode;
}

bool CRunEnvironment::GetDsPrintMode(void){
	return m_bDsPrintMode;
}

#endif
void CRunEnvironment::SetECUInfo(string& info){
	m_ECUIDinfo = info;
}


/*-----------------------------------------------------------------------------
  ��    �ܣ�ȡ�û������к�
  ����˵������
  �� �� ֵ���������к��ַ���
  ˵    ������
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetSerialNumber()
{
	return m_strSerialNumber;
}	

//end add  2007.2.13	

//add by scf 2007.10.09
/*-----------------------------------------------------------------------------
  ��    �ܣ�������ʾģʽ
  ����˵����bDemoMode-1:��ʾģʽ 0���⳵ģʽ
  �� �� ֵ����
  ˵    ������
-----------------------------------------------------------------------------*/
bool CRunEnvironment::GetDemoMode()
{
	return m_bDemoMode;
}


/*-----------------------------------------------------------------------------
  ��    �ܣ��õ���ʾ��Ļ����
  ����˵����
  �� �� ֵ��0:320*240 1:640*480
  ˵    ������
-----------------------------------------------------------------------------*/
unsigned char CRunEnvironment::GetScreenType()
{
	return m_cScreenType;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ�������ʾ��Ļ����
  ����˵����0:320*240 1:640*480
  �� �� ֵ��
  ˵    ������
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetScreenType(unsigned char uiType)
{
	m_cScreenType = uiType;
}


/*-----------------------------------------------------------------------------
  ��    �ܣ�������ʾ����汾��
  ����˵����char* pDisplayVersion-�汾��
  �� �� ֵ����
  ˵    ������
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetDisplayVersion(char* pDisplayVersion)
{
	m_strDisplayVersion = pDisplayVersion;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ�ȡ����ʾ������Ŀ¼
  ����˵������
  �� �� ֵ������Ŀ¼����·����
  ˵    ������
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetDisplayVersion()
{
	return m_strDisplayVersion;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ�MCU��־��Ϣ
  ����˵����TRUE: new mcu   false: old mcu
  �� �� ֵ��
  ˵    ������
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetNewMcuFlag(bool bFlag)
{
	m_bNewMcuFlag = bFlag;
}

bool CRunEnvironment::GetNewMcuFlag()
{
	return m_bNewMcuFlag;
}


/*-----------------------------------------------------------------------------
  ��    �ܣ����ò�Ʒ����
  ����˵����
		  1:V30�ڰ�
		  2:V30����
		  3:DK80
		  4:PCMAX
		  5:PVVCI  //����PCMAX
		  6:PCLINK
		  7:EUROPCMAX //ŷ��PCMAX
		  ...

-----------------------------------------------------------------------------*/
void CRunEnvironment::SetProductType(unsigned char uiProductType)
{
	m_ProuctType = uiProductType;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ���ȡ��Ʒ����
  ����˵����
		  1:V30�ڰ�
		  2:V30����
		  3:DK80
		  4:PCMAX
		  5:PVVCI  //����PCMAX
		  6:PCLINK
		  7:EUROPCMAX //ŷ��PCMAX
		  ...

-----------------------------------------------------------------------------*/
unsigned char CRunEnvironment::GetProductType()
{
	return m_ProuctType;
}


//////////////////////////////////////////////////////////////////////////
void CRunEnvironment::SetUnitType(unsigned char uiUnitType)
{
	m_UnitType = uiUnitType;
}

/*-----------------------------------------------------------------------------
  ��    �ܣ���ȡ��λ����
  ����˵����
		  1:V30�ڰ�
		  2:V30����
-----------------------------------------------------------------------------*/
unsigned char CRunEnvironment::GetUnitType()
{
	return m_UnitType;
}
//////////////////////////////////////////////////////////////////////////

//��ȡLanguage//hpy add
string CRunEnvironment::GetItemLanguage(string CountryCode)
{
	if( !CountryCode.length())
	{
		return "";
	}

	for(unsigned int i=0; i<MAX_LANGUAGE_ITEM; i++)
	{
		string csTmp1(LanguageItem[i].csCountryCode);
		if(CountryCode == csTmp1)
		{
			string csTmp2(LanguageItem[i].csLanguage);
			return csTmp2;	
		}
	}

	return "";
}

//��ȡCodePage//hpy add
unsigned int CRunEnvironment::GetItemCodePage(string CountryCode)
{
	if( !CountryCode.length() )
	{
		return 0;
	}

	for(unsigned int i=0; i<MAX_LANGUAGE_ITEM; i++)
	{
		string csTmp(LanguageItem[i].csCountryCode);
		if(CountryCode == csTmp)
		{
			return LanguageItem[i].iCodePage;	
		}
	}

	return 0;
}	

//BEN ADD 20131008
void CRunEnvironment::SetDBFormat(int iDBFormat)
{
	m_iDBFormat = iDBFormat;
}

int CRunEnvironment::GetDBFormat()
{
	return m_iDBFormat;
}
