#ifndef	__DIAGNOSISUNIT_H_
#define	__DIAGNOSISUNIT_H_

#include "adsStd.h"


class CDiagnosisUnit
{
public:
	CDiagnosisUnit(void);// { };// by modified for Logging Function Optimization 20120712
	virtual ~CDiagnosisUnit(void);// { };

public:
	enum {
		//	DUF_	Diagnosis Unit Function
		DUF_NORMAL_DIAGNOSIS        = 0,
		DUF_SCAN_SYSTEM             = 1,
		DUF_QUICK_READ_TROUBLE_CODE = 2,
		DUF_QUICK_CLEAR_TROUBLE_CODE = 3
	};

public: 
	virtual short Enter (unsigned short uiWay);

protected:
	virtual short NormalDiagnosis (void);
	virtual short ScanSystemExist (void);
	virtual short QuickReadTroubleCode (void) = 0;
	virtual short QuickClearTroubleCode (void) = 0;
	virtual short EnterSystem (void) = 0;
	virtual void ExitSystem (void) = 0;
	virtual void OnEnter (unsigned short uiWay) = 0;
	virtual void OnExit (unsigned short uiWay) = 0;
	virtual short MenuAndProcess (void) = 0;
};


#endif	//__DIAGNOSISUNIT_H_