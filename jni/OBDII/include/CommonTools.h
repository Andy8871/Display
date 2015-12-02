// CommonTools.h: interface for the CCommonTools class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CommonTools__
#define __CommonTools__

extern bool Demo_Flag;

class CCommonTools
{
public:
	short ReadDBFile(vector<CBinary> &ucInfo,string FileName,CBinary binId);
	short GetDataBase(CDatabase &dbData,string FileName);
	short GetVecInfoFromDB(CDatabase &dbData,CBinaryGroup &ucInfo,CBinary binId);
	string GetTxtString(CBinary binTxtID);
	short GetTxtIndex(CBinary &binTxtID);
	short UpdateBinIndex16(CBinary &binIndex,unsigned short iIndex);
	short UpdateBinIndex32(CBinary &binIndex,unsigned int iIndex);
	short MenuAddItem(CBinary TitleTxt,vector<CBinary> AllMenuTxt);
	short MenuAddItemStr(string TitleTxt,vector<string> AllMenuTxt);
	short CBinaryCompare(CBinary binData1,CBinary binData2);
	short CheckCtrlPosAvail(CBinaryGroup vecReceCmd,CBinary binCtrlPos);
	short CheckCtrlPosAvailFF(CBinaryGroup vecReceCmd,CBinary binCtrlPos);
	short GetCtrlData(CBinaryGroup vecReceCmd,CBinary binCtrlPos,CBinary binMasks,CBinary &binData);

	short IsExistDBFile(string FileName,CBinary binId);
	short GetIndexPosInVector(vector<CBinary> vInfo,CBinary CBIndex);
	short GetFuncCmds(vector<CBinary> vecFuncIDs,string FileName);
	short IsCBinaryAvailable(CBinary CBInfo);
	short IsExistInVecBin(vector<CBinary> vecBin,CBinary binItem);
	short StrBinToVecInt(CBinary binStr,vector<unsigned short> &vecInt);
	short GetMaxPos(CBinary binPos1,CBinary binPos2,CBinary binPos3);
	bool IsBufOverFlow(CBinary binCmd,unsigned short iPos);
	short GetDataByMasks(CBinary &binData,CBinary binMasks);
	unsigned char StrBinToBYTE(CBinary binStr);
	unsigned short StrBinToUINT16(CBinary binStr);
	short StrBinToINT16(CBinary binStr);
	unsigned int StrBinToUINT32(CBinary binStr);
	double StrBinToFLOAT(CBinary binStr);
	short MatchVersionID(CBinary binECUID,CBinary binData);
	short GetSpecStepID(CBinary binStepID);
	short GetBytePosInCBinary(CBinary binData,unsigned char bData);
	short StringToCBinary(string strData,CBinary &binData);
	
	CCommonTools();
	virtual ~CCommonTools();

protected:
};

#endif
