/******************************************************************************

  Copyright (c) 2006, AUTOBOSS Ltd,.Co 
  All rights reserved.
 
  文件名称：Profile.c
  文档标识：007汽车诊断平台详细设计说明书--显示程序分册 V1.0
  摘    要：用于读写配置文件；功能包括：读字符串、写字符串、读整型数据、写整型
            数据。

  历史记录：
  ----------------------------------------------------------------------------
  时     间   作者   版本号	  操    作    内    容                               
  ----------------------------------------------------------------------------
  2006.01.21  wcx    1.0AX     创建                                           

******************************************************************************/
#include "adsStd.h"
#include "Profile.h"
#include "RunEnvironment.h"

void MakeLowerString(string& strSource)
{
	for (int i=0; i<strSource.length(); i++)
	{
		if(strSource[i]>='A' && strSource[i]<='Z')
			strSource[i]=strSource[i]+32;
	}
}


/*-----------------------------------------------------------------------------
  功    能：读取配置串
  参数说明：string strFileName 配置文件名,
            string strSectionName 段名, 
            string strKeyName 键值,
            string strDefaultString 缺省串
  返 回 值：读入的字符串
  说    明：无
-----------------------------------------------------------------------------*/
string CProfile::GetString (string strFileName, string strSectionName, string strKeyName, string strDefaultString, bool bCaseSensitive)
{
	CTextFile TextFile;

	if(TextFile.LoadFile(strFileName))
		return strDefaultString;
	
	string strRusult = strDefaultString;
	string strTemp;
	bool bFoundSection = false;
	while (!TextFile.IsEof()){
		strTemp = TextFile.GetLine();
		if(!bFoundSection){
			if(TextFile.IsSectionLine(strTemp))
				if(TextFile.SectionNameIs(strTemp,strSectionName)){
					bFoundSection = true;
			}
		}
		else {
			if(TextFile.IsSectionLine(strTemp))
				break;
			if(TextFile.IsKeyLine(strTemp)){
				string strTemp2=strTemp;
				string strKeyName2=strKeyName;
				if (!bCaseSensitive)
				{
					MakeLowerString(strTemp2);
					MakeLowerString(strKeyName2);
				}
				if(TextFile.KeyNameIs(strTemp2, strKeyName2)){
					strRusult = TextFile.GetValueString(strTemp);
					break;
				}
			}
		}
	}
	return strRusult;
}


/*-----------------------------------------------------------------------------
  功    能：写入配置串
  参数说明：string strFileName 配置文件名,
            string strSectionName 段名,
            string strKeyName 键值,
            string strString 写入串
  返 回 值：成功--false 失败--true
  说    明：无
-----------------------------------------------------------------------------*/
bool CProfile::WriteString (string strFileName, string strSectionName, string strKeyName, string strValue, bool bCaseSensitive)
{
	CTextFile TextFile;
	TextFile.LoadFile(strFileName);

	FILE *fp;
	if ((fp = fopen(strFileName.c_str(),"w+")) == NULL)
	{
		//重试一次
		adsSleep(20);
		if ((fp = fopen(strFileName.c_str(),"w+")) == NULL)
			return true;
	}

	bool bFoundSection = false;
	bool bWriteKeyFinish = false;
	string strTemp;

	while (!TextFile.IsEof()) {
		strTemp = TextFile.GetLine();
		if (!bFoundSection) {
			if (TextFile.IsSectionLine(strTemp)){
				if(TextFile.SectionNameIs(strTemp,strSectionName))
					bFoundSection = true;
			}
		}
		else {
			if(TextFile.IsKeyLine(strTemp)){
				string strTemp2=strTemp;
				string strKeyName2=strKeyName;
				if (!bCaseSensitive)
				{
					MakeLowerString(strTemp2);
					MakeLowerString(strKeyName2);
				}
				if(TextFile.KeyNameIs(strTemp2, strKeyName2)){
					strTemp = TextFile.SetValueString(strTemp, strValue);
					strTemp += "\r\n"; 
					fwrite(strTemp.c_str(), 1, strTemp.length(), fp);
					bWriteKeyFinish = true;
					break;
				}
			}
			else if(TextFile.IsSectionLine(strTemp)) {
				strTemp = "[" + strSectionName + "]\r\n"; 
				fwrite(strTemp.c_str(), 1, strTemp.length(), fp);
				strTemp = strKeyName + " = " + strValue + "\r\n  \r\n"; 
				fwrite(strTemp.c_str(), 1, strTemp.length(), fp);
				bWriteKeyFinish = true;
				break;
			}
		}

		strTemp += "\r\n"; 
		fwrite(strTemp.c_str(), 1, strTemp.length(), fp);
	}

	char *pCurrent = TextFile.GetCurrentPointer();
	fwrite (pCurrent, 1, strlen(pCurrent), fp);

	if (!bWriteKeyFinish) {
		if (!bFoundSection) {
			strTemp = "[" + strSectionName + "]\r\n"; 
			fwrite(strTemp.c_str(), 1, strTemp.length(), fp);
		}
		strTemp = strKeyName + " = " + strValue + "\r\n  \r\n"; 
		fwrite(strTemp.c_str(), 1, strTemp.length(), fp);
	}

	fclose(fp);
	
	return false;
}


/*-----------------------------------------------------------------------------
  功    能：读取配置数值
  参数说明：string strFileName 配置文件名,
            string strSectionName 段名,
            string strKeyName 键值,
            int iDefaultValue 缺省值
  返 回 值：读入的数值
  说    明：无
-----------------------------------------------------------------------------*/
int CProfile::GetInteger (string strFileName, string strSectionName, string strKeyName, int intDefaultValue, bool bCaseSensitive) 
{
	char szTemp[50];

	sprintf(szTemp,"%d",intDefaultValue);
	string strTemp = GetString (strFileName, strSectionName, strKeyName, szTemp, bCaseSensitive);

	return atoi(strTemp.c_str());
}


/*-----------------------------------------------------------------------------
  功    能：读取配置串
  参数说明：string strFileName 配置文件名,
            string strSectionName 段名,
            string strKeyName 键值,
            int iValue 写入值
  返 回 值：成功--false 失败--true
  说    明：无
-----------------------------------------------------------------------------*/
bool CProfile::WriteInteger (string strFileName, string strSectionName, string strKeyName, int iValue, bool bCaseSensitive) 
{
	char szTemp[50];
	sprintf(szTemp,"%d",iValue);
	
	return WriteString(strFileName, strSectionName, strKeyName, szTemp, bCaseSensitive);
}

string CProfile::GetString(string strElemName, string strAttribute, string strDefaultString)
{
	string strRet = strDefaultString;
	while (true)
	{
		char pszFilePath[128];
		string strDisplayDir = CRunEnvironment::GetDisplayDirectory();
		sprintf(pszFilePath, "%sConfig.xml", strDisplayDir.c_str());
		//CString strFilePath = CRunEnvironment::GetDisplayDirectory() + _T("Config.xml");
		TiXmlDocument xmlDoc;
		
		if (!xmlDoc.LoadFile(pszFilePath))
			break;

		TiXmlElement* pXmlRoot = xmlDoc.RootElement();
		TiXmlElement* pXmlElem = pXmlRoot;
		const char* pszElem = strElemName.c_str();//UnicodeToUTF8(strElemName.GetBuffer());
		if (!GetElementByName(pXmlRoot, pszElem, pXmlElem))
			break;

		if (pXmlElem)
		{
			const char* pszAttribute = strAttribute.c_str();//UnicodeToUTF8(strAttribute.GetBuffer());
			strRet = pXmlElem->Attribute(pszAttribute);
			if (strRet.empty())
				strRet = strDefaultString;
		}
		break;
	}
	return strRet;
}


int CProfile::GetInteger(string strElemName, string strAttribute, int iDefaultValue)
{
	char szDefaultValue[32];
	sprintf(szDefaultValue, "%d", iDefaultValue);//strDefaultValue.Format(_T("%d"), iDefaultValue);
	string strRet = GetString(strElemName, strAttribute, szDefaultValue);
	const char* pszValue = strRet.c_str();
	int iValue = atoi(pszValue);
	return iValue;
}

bool CProfile::WriteString(string strElemName, string strAttribute, string strValue)
{
	bool bRet = false;
	while (true)
	{
		char pszFilePath[128];
		string strDisplayDir = CRunEnvironment::GetDisplayDirectory();
		sprintf(pszFilePath, "%sConfig.xml", strDisplayDir.c_str());

		TiXmlDocument xmlDoc;
		if (!xmlDoc.LoadFile(pszFilePath))
			break;

		TiXmlElement* pRoot = xmlDoc.RootElement();
		TiXmlElement* pElem = pRoot;

		const char* pszElem = strElemName.c_str();
		if (!GetElementByName(pRoot, pszElem, pElem))
			break;

		const char* pszAttribute = strAttribute.c_str();
		const char* pszValue = strValue.c_str();
		pElem->SetAttribute(pszAttribute, pszValue);
		if (!xmlDoc.SaveFile(pszFilePath))
			break;

		bRet = true;
		break;
	}
	return bRet;
}

bool CProfile::WriteInteger(string strElemName, string strAttribute, int iValue)
{
	char szTemp[50];
	sprintf(szTemp,"%d",iValue);
	return WriteString(strElemName, strAttribute, szTemp);
}

bool CProfile::GetElementByName(TiXmlElement* pRoot, string strElemName, TiXmlElement*& xmlElement)
{
	
	const char* pszElemName = strElemName.c_str();
	if (memcmp(pRoot->Value(), pszElemName, strElemName.size()) == 0)
	{
		xmlElement = pRoot;
		return true;
	}
	TiXmlElement* pXmlElem = pRoot;
	if (strElemName.find('\\') != -1)
	{
		int nStart = 0;
		string strElem;
		char* pElem = NULL;
		while ((nStart = strElemName.find('\\')) != string::npos )
		{
			strElem = strElemName.substr(0, nStart);
			strElemName = strElemName.substr(nStart + 1, strElemName.size() - nStart - 1);
			TiXmlNode* pTempXmlNode;
			if (!(pTempXmlNode = pXmlElem->FirstChild(strElem.c_str())))
				break;
			pXmlElem = pTempXmlNode->ToElement();
		}
		TiXmlNode* pTempXmlNode;
		if ((pTempXmlNode = pXmlElem->FirstChild(strElemName.c_str())))
		{
			xmlElement = pTempXmlNode->ToElement();
			return true;
		}
	}
	return false;
}


CProfile::CTextFile::CTextFile()
{
	m_pFileContain = NULL;
	m_iCurrentPosition = 0;
}

CProfile::CTextFile::~CTextFile()
{
	if(m_pFileContain){
		delete m_pFileContain;
		m_pFileContain = NULL;
		m_iCurrentPosition = 0;
	}
}




/*-----------------------------------------------------------------------------
  功    能：从缓冲区中读取一行文本
  参数说明：无
  返 回 值：读到的文件串
  说    明：无
-----------------------------------------------------------------------------*/
string CProfile::CTextFile::GetLine()
{
	string strTemp = "";
	if(m_pFileContain != NULL){
		while ((*(m_pFileContain+m_iCurrentPosition) != 0)
			&&(*(m_pFileContain+m_iCurrentPosition) != '\r')
			&&(*(m_pFileContain+m_iCurrentPosition) != '\n'))
		{
			strTemp += *(m_pFileContain+m_iCurrentPosition++);
		}
		for(int i = 0; i<2; i++){
			if ((*(m_pFileContain+m_iCurrentPosition) == '\r')
				   ||(*(m_pFileContain+m_iCurrentPosition) == '\n'))
			{
				m_iCurrentPosition++;
			}
		}
	}
	return strTemp;
}


/*-----------------------------------------------------------------------------
  功    能：取得当前文本位置指针
  参数说明：无
  返 回 值：文本指针
  说    明：无
-----------------------------------------------------------------------------*/
char * CProfile::CTextFile::GetCurrentPointer()
{
	return m_pFileContain?(m_pFileContain+m_iCurrentPosition):NULL;
}


/*-----------------------------------------------------------------------------
  功    能：判断文本串是不为“段”行
  参数说明：string strText 文本串
  返 回 值：是“段”行--true, 否则--false
  说    明：无
-----------------------------------------------------------------------------*/
bool CProfile::CTextFile::IsSectionLine(string strText)
{
	int iLeft = strText.find("[");
	int iRight = strText.find("]");
	int iDemo = strText.find("//");
	if ((iRight > iLeft) && ( iLeft >= 0) && (iDemo <0 || iDemo > iRight))
		return true;
	return false;
}


/*-----------------------------------------------------------------------------
  功    能：判断“段”行的段名是否为给定段名
  参数说明：string strSrcText 段行文本串, string strSectionName 段名文本串
  返 回 值：是给定段名--true, 否则--false
  说    明：无
-----------------------------------------------------------------------------*/
bool CProfile::CTextFile::SectionNameIs(string strSrcText, string strSectionName)
{
	int iStrPosition = strSrcText.find(strSectionName);
	int iLeft = strSrcText.find("[");
	int iRight = strSrcText.find("]");
	int iDemo = strSrcText.find("//");
	if ( (iRight > iStrPosition) 
		  && (iStrPosition > iLeft) 
		  && ( iLeft >= 0) 
		  && (iDemo <0 || iDemo > iRight))
	{
		return true;
	}
	return false;
}


/*-----------------------------------------------------------------------------
  功    能：判断文本串是不为“键值”行
  参数说明：string strText 文本串
  返 回 值：是键值行--true, 否则--false
  说    明：无
-----------------------------------------------------------------------------*/
bool CProfile::CTextFile::IsKeyLine(string strText)
{
	int iEqual = strText.find("=");
	int iDemo = strText.find("//");
	if ((iEqual >= 0) && (iDemo <0 || iDemo > iEqual))
		return true;
	return false;
}


/*-----------------------------------------------------------------------------
  功    能：判断“键值”行是否为给定键值
  参数说明：string strSrcText 键值行文本串, string strSectionName 键值文本串
  返 回 值：是给定键值行--true, 否则--false
  说    明：无
-----------------------------------------------------------------------------*/
bool CProfile::CTextFile::KeyNameIs(string strSrcText, string strKeyName)
{
	int iEqual = strSrcText.find("=");
	int iDemo = strSrcText.find("//");
	int iStrPosition = strSrcText.find(strKeyName);

	if ((iStrPosition >= 0) 
		 && ( iEqual > iStrPosition)
		 && (iDemo <0 || iDemo > iEqual))
	{
		return true;
	}
	return false;
}


/*-----------------------------------------------------------------------------
  功    能：判断是否到文件尾
  参数说明：无
  返 回 值：未到文件尾--false 到文件尾--true
  说    明：无
-----------------------------------------------------------------------------*/
bool CProfile::CTextFile::IsEof()
{
	if(m_pFileContain != NULL)
		if( *(m_pFileContain+m_iCurrentPosition) != 0)
			return false;
	return true;
}


/*-----------------------------------------------------------------------------
  功    能：读入配置文件到缓冲区
  参数说明：string strFileName 配置文件名
  返 回 值：成功--false 失败--true
  说    明：无
-----------------------------------------------------------------------------*/
bool CProfile::CTextFile::LoadFile(string strFileName)
{
	bool bRet = true;
	FILE *fp;

	fp = fopen(strFileName.c_str(),"r");

	if(fp==NULL)
	{
		//打开失败，重试一次
		adsSleep(20);
		fp = fopen(strFileName.c_str(),"r");
	}

	if(fp!= NULL)
	{
		fseek (fp, 0, SEEK_END);
		int iFileLength = ftell(fp);
		fseek (fp, 0, SEEK_SET);
		if(m_pFileContain)
			delete m_pFileContain;
		if((m_pFileContain = new char[iFileLength+1]) != NULL){
//			int iTemp = fread(m_pFileContain, 1, iFileLength, fp);
//			m_pFileContain[iTemp] = 0;
			m_pFileContain[fread(m_pFileContain, 1, iFileLength, fp)] = 0;
			bRet = false;
		}
		fclose(fp);
	}
	return bRet;
}


/*-----------------------------------------------------------------------------
  功    能：取得键值
  参数说明：string strText 文本串
  返 回 值：键值
  说    明：无
-----------------------------------------------------------------------------*/
string CProfile::CTextFile::GetValueString(string strText)
{
	int iPos;
	string strResult = "";

	if ((iPos = strText.find("="))<0)
		return "";
	strText.erase(0,iPos+1);

	if ((iPos = strText.find("//")) >= 0)
		strText.erase(iPos);

	while ((strText.length()>0) && isspace(strText[0]))
		strText.erase(0,1);
	for (iPos = 0; iPos<strText.length(); iPos++){
		//if(isspace(strText[iPos]))
		if(strText[iPos] == 0x20)
			strText.erase(iPos);
	}

	return strText;
}


/*-----------------------------------------------------------------------------
  功    能：设置键值
  参数说明：string strSrc 原始串, string strValuestring 键值文本串
  返 回 值：设置后的文本
  说    明：无
-----------------------------------------------------------------------------*/
string CProfile::CTextFile::SetValueString(string strSrc, string strValue)
{
	if (!IsKeyLine(strSrc))
		return strSrc;
	string strOldValue = GetValueString(strSrc);
	if (strOldValue.empty()) {
		strSrc.insert(strSrc.find("=")+1," "+strValue);
	}
	else
		strSrc.replace(strSrc.find(strOldValue), strOldValue.length(), strValue);

	return strSrc;
}
