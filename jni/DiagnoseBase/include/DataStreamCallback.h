/***************************************************************************//**
\file		DataStreamCallback.h
\brief		declaration of the CDataStreamCallback class.
\author		Brandon Gao
*******************************************************************************/

#if !defined(AFX_DATASTREAMCALLBACK_H__33129B45_CF81_4176_8851_B840735481B2__INCLUDED_)
#define AFX_DATASTREAMCALLBACK_H__33129B45_CF81_4176_8851_B840735481B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable: 4786 )
#define DSFUNCPOINTER void*

#include "adsStd.h"
#include <map>
#include <string>

using namespace std;

typedef void (*DsFunction) ( int, unsigned char*, string& );

/***************************************************************************//**
\brief	handle complicated data stream algorithm
*******************************************************************************/
class CDataStreamCallback  
{
public:
	CDataStreamCallback();
	CDataStreamCallback( CDataStreamCallback& value );

	virtual ~CDataStreamCallback();

public:
	CDataStreamCallback& operator= ( const CDataStreamCallback& value );
	CDataStreamCallback& operator= ( const map< string, DsFunction >& value );

	void	SetCallbackMapping( map< string, DsFunction > value );
	string	CalcDsByCallbackFunc( int CmdIndex, unsigned char* CmdArray, string FuncName );	// 通过调用m_cbMapping中保存的函数指针来计算数据流

protected:
	map< string, DsFunction > m_cbMapping;
};

#endif // !defined(AFX_DATASTREAMCALLBACK_H__33129B45_CF81_4176_8851_B840735481B2__INCLUDED_)
