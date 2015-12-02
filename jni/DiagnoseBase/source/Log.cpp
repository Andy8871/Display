//******************************************************************************
//	Filename:	Log.cpp
//
//	Purpose:
//	(debug) information logging (singleton style object used)
//
//	Author:
//	james.zhang@spx.com
//******************************************************************************
#include "stdafx.h"
#include "Log.h"
#include "time.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog(const char* sLogFileNamePrefix)
{
	m_pf = NULL;
	// add time info to file name
	string s_filename = sLogFileNamePrefix;
	time_t tm = time(0);
	m_pt = localtime(&tm);
	static char pch_tail[] = "yyyymmdd.txt";
	sprintf(pch_tail,"%04d%02d%02d.txt",m_pt->tm_year,m_pt->tm_mon,m_pt->tm_mday);
	s_filename += pch_tail;

	SetLogFile(s_filename.c_str());
}


CLog::~CLog()
{
	// close log file if exists
	SetLogFile(NULL);	//lint !e1551
}	//lint !e1740

void CLog::AddLog(const char* sLog)
{
	if(sLog==NULL) 
	{
		return;
	}

	if(m_pf)
	{
		// add time info.
		timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		m_pt = localtime(&tv.tv_sec);
		static char pch_t[] = "hh:mm:ss:eee ";
		sprintf(pch_t,"%02d:%02d:%02d:%03d ",m_pt->tm_hour,m_pt->tm_min ,m_pt->tm_sec,
				tv.tv_usec);
		m_sLog = pch_t;

		m_sLog += sLog;

		// add carriage-return in case of need
		if(m_sLog.length()<2||m_sLog.substr(m_sLog.length()-2,2)!="\r\n")
		{
			m_sLog += "\r\n";
		}

		// write log to file
		fwrite(m_sLog.c_str(),sizeof(char),m_sLog.length(),m_pf);
		fflush(m_pf);
	}
}

CLog* CLog::GetNamedSingleton()
{
	static CLog theLog(LOG_FILE_PREFIX);

	return &theLog;
}

void CLog::SetLogFile(const char* sLogFileName)
{
	// close former log file if exists
	if(m_pf)
	{
		fclose(m_pf);
		m_pf = NULL;
	}

	// return if null name
	if(sLogFileName==NULL) 
	{
		return;
	}

	// open this file for logging	
	m_pf = fopen(sLogFileName,"a+b");
	if(!m_pf)
	{
		return;
	}
}
