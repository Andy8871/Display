/***************************************************************************//**
\file		DataStreamCallback.cpp
\brief		definition of the CDataStreamCallback class.
\author		Brandon Gao
*******************************************************************************/

#include "stdafx.h"
#include "DataStreamCallback.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataStreamCallback::CDataStreamCallback()
{

}

CDataStreamCallback::CDataStreamCallback( CDataStreamCallback& value )
{
	m_cbMapping = value.m_cbMapping; 
}

CDataStreamCallback::~CDataStreamCallback()
{

}

CDataStreamCallback& CDataStreamCallback::operator= ( const CDataStreamCallback& value )
{
	if( m_cbMapping != value.m_cbMapping )
		m_cbMapping = value.m_cbMapping; 

	return *this;
}

CDataStreamCallback& CDataStreamCallback::operator= ( const map< string, DsFunction >& value )
{
	if( m_cbMapping != value )
		m_cbMapping = value; 

	return *this;
}

/***************************************************************************//**
\brief		set the value of callback function mapping
\param[in]	value		the value you want to give to m_cbMapping
\return		void
*******************************************************************************/
void CDataStreamCallback::SetCallbackMapping( map< string, DsFunction > value )
{
	if( m_cbMapping != value )
		m_cbMapping = value; 
}

/***************************************************************************//**
\brief		calculate data stream value by user-defined function through a
			function pointer
\param[in]	CmdIndex	index of data stream commands
			CmdArray	commands received from ECU
			Length		the length of the CmdArray
			FuncName	the function name you gonna call to calculate the value
\return		string		data stream value
*******************************************************************************/
string CDataStreamCallback::CalcDsByCallbackFunc( int CmdIndex, unsigned char* CmdArray, string FuncName )
{
	string result;

	DsFunction OwnDsFunc;
	OwnDsFunc = ( DsFunction ) m_cbMapping[FuncName];
	OwnDsFunc( CmdIndex, CmdArray, result );

	return result;
}
