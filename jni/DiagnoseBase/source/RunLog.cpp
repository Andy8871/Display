// RunLog.cpp: implementation of the CRunLog class.
//
//////////////////////////////////////////////////////////////////////
 
#include "RunLog.h"
#include "RunEnvironment.h"
#include "Profile.h"
#include <string.h>
#include <time.h>
#include <locale.h>

#ifdef WIN32
	#include "Display.h"
	extern CDisplay * g_pDisplay;
#else
	#include <dirent.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/types.h>
	#include <sys/stat.h>
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define LOG_TIME 0


CRunLog::CRunLog()
{
	m_PFile = NULL;
	m_isValidInGroupINI = true;

#ifdef _DEBUG
	m_EncryptFlag = false;
#else
	m_EncryptFlag = true;
#endif

	m_EncryptFile = NULL;
	m_strContent = "";
	m_bufSize = 0;
}

CRunLog::~CRunLog()
{
/*////Begin: by deleted for Logging Function Optimization 20120712
	if (m_strContent.length() > 0)
	{
		//关闭文件前写入缓存中的数据
		WriteContent("",true);
	}
	if (m_PFile)
	{
		fclose(m_PFile);
	}
	if (m_EncryptFile)
	{
		m_EncryptFile->Close();//by for debug
		delete m_EncryptFile;
	}
*/
}

void CRunLog::Close()//by debug
{
	if (m_strContent.length() > 0)
	{
		//关闭文件前写入缓存中的数据
		WriteContent("",true);
	}
	if (m_PFile)
	{
		fclose(m_PFile);
		//delete m_PFile;//by deleted for "fclose did it"
		m_PFile = NULL;
		
	}

	if (m_EncryptFile)
	{
		m_EncryptFile->Close();//by for debug
		delete m_EncryptFile;
		m_EncryptFile = NULL;
	}
}

char* CRunLog::Unicode2ANSI(wchar_t* pBuff)
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
#ifdef WIN32
	iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		pBuff,
		-1,
		NULL,
		0,
		NULL,
		NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
	::WideCharToMultiByte(CP_ACP,
		0,
		pBuff,
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL );
#endif
	return pElementText;
}

bool CRunLog::SetLogByGrpIni(void)
{
	string filename=CRunEnvironment::GetDiagnosisDirectory();
//  	int idx=filename.rfind('\\');
// 	filename=filename.substr(0,idx);
// 	filename += "\\MAKER.ini";
//	int iLogFlag = CProfile::GetInteger(filename,"BRAND","LogFlag",0);
	int iLogFlag = CProfile::GetInteger("COMMON\\LOG","FLAG",0);
	
	if (!iLogFlag)
		WriteContent("WARNING: Logging is not supported",true);

	m_isValidInGroupINI = iLogFlag;
	return m_isValidInGroupINI;
}
bool CRunLog::GetLogPermit(void)
{
 	return m_isValidInGroupINI;
}

bool CRunLog::Init(char* fileName)
{
	if (!CRunEnvironment::GetCheckMode())
	{
		return false;//hpy: avoid logging
	}
	/////如果当前已经是DEMO功能状态
	if(CRunEnvironment::GetDemoMode()==true)
		return true;
	string name = "";
	if (fileName == NULL)
	{
		string diagnoseDir = CRunEnvironment::GetDiagnosisDirectory();
		string strFilePath = diagnoseDir + "\\" + CRunEnvironment::GetLanguage() + ".txt";
		FILE* pFile = NULL;
		pFile = fopen(strFilePath.c_str(), "rb");
		string strVer = "";
		if (NULL != pFile)
		{
			fseek(pFile, 0, SEEK_END);
			int nLen = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);
			wchar_t *szBuff = new wchar_t[nLen / 2];
			fread(szBuff, 1, nLen, pFile);
			string strBuffer = Unicode2ANSI(szBuff);
			int nPos = strBuffer.find('V');
			if (nPos == string::npos)
				nPos = strBuffer.find('v');
			int nEnd = strBuffer.find(0x0D, nPos);
			if (nEnd != string::npos && nEnd > nPos)
				strVer = strBuffer.substr(nPos, nEnd - nPos);		
		}
		int pos = diagnoseDir.rfind('\\');
		if (pos != string::npos)
		{
			pos = diagnoseDir.rfind('\\',pos-1);
		}
		if (pos != string::npos)
		{
			name = diagnoseDir.substr(pos+1);
			pos = name.find('\\');
			if (pos != string::npos)
			{
				name.replace(pos,1,"_");
			}
		}
		name += "_" + strVer/*CRunEnvironment::GetLanguage()*/ + "_";
		//SYSTEMTIME systime;
		//GetLocalTime(&systime);
		time_t t = time(0); 
		char tmp[64];       
        strftime(tmp,sizeof(tmp),"%Y%m%d_%H%M%S",localtime(&t)); 
		name += tmp;
		name += ".dat"; 
	}
	else
	{
		name = fileName;
	}

	string ldir = CRunEnvironment::GetDisplayDirectory()+"Log";
#ifdef WIN32
	CreateDirectory(ldir.c_str(),NULL);
#else
    mkdir(ldir.c_str(), 0755);
#endif
	string file = ldir +"\\"+ name;
	SetBufSize();

	if (m_EncryptFlag)
	{
		m_EncryptFile = new CEncryptFile;
		if (m_EncryptFile == NULL)
		{
			return false;
		}
		if (!m_EncryptFile->Open(file.c_str(),"wb"))
		{
			return false;
		}
	}else
	{
		m_PFile = fopen(file.c_str(),"wb");
		if (m_PFile == NULL)
		{
			return false;
		}
	}
	SetLogByGrpIni();

	m_strContent += "-----------------LibVersion:20140113-----------------\r\n\r\n";//by add for log Lib version 20120629

	return true;
}
bool CRunLog::WriteHeader(const char* content)
{
	string  division = "-------------------------------------------------------------";
	string writeStr = content;

	if (m_EncryptFlag)
	{
		if (m_EncryptFile)
		{
			division += "\r\n";
			//画横线
			m_strContent += division;
			if (content != NULL)
			{
				writeStr += "\r\n\r\n";
				m_strContent += writeStr;
			}
		}else
			return false;
	}
	else
	{
		if (m_PFile)
		{
			division += "\n";
			//画横线
			m_strContent += division;
			if (content != NULL)
			{
				writeStr += "\n\n";
				m_strContent += writeStr;
			}
		}else
			return false;
	}
	return WriteContent("",true);
}
bool CRunLog::WriteContent(const string& data,bool writeNow/* = false*/)
{
	return WriteContent(data.c_str(),writeNow);
}
bool CRunLog::WriteMenu(void)
{
	string temp = "Menu: ";
	temp += CRunEnvironment::GetMenuChoosedDir();
	temp += "\r\n";
	return this->WriteContent(temp);
}
bool CRunLog::WriteContent(CBinary& data,bool writeNow /*= false*/)
{
	if (data.GetSize() == 0)
		return false;
	int size = data.GetSize();
	char buf[20];
	string strData = "";
	for (int i = 0; i < size; i++)
	{
		sprintf(buf,"%02x",data.GetAt(i));
		strData += "\\x";
		strData += buf;
	}

	return WriteContent(strData.c_str(),writeNow);
}

bool CRunLog::WriteContent(const char* content,bool writeNow/* = false*/)
{	
	if (content == NULL)
		return false;
	/////如果当前是DEMO功能状态
	if(CRunEnvironment::GetDemoMode()==true)
		return true;
	string writeStr = content;

	if (m_EncryptFlag)
	{
		if (m_EncryptFile)
		{
			writeStr += "\r\n";
		}else
			return false;
	}
	else 
	{
		if (m_PFile)
		{
			writeStr += "\n";
		}else
			return false;
	}

	if ((m_strContent.length()+writeStr.length()) > m_bufSize)
	{
		
		//写入文件
		if (m_EncryptFlag)
		{
			if (m_EncryptFile)
			{
				m_EncryptFile->Write(m_strContent.c_str(),m_strContent.length());
			}
			else
				return false;
		}
		else 
		{
			if (m_PFile)
			{
				fwrite(m_strContent.c_str(),sizeof(char),m_strContent.length(),m_PFile);
				fflush(m_PFile);
			}
			else
				return false;
		}
		//重新赋值
		m_strContent = writeStr;
	}
	else
	{
		m_strContent += writeStr;
		if (writeNow == true)
		{
			//写入文件
			if (m_strContent.length() > 0)
			{
				
				if (m_EncryptFlag)
				{
					if (m_EncryptFile)
					{
						if (!m_EncryptFile->Write(m_strContent.c_str(),m_strContent.length()))
						{
#ifdef WIN32
							g_pDisplay->MessageBox("Write Error! string = %s",m_strContent.c_str());
#endif
						}
					}
					else
						return false;
				}
				else
				{
					if (m_PFile)
					{
						int iLen = fwrite(m_strContent.c_str(),sizeof(char),m_strContent.length(),m_PFile);
						fflush(m_PFile);
					}
					else
						return false;
				}	
				m_strContent = "";
			} 
		}
	}	
	return true;
}
void CRunLog::WriteTimeTest(void)
{
#if LOG_TIME
	WriteTime(8);
	WriteTime(512);
	WriteTime(1024);
	WriteTime(BUF_SIZE);
#endif
}

void CRunLog::WriteTime(int bufSize)
{
	//char tmp[64] = "";
    //unsigned long   start;
	unsigned long   time_i=0;   
	//start = GetTickCount();
	char buf[BUF_SIZE] = {0};
	
	if (m_EncryptFlag)
	{
		if (m_EncryptFile)
		{
			m_EncryptFile->Write(buf,bufSize);
		}
	}
	else 
	{
		if (m_PFile)
		{
			fwrite(buf,sizeof(char),bufSize,m_PFile);
		}
	}
	fflush(m_PFile);
	//time_i = GetTickCount() - start;
	sprintf(buf,"\r\nWriter Buffer[%d] cost %lu ms\r\n",bufSize,time_i);
	string endTime = "";
	endTime += buf;

	if (m_EncryptFlag)
	{
		if (m_EncryptFile)
		{
			m_EncryptFile->Write(endTime.c_str(),endTime.length());
		}
	}
	else
	{
		if (m_PFile)
		{
			fwrite(endTime.c_str(),sizeof(char),endTime.length(),m_PFile);
		}
	}
	fflush(m_PFile);
}
void CRunLog::SetBufSize(unsigned long bufSize/* = 2048*/)
{
	m_bufSize = bufSize;
	m_strContent.reserve(m_bufSize);
}


