// DataListFunc.h: interface for the CDataListFunc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DataListFunc__
#define __DataListFunc__

#include "DiagnosisUnit.h"

extern bool Demo_Flag;

class CDataListFunc
{
public:
	CDataListFunc();
	virtual ~CDataListFunc();

	short GetDataListVecItem(void);
	short ScanDataItem(CBinaryGroup vecItemInfo);
	short CheckPIDSupp(CBinary binData,unsigned char bPID,short iPos);
	short OBDIIDataScan(void);
	short DataListShow(CBinary binTitle);
	short DataListFlash(CBinary binTitle,CBinaryGroup vecAllItemID);
	short SystemInfoShow(CBinary binTitle,CBinaryGroup vecAllItemID);
	short GetDSGrpStrIDs(CBinaryGroup &vecItemStrID);
	short GetDSGrpItems(CBinaryGroup &vecItemID,unsigned char bGrp);
	short GetItemTxtInfo(CBinaryGroup vecItemID,vector<string> &vecItemName,vector<string> &vecItemUnit);
	void DataListFuncInit(void);
	short DataListInitComm(void);
	short DataListEndComm(void);
	short ReadDataAndCalculate(CDatabase &dbData,CBinary binItemIndex,string &StringValue);
	short GetStringValue(unsigned char bType,CBinary binConvID,CBinary binData,string &strResult);
	short DisType0(CBinary binData,CBinary binConvID,string &strResult);
	short DisType1(CBinary binData,string &strResult);
	short DisType2(CBinary binData,string &strResult);
	short DisType3(CBinary binData,string &strResult);
	short DataInScope(CBinary binData,CBinary binMin,CBinary binMax);
	string GetNumDisFormat(unsigned char bNumOfDec);
	short Conv_Linearity(CBinaryGroup vecItemInfo,CBinary binData,string &strResult);
	short Conv_Status(CBinaryGroup vecItemInfo,CBinary binData,string &strResult);
	short Conv_Display(CBinaryGroup vecItemInfo,CBinary binData,string &strResult);
	short Conv_Spe1(CBinaryGroup vecItemInfo,CBinary binData,string &strResult);
	short Conv_Spe2(CBinaryGroup vecItemInfo,CBinary binData,string &strResult);
	short Conv_Spe3(CBinaryGroup vecItemInfo,CBinary binData,string &strResult);
	short Conv_Spe4(CBinaryGroup vecItemInfo,CBinary binData,string &strResult);
	short Conv_Spe5(CBinaryGroup vecItemInfo,CBinary binData,string &strResult);

	double GetCalculateResult(void);

	short OBDIICheckFreezeData(void);

	vector<CBinary> vecDataListIndex;

	bool FirstTimeInit;
	double fCalResult; //数值算法计算结果
	bool Flag_DataList;

protected:

};

#endif
