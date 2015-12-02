// ExtDataStream.h: interface for the CExtDataStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTDATASTREAM_H__DA2E3C2E_84BD_425E_B62B_4488DA2064E3__INCLUDED_)
#define AFX_EXTDATASTREAM_H__DA2E3C2E_84BD_425E_B62B_4488DA2064E3__INCLUDED_

#include "adsStd.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdDataStream.h"

typedef struct
{
	CBinary binGroupID;
	string	strGroupName;
	vector<CBinary> listDataStreamID;
}DATASTREAM_GROUP_ITEM;

class CExtDataStream : public CStdDataStream  
{
public:
	short ShowDataStream(unsigned short nFileID);

	short ShowDataStream(void);
	short AddGroupID(CBinary binGroupID);
	CExtDataStream();
	virtual ~CExtDataStream();

	vector<DATASTREAM_GROUP_ITEM> m_listDataStreamGroup;
	string ExceptionArithmetic (CBinary idDataStream, CReceiveFrame*pReceiveFrame);

};

#endif // !defined(AFX_EXTDATASTREAM_H__DA2E3C2E_84BD_425E_B62B_4488DA2064E3__INCLUDED_)
