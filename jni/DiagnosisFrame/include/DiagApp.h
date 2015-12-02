#ifndef	__DIAGAPP_H_
#define	__DIAGAPP_H_

#include "AdsStd.h"

class CDiagApp
{
public: 
	CDiagApp(void);
	virtual ~CDiagApp(void);

	virtual short InitInstance () = 0;
	static CDiagApp *m_pMainApplication;
};


CDiagApp * adsGetDiagApp ();

void adsSleep(int iMilliseconds);

#endif	//__DIAGAPP_H_
