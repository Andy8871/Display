/***************************************************************************//**
\file		Log.h
\brief		declaration of the CLog class.
\author		james.zhang@spx.com
*******************************************************************************/
#ifndef LOG_H
#define LOG_H

#include "adsStd.h"

#ifdef UNICODE
#define LOG_FILE_PREFIX		"\\Storage Card\\dd_log_"
#else	// #ifdef UNICODE
#define LOG_FILE_PREFIX		"dd_log_"
#endif	// #ifdef UNICODE

/***************************************************************************//**
\brief	(debug) information logging (singleton style object used)
*******************************************************************************/
class CLog  
{
public:
	virtual ~CLog();

/***************************************************************************//**
\brief		retrieve the only instance of this object (singleton style)
\return		the only instance of this object
*******************************************************************************/
	static CLog* GetNamedSingleton();

/***************************************************************************//**
\brief		add a new line of logging contents currently to a named file
\param[in]	sLog	logging contents
*******************************************************************************/
	void AddLog(const char* sLog);

private:

/***************************************************************************//**
\brief		private constructor (singleton style)
*******************************************************************************/
	CLog(const char* sLogFileNamePrefix);	/*lint !e1704 */

/***************************************************************************//**
\brief		close the former file and open the specified file for logging
\param[in]	sLogFileName	the specified file to be opened for logging, or
							set it to NULL to just closer the former file
*******************************************************************************/
	void SetLogFile(const char* sLogFileName);

private:
	FILE*		m_pf;
	tm*			m_pt;
	string		m_sLog;
};	/*lint !e1712 */

#endif // #ifndef LOG_H

