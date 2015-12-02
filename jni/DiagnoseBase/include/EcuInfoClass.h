/***************************************************************************//**
\file		EcuInfoClass.h
\brief		declaration of the CEcuInfoClass class.
\author		James Zhang
*******************************************************************************/
#if !defined(AFX_ECUINFOCLASS_H__4C03D18B_438D_43E3_AAA9_BCD930AF0EDC__INCLUDED_)
#define AFX_ECUINFOCLASS_H__4C03D18B_438D_43E3_AAA9_BCD930AF0EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "adsStd.h"

class CPtcAPI;
class CLinkLayer;

/***************************************************************************//**
\brief	manage the whole ECU information procedure of an ECU
*******************************************************************************/
class CEcuInfoClass  
{
public:
	CEcuInfoClass();
	virtual ~CEcuInfoClass();

/***************************************************************************//**
\brief		start the whole ECU information procedure of an ECU
\param[in]	pPtcAPI		to retrieve data from data-driven file
\param[in]	pLinkLayer	to use this do real send-receive work
\return		bool		true if function succeeded, false otherwise
\warning
*******************************************************************************/
	bool Start(IN CPtcAPI* pPtcAPI,IN CLinkLayer* pLinkLayer,IN const vector<unsigned long>& vShowStAddrs);
};

#endif // !defined(AFX_ECUINFOCLASS_H__4C03D18B_438D_43E3_AAA9_BCD930AF0EDC__INCLUDED_)
