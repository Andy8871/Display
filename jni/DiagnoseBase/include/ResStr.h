// ResStr.h: interface for the CResStr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESSTR_H__556A0B1E_6F95_4DF5_A451_23A7CB98C0EE__INCLUDED_)
#define AFX_RESSTR_H__556A0B1E_6F95_4DF5_A451_23A7CB98C0EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "adsStd.h"

/*
 *	James Zhang		2011-5-17 12:15:45
 *	Provide a utility macro AuxSysStr(ID_) to ensure that we can always show
 *	something even the system string is empty or ID_ is invalid.
 *
 *	Note: DO NOT use more than one AuxSysStr in one sentence, which looks like:
 *	
 *		adsDisplay.MessageBox(AuxSysStr(ID1),AuxSysStr(ID2)...);	// WRONG!
 *
 *	Correct one could be:
 *
 *		string s_id1 = AuxSysStr(ID1);
 *		string s_id2 = AuxSysStr(ID2);
 *		adsDisplay.MessageBox(s_id1,s_id2...);						// CORRECT!
 */
extern string _s;
#define _LDH	strlen("DATADRV_")	// length of the macro prefix of resource
#ifdef	_DEBUG 
#define _AMI	" (DBG prompt: try use latest SysResource_XX.dat)"
#else	//	#ifdef	_DEBUG
#define _AMI	""
#endif	//	#ifdef	_DEBUG
#define AuxSysStr(ID_)	(_s=adsResStr.GetStringFromID(ID_)\
,_s.empty()?(_s=#ID_,(_s.size()>_LDH?_s.erase(0,_LDH):_s),_s+=_AMI):_s,_s)


typedef struct tagSysString_t
{
	unsigned long dwStringID;
	wstring wsString;
}SysString_t;

class CResStr  
{
public:
	CResStr();
	virtual ~CResStr();
	bool Init(void);
	string GetStringFromID(unsigned long idVal);
	void Release(void){m_InitFlag = false;};
private:
	bool m_InitFlag;
	std::vector<SysString_t> m_veSysStringList;
};

#endif // !defined(AFX_RESSTR_H__556A0B1E_6F95_4DF5_A451_23A7CB98C0EE__INCLUDED_)
