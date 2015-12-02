/******************************************************************************

  Copyright (c) 2006, AUTOBOSS Ltd,.Co 
  All rights reserved.
 
  �ļ����ƣ�Profile.c
  �ĵ���ʶ��007�������ƽ̨��ϸ���˵����--��ʾ����ֲ� V1.0
  ժ    Ҫ�����ڶ�д�����ļ������ܰ��������ַ�����д�ַ��������������ݡ�д����
            ���ݡ�

  ��ʷ��¼��
  ----------------------------------------------------------------------------
  ʱ     ��   ����   �汾��	  ��    ��    ��    ��                               
  ----------------------------------------------------------------------------
  2006.01.21  wcx    1.0AX     ����                                           

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
  ��    �ܣ���ȡ���ô�
  ����˵����string strFileName �����ļ���,
            string strSectionName ����, 
            string strKeyName ��ֵ,
            string strDefaultString ȱʡ��
  �� �� ֵ��������ַ���
  ˵    ������
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
  ��    �ܣ�д�����ô�
  ����˵����string strFileName �����ļ���,
            string strSectionName ����,
            string strKeyName ��ֵ,
            string strString д�봮
  �� �� ֵ���ɹ�--false ʧ��--true
  ˵    ������
-----------------------------------------------------------------------------*/
bool CProfile::WriteString (string strFileName, string strSectionName, string strKeyName, string strValue, bool bCaseSensitive)
{
	CTextFile TextFile;
	TextFile.LoadFile(strFileName);

	FILE *fp;
	if ((fp = fopen(strFileName.c_str(),"w+")) == NULL)
	{
		//����һ��
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
  ��    �ܣ���ȡ������ֵ
  ����˵����string strFileName �����ļ���,
            string strSectionName ����,
            string strKeyName ��ֵ,
            int iDefaultValue ȱʡֵ
  �� �� ֵ���������ֵ
  ˵    ������
-----------------------------------------------------------------------------*/
int CProfile::GetInteger (string strFileName, string strSectionName, string strKeyName, int intDefaultValue, bool bCaseSensitive) 
{
	char szTemp[50];

	sprintf(szTemp,"%d",intDefaultValue);
	string strTemp = GetString (strFileName, strSectionName, strKeyName, szTemp, bCaseSensitive);

	return atoi(strTemp.c_str());
}


/*-----------------------------------------------------------------------------
  ��    �ܣ���ȡ���ô�
  ����˵����string strFileName �����ļ���,
            string strSectionName ����,
            string strKeyName ��ֵ,
            int iValue д��ֵ
  �� �� ֵ���ɹ�--false ʧ��--true
  ˵    ������
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
  ��    �ܣ��ӻ������ж�ȡһ���ı�
  ����˵������
  �� �� ֵ���������ļ���
  ˵    ������
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
  ��    �ܣ�ȡ�õ�ǰ�ı�λ��ָ��
  ����˵������
  �� �� ֵ���ı�ָ��
  ˵    ������
-----------------------------------------------------------------------------*/
char * CProfile::CTextFile::GetCurrentPointer()
{
	return m_pFileContain?(m_pFileContain+m_iCurrentPosition):NULL;
}


/*-----------------------------------------------------------------------------
  ��    �ܣ��ж��ı����ǲ�Ϊ���Ρ���
  ����˵����string strText �ı���
  �� �� ֵ���ǡ��Ρ���--true, ����--false
  ˵    ������
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
  ��    �ܣ��жϡ��Ρ��еĶ����Ƿ�Ϊ��������
  ����˵����string strSrcText �����ı���, string strSectionName �����ı���
  �� �� ֵ���Ǹ�������--true, ����--false
  ˵    ������
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
  ��    �ܣ��ж��ı����ǲ�Ϊ����ֵ����
  ����˵����string strText �ı���
  �� �� ֵ���Ǽ�ֵ��--true, ����--false
  ˵    ������
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
  ��    �ܣ��жϡ���ֵ�����Ƿ�Ϊ������ֵ
  ����˵����string strSrcText ��ֵ���ı���, string strSectionName ��ֵ�ı���
  �� �� ֵ���Ǹ�����ֵ��--true, ����--false
  ˵    ������
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
  ��    �ܣ��ж��Ƿ��ļ�β
  ����˵������
  �� �� ֵ��δ���ļ�β--false ���ļ�β--true
  ˵    ������
-----------------------------------------------------------------------------*/
bool CProfile::CTextFile::IsEof()
{
	if(m_pFileContain != NULL)
		if( *(m_pFileContain+m_iCurrentPosition) != 0)
			return false;
	return true;
}


/*-----------------------------------------------------------------------------
  ��    �ܣ����������ļ���������
  ����˵����string strFileName �����ļ���
  �� �� ֵ���ɹ�--false ʧ��--true
  ˵    ������
-----------------------------------------------------------------------------*/
bool CProfile::CTextFile::LoadFile(string strFileName)
{
	bool bRet = true;
	FILE *fp;

	fp = fopen(strFileName.c_str(),"r");

	if(fp==NULL)
	{
		//��ʧ�ܣ�����һ��
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
  ��    �ܣ�ȡ�ü�ֵ
  ����˵����string strText �ı���
  �� �� ֵ����ֵ
  ˵    ������
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
  ��    �ܣ����ü�ֵ
  ����˵����string strSrc ԭʼ��, string strValuestring ��ֵ�ı���
  �� �� ֵ�����ú���ı�
  ˵    ������
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
