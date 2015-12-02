/***************************************************************************//**
\file		DataCommon.h
\brief		declaration of common definitions/utilities, data structures
			, debug/log output mechanism and so on
\author		james.zhang@spx.com
*******************************************************************************/
#ifndef DATACOMMON_H
#define DATACOMMON_H

/*
 *	since debug environment hasn't been constructed for AutoBoss V3X, we use 
 *	this macro for our debugging purpose.
 */
#if !defined(SGL_DEBUG) & defined(_DEBUG)
#define SGL_DEBUG
#else
//#define SGL_DEBUG	// we can open this for CE-debugging temporarily	JZ
#endif	// #ifndef SGL_DEBUG

// tunable
#define	MX_PTH						512			// 1024
#define MX_RES						2048		// 4096

// constant
#define SEC							1000		// 1000 milliseconds of 1 sec
#define BITSINBYTE					8
#define BYTE_CAPACITY				256

#define	SZ_EMPTY					""
#define SZ_COMMA					","

// use a global variable to implement our Set/Get-Last-Error temporarily 
extern unsigned long g_dwSglLastErrNo;
#define SglSetLastErr(err_no)		(g_dwSglLastErrNo=(err_no))	// SetLastError
#define SglGetLastErr()				g_dwSglLastErrNo			// GetLastError

// special Last-Error no.
enum {
	emSleNothingToSet = 0
	,emSleUseWindowsLastError
};
#define WINDOWS_LAST_ERROR		emSleUseWindowsLastError
/*
 *	Function: SglErrTrace()
 *	
 *	Purpose:
 *	in debug mode, report the error message and set the last error no.
 *	in release mode, it only set the last error no.
 *	
 *	Input:
 *	const char* sErrMsg				
 *		- the error message to be reported
 *	unsigned long dwSglSetLastErrNo		
 *		- leave it alone to set nothing
 *		- or set it to WINDOWS_LAST_ERROR to use Windows' GetLastError() no.
 *		- or set it to one of the our error no. defined in SpxGasLibError.h
 */
void SglErrTrace(const char* sErrMsg=NULL
			   ,unsigned long dwSglSetLastErrNo=emSleNothingToSet);

 //void DdLog(const char* sFmt,...);

#ifdef SGL_DEBUG

extern string g_sDbg;
extern char g_pchLine[];
extern int g_nPos;

#define MAX_FN_LOG_LEN		15	// max filename logging length

// this macro is used to provide more debugging info. in debug mode
#define SglTrace(err_msg,err_no) do {										\
/* add filename distilled from filepath, truncate to same length(15) */		\
g_sDbg = __FILE__;															\
g_nPos = g_sDbg.find_last_of('\\');											\
if(g_nPos!=string::npos) g_sDbg.erase(0,g_nPos+1);							\
if(g_sDbg.length()>MAX_FN_LOG_LEN) g_sDbg.erase(MAX_FN_LOG_LEN);			\
else if(g_sDbg.length()<MAX_FN_LOG_LEN)										\
	g_sDbg.append(MAX_FN_LOG_LEN-g_sDbg.length(),' ');						\
/* add line of file */														\
sprintf(g_pchLine," %04d ",__LINE__);										\
g_sDbg += g_pchLine;														\
/* add error message, then report */										\
g_sDbg += (err_msg);														\
SglErrTrace(g_sDbg.c_str(),err_no);											\
} while(0)

extern char g_pchDdLogFmt[MX_RES];

#define _D	g_pchDdLogFmt,
#define DdLog(fmt_)	sprintf fmt_; SglTrace(g_pchDdLogFmt,0)

#else	// #ifdef SGL_DEBUG

#pragma warning(disable:4390)

/** should do nothing in release version for performance reason */
#define SglTrace(err_msg,err_no)	/* SglErrTrace(err_msg,err_no) */
#define SglTraceBuffer(t,b,l)

#define DdLog(fmt_) 

#endif	// #ifdef SGL_DEBUG

// following macros are most like Windows' TRACE / VERIFY
#define EMPTY_PLACEHOLDER
#define _AuxIfF(sth_to_be_verified,err_no,ret_sentence) do {				\
	if(!(sth_to_be_verified)) { SglTrace("!("#sth_to_be_verified ")"		\
	,(err_no)); ret_sentence; }												\
} while(0)

#define AuxIfFCont(sth_to_be_verified)										\
	if(!(sth_to_be_verified)) { SglTrace("!("#sth_to_be_verified			\
	")",NULL); continue; }

#define AuxIfFBrk(sth_to_be_verified)										\
	if(!(sth_to_be_verified)) { SglTrace("!("#sth_to_be_verified			\
	")",NULL); break; }


#define AuxIfFTrace1(sth,err_no)	_AuxIfF(sth,err_no,EMPTY_PLACEHOLDER)
#define AuxIfFTrace0(sth)			AuxIfFTrace1(sth,NULL)

#define AuxIfFRetF1(sth,err_no)		_AuxIfF(sth,err_no,return false)
#define AuxIfFRetF0(sth)			AuxIfFRetF1(sth,NULL)

#define AuxRetBool(sth)				do {AuxIfFRetF0(sth);return true;} while(0)			

#define AuxIfFRet(sth,err_no)		_AuxIfF(sth,err_no,return)
#define AuxIfFRetNeg1(sth,err_no)		_AuxIfF(sth,err_no,return -1)

// safe release macros:
#define _AuxSafeRel(sth_to_be_released,release_action) do {					\
	if((sth_to_be_released)!=NULL) { release_action;						\
	(sth_to_be_released)=NULL; }											\
}while(0)
#define ReleaseHandle(h)	_AuxSafeRel(h,AuxIfFTrace1(CloseHandle(h)		\
											,WINDOWS_LAST_ERROR))
#define ReleaseBuf(p)		_AuxSafeRel(p,delete [] (p))
#define ReleasePointer(p)	_AuxSafeRel(p,delete (p))
#define ReleaseFile(f)		_AuxSafeRel(f,fclose(f))

#define ZeroStruct(st)  memset(&(st), 0, sizeof(st))

// "AuxIf*edRet*" macros check if an object(instance) is initialized/unitialized
// (that is, ready for using), and then return something correspondingly. 
#define AuxIfInitedRetT(c) do {												\
	if(m_bInitialized) { SglTrace(#c " already initialized",NULL);			\
	return true; }															\
} while(0)

#define _AuxIfUninited(object_class,ret_sentence) do {						\
	if(!m_bInitialized) { SglTrace(#object_class							\
	" not initialized yet",SGL_ITN_NOT_INITED); ret_sentence; }				\
} while(0)
#define AuxIfUninitedRetF(c)		_AuxIfUninited(c,return false)
#define AuxIfUninitedRet0(c)		_AuxIfUninited(c,return)
#define AuxIfUninitedRet1(c,ret)	_AuxIfUninited(c,return (ret))

// "AuxAlloc" macros safely allocate an object
#define AuxAlloc(p_object,object_class) do {								\
	p_object = new object_class();											\
	AuxIfFRetF1(p_object,SGL_ERR_NOT_ENOUGH_MEMORY);						\
} while(0)

// "AuxAllocInit*" macros safely allocate and initialize an object
#define _AuxAllocInit(p_object,object_class,init_func) do {					\
	p_object = new object_class();											\
	AuxIfFRetF1(p_object,SGL_ERR_NOT_ENOUGH_MEMORY);						\
	AuxIfFRetF0(p_object->init_func);										\
} while(0)
#define AuxAllocInit0(p,cls)		_AuxAllocInit(p,cls,Initialize())
#define AuxAllocInit1(p,cls,p1)		_AuxAllocInit(p,cls,Initialize(p1))
#define AuxAllocInit2(p,cls,p1,p2)	_AuxAllocInit(p,cls,Initialize(p1,p2))

#if !defined(UNICODE) & defined(_DEBUG) & 0
// on PC debug
#include <process.h>    /* _beginthread, _endthread */

// use _beginthreadex/_endthreadex instead of CreateThread/ExitThread, but I
// can't find those functions for CE CRT library. Maybe CE's CreateThread/
// ExitThread are good enough.
typedef unsigned (__stdcall *PTHREAD_START) (void *);
#define AuxBgnThrdEx(psa, cbStackSize, pfnStartAddr,	\
	pvParam, dwCreateFlags, pdwThreadId)				\
	((HANDLE)_beginthreadex(							\
	(void *)        (psa),								\
	(unsigned)      (cbStackSize),						\
	(PTHREAD_START) (pfnStartAddr),						\
	(void *)        (pvParam),							\
	(unsigned)      (dwCreateFlags),					\
(unsigned *)    (pdwThreadId)))

#define AuxEndThrdEx		_endthreadex

#else	// #if !defined(UNICODE) && defined(_DEBUG)

#define AuxBgnThrdEx		CreateThread

#define AuxEndThrdEx		ExitThread

#endif	// #if !defined(UNICODE) && defined(_DEBUG)

////////////////////////////////////////////////////////////////////////////////
// Following are Pass-Through Log wrapper
////////////////////////////////////////////////////////////////////////////////

#define PTL_SET_LV_NO_LOG		254

// logging level: the higher, the more critical
#define PTL_CRITICAL	5
#define PTL_IMPORTANT	4
#define PTL_NORMAL		3
#define PTL_UNIMPORTANT	2
#define PTL_VERBOSE		1
#define PTL_SUPPRESSED	0

// !!! adjust this macro to (un)suppress log messages according to log level
#define PTL_CURRENT_LEVEL	PTL_NORMAL	// PTL_NORMAL	PTL_SET_LV_NO_LOG		

#define _PTLog(err_msg) SglTrace(err_msg,0)

#define _PTOnLv(do_sth,lv) do {if((lv)+1>PTL_CURRENT_LEVEL) do_sth;} while(0) 

// the standard log macro
#define PTLogEx(err_msg,lv)		_PTOnLv(_PTLog(err_msg),lv)
#define PTLog(err_msg)			PTLogEx(err_msg,PTL_NORMAL)	//lint !e506 !e774	


int Lpstr2Lpwstr(const char* lpcStr, OUT wchar_t* lpwStr);
int Lpwstr2Lpstr(const wchar_t* lpcwStr, char* lpStr);

#pragma warning(disable:4786)
#include <string>
#ifdef WIN32
#include <xstring>
#endif
using namespace std;
void Wstr2Str(wstring& wstr,string& str);
string& StrReplaceAll(string& str,const char* lpszOld,const char* lpszNew); 
string& StrReplaceAllDistinct(string& str,const char* lpszOld,const char* lpszNew);  
unsigned long StrKeywordAppearCnts(string& str,const char* lpszKeyword);
bool CHARS2HEX(unsigned char* out,const char* in,int len);


/***************************************************************************//**
\brief		extract	dword list from a "keyword" separate string
\param[in]	str					input string to be extracted
\param[in]	delimiter			keyword string used as delimiter in "str"
\param[out]	vDwords				dword list
\return		int		number of unsigned long extracted (that is, size of vDwords)
\warning	here is piece of unit test codes (in another MFC test program), FYI:
// 	vector<unsigned long> v_dwords;
// 	StrSplit2Dws(string("0x00000008,12,0x000000012,0x00000016")
// 		,",",v_dwords);
// 	CString s_rpt,s_sct;
// 	for(vector<unsigned long>::iterator it=v_dwords.begin();it!=v_dwords.end();it++)
// 	{
// 		s_sct.Format("%d ",*it);
// 		s_rpt = s_rpt + s_sct;
// 	}
// 	AfxMessageBox(s_rpt,MB_ICONINFORMATION);

Codes Output is:
	8 12 18 22
*******************************************************************************/
int StrSplit2Dws(string& str,const char* delimiter,vector<unsigned long>& vDwords);

/***************************************************************************//**
\brief		find out how many "lpszKeyword"(s) existed in "str", also tailor
			the "str" to ensure that the counts of "lpszKeyword"(s) existed in 
			"str" is not more than "dwMaxKwAppearCnts".
\param[in,out]	str					input string to be checked and tailored
\param[in]		lpszKeyword			keyword string to be found in "str"
\param[in]		dwMaxKwAppearCnts	upper limit of counts of "lpszKeyword"(s)
\return			unsigned long	actual counts of "lpszKeyword"(s) existed in "str"
\warning	
*******************************************************************************/
unsigned long StrKeywordAppearCntsClipped(string& str,const char* lpszKeyword
								  ,unsigned long dwMaxKwAppearCnts);
#endif	// #ifndef DATACOMMON_H


