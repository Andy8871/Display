/***************************************************************************//**
\file		ResAPI.h
\brief		declaration of the CResAPI class.
\author		James Zhang
*******************************************************************************/

#if !defined(AFX_RESAPI_H__74F435E7_674A_4809_917C_73C0A086DEAD__INCLUDED_)
#define AFX_RESAPI_H__74F435E7_674A_4809_917C_73C0A086DEAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "adsStd.h"
#define MX_PTH 512
/***************************************************************************//**
\brief	the whole index part of "Res.dat" are required to be sorted by "text_id"
		in file. By now, to get string by "text_id":
		- if sorting job already done, we can simply use explicit bisection 
		method (that is, binary search)
		- or else, we can use "built-in" multimap<stTextId,nIdx> in our program
*******************************************************************************/
#undef SORTED_BY_TEXT_ID_IN_FILE
#define SORTED_BY_TEXT_ID_IN_FILE

#define TEXT_ID_LEN		6
/***************************************************************************//**
\brief	this text_id is used to uniquely identify one piece of string resource
		within one vehicle module
*******************************************************************************/
typedef struct tpStTextId
{
	unsigned char	bt[TEXT_ID_LEN];

	tpStTextId() { memset(bt, 0, sizeof(bt)); }

#ifndef SORTED_BY_TEXT_ID_IN_FILE
	bool operator<(const tpStTextId &stTextId2) const
	{
		return (memcmp(this,&stTextId2,TEXT_ID_LEN)<0);
	}
#endif	//	#ifndef SORTED_BY_TEXT_ID_IN_FILE

} StTextId;

#ifndef SORTED_BY_TEXT_ID_IN_FILE
typedef multimap<StTextId,unsigned long>	MmpTxtId2Id;
#endif	//	#ifndef SORTED_BY_TEXT_ID_IN_FILE

class CDataDrvFile;

/***************************************************************************//**
\brief	directly access the data from the individual "resource" data-driven file
*******************************************************************************/
class CResAPI  
{
public:
	CResAPI();
	virtual ~CResAPI();

	bool Initialize(const char* lpszPath);	// re-initialization is valid 

	// id to string
	bool GetWstr(IN unsigned long dwId,OUT wstring& wstrRet);
	bool GetStr(IN unsigned long dwId,OUT string& strRet);

	// id to text_id
	bool GetTextId(IN unsigned long dwId,OUT StTextId& stTxtIdRet);

	// text_id to string
	bool GetWstr(IN StTextId& stTxtId,OUT wstring& wstrRet);
	bool GetStr(IN StTextId& stTxtId,OUT string& strRet);

private:
	void Clear();

private:
	CDataDrvFile* m_pDataDrvF;

	unsigned long m_dwIdCnt;

/***************************************************************************//**
\brief	[data-driven data structure] one piece of index part of "Res.dat"
\warning	sizeof(tpStStringTable) is 16 since packing alignment of the 
			structure is 4.
*******************************************************************************/
	typedef struct tpStStringTable
	{
		unsigned long		dwStringAddr;
		unsigned long		dwLen;
		StTextId	stTxtId;

		unsigned long		dwReservedData;
	} StStringTable,StrTbl,*PStrTbl;
	
	PStrTbl	m_StrTbls;

#ifndef SORTED_BY_TEXT_ID_IN_FILE
	MmpTxtId2Id m_mmpTxtId2Id;
#endif	//	#ifndef SORTED_BY_TEXT_ID_IN_FILE

	bool m_bInitialized;

	char	m_pchCurPath[MX_PTH];
};

#endif // !defined(AFX_RESAPI_H__74F435E7_674A_4809_917C_73C0A086DEAD__INCLUDED_)
