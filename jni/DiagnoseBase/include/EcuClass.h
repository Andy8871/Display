/***************************************************************************//**
\file		EcuClass.h
\brief		declaration of the CEcuClass class.
\author		James Zhang
*******************************************************************************/

#if !defined(AFX_ECUCLASS_H__A5C57881_7837_4FBA_A2EF_7DFBD6216363__INCLUDED_)
#define AFX_ECUCLASS_H__A5C57881_7837_4FBA_A2EF_7DFBD6216363__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "adsStd.h"

/***************************************************************************//**
\brief	manage the whole ECU diagnostic procedure
*******************************************************************************/
class CEcuClass  
{
public:
	CEcuClass();
	virtual ~CEcuClass();

/***************************************************************************//**
\brief	keep an default title string for later using
\param[in]	strDefaultTitle		default title string
*******************************************************************************/
	void SetDefaultTitle(IN string& strDefaultTitle);

/***************************************************************************//**
\brief			start the whole vehicle's diagnostic procedure 
\param[in]		strPtcList	PTC filename list e.g. "PTC01,PTC02"
\param[in]		dwFlag	special mode. You can combine options (macro: ECSD_F_*)
						listed below by using the bitwise-OR (|) operator. 
\return	bool	true if function succeeded, false otherwise
\warning		now error-message report logic has been included
*******************************************************************************/
#define ECSD_F_NOTHING			0x00	/* nothing special */
#define ECSD_F_NO_ERR_RPT		0x01	/* do not report error, keep silent */
#define ECSD_F_ENTER_EXIT_SYS	0x02	/* simply enter and exit system */
	bool StartDiag(IN string& strPtcList,unsigned long dwFlag=ECSD_F_NOTHING);
	bool StartDiag(const char* strPtcList,unsigned long dwFlag=ECSD_F_NOTHING);

private:
/***************************************************************************//**
\brief		
\param[in]	
\param[out]	
\return		bool	true if diagnosis procedure successfully done, false if 
					ECU(communication) error occurs -- need to prompt error
\warning	
*******************************************************************************/
	bool StartDiagInner(IN string& strPtcList,unsigned long dwFlag);

private:
	string			m_strDftTtl;
};

#endif // !defined(AFX_ECUCLASS_H__A5C57881_7837_4FBA_A2EF_7DFBD6216363__INCLUDED_)
