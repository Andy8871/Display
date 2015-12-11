// RunLog.h: interface for the CRunLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUNLOG_H__74DAE9C9_021F_4F85_BFEF_DC6A2B43E380__INCLUDED_)
#define AFX_RUNLOG_H__74DAE9C9_021F_4F85_BFEF_DC6A2B43E380__INCLUDED_
//
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
#include "Binary.h"
#include "../../Security/include/EncryptFile.h"
#ifdef WIN32
#include "PtcAPI.h"
#endif

class CRunLog  
{
public:
	//Operator，no need to init. All done in baselib.
	//all you have to do is adding the words you want to log.
	//All standard output format have been done in lib.
	bool WriteHeader(const char* content = NULL);
	bool WriteContent(const char* content = NULL,bool writeNow = true);
	bool WriteContent(CBinary& data,bool writeNow = true);
	bool WriteContent(const string& data,bool writeNow = true);
	bool WriteMenu(void);

	void SetBufSize(unsigned long bufSize = BUF_SIZE);
	void Close(void);//by added for Logging Function Optimization 20120712

	CRunLog();
	bool Init(char* fileName = NULL);
	virtual ~CRunLog();
	bool GetLogPermit(void);//{return m_isValidInGroupINI;};
	//Log Time Test
	void WriteTimeTest(void);

private:
	//根据Group.ini中的Logging_Supported确定是否启用Log功能
	bool SetLogByGrpIni(void);
	char* Unicode2ANSI(wchar_t* pBuff);
	enum
	{
		//BUF_SIZE = 2048
		BUF_SIZE = 512//by for debug
	};
	void WriteTime(int bufSize);

	FILE* m_PFile; //打开文件指针

	CEncryptFile* m_EncryptFile;
	bool m_EncryptFlag;
	string m_strContent;
	unsigned long m_bufSize;
	bool m_isValidInGroupINI;
};

#endif // !defined(AFX_RUNLOG_H__74DAE9C9_021F_4F85_BFEF_DC6A2B43E380__INCLUDED_)
