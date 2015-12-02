// NewVsIndicator.h: interface for the CNewVsIndicator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWVSINDICATOR_H__39C8E073_9300_47DD_8734_3985C44822F6__INCLUDED_)
#define AFX_NEWVSINDICATOR_H__39C8E073_9300_47DD_8734_3985C44822F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNewVsIndicator  
{
public:
	static bool IsNewVehicleSelection(void);
	static void DestoryNewVSIndicator(void);
	static void CreateNewVSIndicator(void);
	CNewVsIndicator();
	virtual ~CNewVsIndicator();

};

#endif // !defined(AFX_NEWVSINDICATOR_H__39C8E073_9300_47DD_8734_3985C44822F6__INCLUDED_)
