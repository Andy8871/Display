/*************************************************************
  Copyright (c) 2006, AUTOBOSS Ltd,.Co 
  All rights reserved.
 
  文件名称：RunEnvironment.cpp
  文档标识：运行环境类
  摘    要：设置取得：显示程序工作目录；诊断程序工作目录；语言代码

  历史记录：
  ----------------------------------------------------------------------------
  时     间   作者   版本号	  操    作    内    容                               
  ----------------------------------------------------------------------------
  2006.1.9    sujer  1.0      创建此类

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

#define MAX_LANGUAGE_ITEM 30 //语言种类

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
  功    能：设置显示程序工作目录
  参数说明：char* pDirectory-指向工作目录的指针
  返 回 值：无
  说    明：无
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetDisplayDirectory(const char* pDirectory)
{
	m_strDisplayDirectory = pDirectory;
}

/*-----------------------------------------------------------------------------
  功    能：取得显示程序工作目录
  参数说明：无
  返 回 值：工作目录绝对路径名
  说    明：无
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetDisplayDirectory()
{
	return m_strDisplayDirectory;
}

/*-----------------------------------------------------------------------------
  功    能：设置诊断程序工作目录
  参数说明：char* pDirectory-指向工作目录的指针
  返 回 值：无
  说    明：无
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetDiagnosisDirectory(const char* pDirectory)
{
	m_strDiagnosisDirectory = pDirectory;
}

/*-----------------------------------------------------------------------------
  功    能：取得诊断程序工作目录
  参数说明：无
  返 回 值：工作目录绝对路径名
  说    明：无
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetDiagnosisDirectory()
{
	return m_strDiagnosisDirectory;
}

/*-----------------------------------------------------------------------------
  功    能：设置语言代码
  参数说明：char* pLanguage-指向设置语言代码字符串的指针
  返 回 值：无
  说    明：无
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetLanguage(const char* pLanguage)
{
	m_strLanguage = pLanguage;
}

/*-----------------------------------------------------------------------------
  功    能：取得语言代码
  参数说明：无
  返 回 值：语言代码字符串
  说    明：无
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetLanguage()
{
	return m_strLanguage;
}


//add by scf 2007.2.13
/*-----------------------------------------------------------------------------
  功    能：设置机器序列号
  参数说明：char* pSerialNumber-指向设置机器序列号字符串的指针
  返 回 值：无
  说    明：无
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetSerialNumber(const char* pSerialNumber)
{
	m_strSerialNumber = pSerialNumber;
}


//add by scf 2007.10.09
/*-----------------------------------------------------------------------------
  功    能：设置演示模式
  参数说明：bDemoMode-1:演示模式 0：测车模式
  返 回 值：无
  说    明：无
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
  功    能：取得机器序列号
  参数说明：无
  返 回 值：机器序列号字符串
  说    明：无
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetSerialNumber()
{
	return m_strSerialNumber;
}	

//end add  2007.2.13	

//add by scf 2007.10.09
/*-----------------------------------------------------------------------------
  功    能：设置演示模式
  参数说明：bDemoMode-1:演示模式 0：测车模式
  返 回 值：无
  说    明：无
-----------------------------------------------------------------------------*/
bool CRunEnvironment::GetDemoMode()
{
	return m_bDemoMode;
}


/*-----------------------------------------------------------------------------
  功    能：得到显示屏幕类型
  参数说明：
  返 回 值：0:320*240 1:640*480
  说    明：无
-----------------------------------------------------------------------------*/
unsigned char CRunEnvironment::GetScreenType()
{
	return m_cScreenType;
}

/*-----------------------------------------------------------------------------
  功    能：设置显示屏幕类型
  参数说明：0:320*240 1:640*480
  返 回 值：
  说    明：无
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetScreenType(unsigned char uiType)
{
	m_cScreenType = uiType;
}


/*-----------------------------------------------------------------------------
  功    能：设置显示程序版本号
  参数说明：char* pDisplayVersion-版本号
  返 回 值：无
  说    明：无
-----------------------------------------------------------------------------*/
void CRunEnvironment::SetDisplayVersion(char* pDisplayVersion)
{
	m_strDisplayVersion = pDisplayVersion;
}

/*-----------------------------------------------------------------------------
  功    能：取得显示程序工作目录
  参数说明：无
  返 回 值：工作目录绝对路径名
  说    明：无
-----------------------------------------------------------------------------*/
string CRunEnvironment::GetDisplayVersion()
{
	return m_strDisplayVersion;
}

/*-----------------------------------------------------------------------------
  功    能：MCU标志信息
  参数说明：TRUE: new mcu   false: old mcu
  返 回 值：
  说    明：无
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
  功    能：设置产品类型
  参数说明：
		  1:V30黑白
		  2:V30彩屏
		  3:DK80
		  4:PCMAX
		  5:PVVCI  //澳洲PCMAX
		  6:PCLINK
		  7:EUROPCMAX //欧洲PCMAX
		  ...

-----------------------------------------------------------------------------*/
void CRunEnvironment::SetProductType(unsigned char uiProductType)
{
	m_ProuctType = uiProductType;
}

/*-----------------------------------------------------------------------------
  功    能：获取产品类型
  参数说明：
		  1:V30黑白
		  2:V30彩屏
		  3:DK80
		  4:PCMAX
		  5:PVVCI  //澳洲PCMAX
		  6:PCLINK
		  7:EUROPCMAX //欧洲PCMAX
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
  功    能：获取单位类型
  参数说明：
		  1:V30黑白
		  2:V30彩屏
-----------------------------------------------------------------------------*/
unsigned char CRunEnvironment::GetUnitType()
{
	return m_UnitType;
}
//////////////////////////////////////////////////////////////////////////

//获取Language//hpy add
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

//获取CodePage//hpy add
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
