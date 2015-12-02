// BasicDiagnosticUnit.h: interface for the CBasicDiagnosticUnit class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BasicDiagnosticUnit__
#define __BasicDiagnosticUnit__

#include "DiagnosisUnit.h"
#include "CommonTools.h"
#include "sysstr.h"
#include "CommLayer.h"

extern bool Demo_Flag;

class CBasicDiagnosticUnit : public CDiagnosisUnit  
{
public:
	CBasicDiagnosticUnit();
	virtual ~CBasicDiagnosticUnit();

	short GetFuncInfo(void);
	short GetFuncMenuInfo(CBinaryGroup &vecFuncMenuID);
	short SetSysNameID(CBinary binIndex);
	short OBDIIShow(void);
	short OBDIIDiagnose(void);
	short OBDIIExitSystem(void);
	short GetOBDIISysStrs(string &strSys,unsigned char bECUID);
	short OBDIIFuncShow(string strTitle);
	short GetFuncItemStr(CBinaryGroup &vecItemStrID,vector<string> &vecItemStr);
	short OBDIIOnBoardTest(string strTitle);
	short OBDIIOnBoardTestShow(string strTitle,CBinary binItemID);
	short OBDIIOnBoardTestShow1(string strTitle,CBinary binItemID);
	short GetTestIDStr(unsigned char bID,string &strItem);
	short GetTestConvInfo(unsigned char bID,CBinaryGroup &vecItemInfo);
	short OBDIIO2MonitorTest(string strTitle);
	short OBDIIO2MonitorTestShow(string strTitle,unsigned char bSensorID);
	short OBDIISysInfo_NoCAN(string strTitle,CBinaryGroup vecItemID);
	short OBDIISysInfo_CAN(string strTitle,CBinaryGroup vecItemID);
	short OBDIIReadnissCode(CBinary binTitle,CBinaryGroup vecItemID);

	CBinary binSysTxtIndex; // 用于记录菜单名称ID
	
protected:
	virtual void OnExit (unsigned short uiWay);
	virtual void OnEnter (unsigned short uiWay);
	virtual short ScanSystemExist (void);
	virtual short QuickClearTroubleCode (void);
	virtual short QuickReadTroubleCode (void);
	virtual short EnterSystem (void);
	virtual void ExitSystem (void);
	virtual short MenuAndProcess (void);
	virtual short NormalDiagnosis (void);
};

#endif
