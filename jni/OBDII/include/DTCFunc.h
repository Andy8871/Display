// DTCFunc.h: interface for the CDTCFunc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DTCFunc__
#define __DTCFunc__

#include "DiagnosisUnit.h"

class CDTCFunc
{
public:
	CDTCFunc();
	virtual ~CDTCFunc();

	void InitDTCFunc(void);
	short CheckDTCFunc(void);
	short GetDTCFuncMenu(CBinaryGroup &vecFuncMenuID);
	short DTCFuncShow(CBinary binTitle);
	short ReadDTCFunc(CBinary binTitle);
	short ReadDTCCode(void);
	short GetDTCNum(CBinaryGroup vecItemInfo);
	short GetDTCCodeHandle(CBinaryGroup vecItemInfo,short iNum);
	short InputVecDTCInfo(CBinary binCode,unsigned char bStatus);
	short GetVecDTCStr(vector<string> &vecStrCode,vector<string> &vecStrDes,vector<string> &vecStrStatus);
	short GetDTCCodeInfo(CBinary binCode,CBinaryGroup &vecItemInfo);
	short GetDTCCodeInfoByECUID(CBinary binECUID,CBinary binCode,CBinaryGroup &vecItemInfo);
	short ShowOBDIIBrands(CBinary &binECUID);
	string GetStrDTCStatus(unsigned char bType);
	string GetStrDefaultCode(CBinary binCode);
	short ClearDTCFunc(CBinary binTitle);
	short ClearDTCCode(void);

	typedef struct DTC_INFO
	{
		CBinary binCode;
		CBinary binStatus;
	}DTCINFO;

	bool Flag_DTC;
	vector<DTCINFO> vecDTCInfo;
	bool Flag_Success;
protected:
};

#endif
