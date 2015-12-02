// StClass.h: interface for the CStClass class.
//
//////////////////////////////////////////////////////////////////////
/***************************************************************************//**
\file		StClass.h
\brief		declaration of the CStClass class.
\author		James Zhang
*******************************************************************************/
#if !defined(AFX_STCLASS_H__7D7284E8_C471_485E_9814_1DC1E5E27BE5__INCLUDED_)
#define AFX_STCLASS_H__7D7284E8_C471_485E_9814_1DC1E5E27BE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "adsStd.h"

class CPtcAPI;
class CLinkLayer;

#if defined(_WIN32) & defined(_DEBUG)
class CStMenuParse;
#endif	// #if defined(_WIN32) & defined(_DEBUG)


/***************************************************************************//**
\brief	manage the whole Functional Test | Special Test procedure of an ECU
*******************************************************************************/
class CStClass  
{
public:
	CStClass();
	virtual ~CStClass();

	bool Init(const unsigned long cdwStRootNodeAddr,const vector<unsigned long>& vDsAvailAddrs);

	bool Start(CPtcAPI* pPtcApi,IN CLinkLayer* pLinkLayer
		,const vector<unsigned long>& vShowStAddrs,OUT bool& bExitSys);

private:
	unsigned long m_dwStRootNodeAddr;
	vector<unsigned long> m_vDsAvailAddrs;

#if defined(_WIN32) & defined(_DEBUG)
private:
	void DbgLogMenuTree(CStMenuParse* pMP,int nIdx);
private:
	int m_nDbgMenuLv;
#endif	// #if defined(_WIN32) & defined(_DEBUG)

};

#endif // !defined(AFX_STCLASS_H__7D7284E8_C471_485E_9814_1DC1E5E27BE5__INCLUDED_)
