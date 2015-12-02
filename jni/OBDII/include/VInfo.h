// VInfo.h: interface for the CVInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VInfo__
#define __VInfo__

class CVInfo
{
public:
	CVInfo();
	CVInfo(string strMenuDbFileName);
	virtual ~CVInfo();

public:

	short StartShow(void);
	short GetSelectMenu(CBinaryGroup &vecMenuItem);
	short ShowVehicleInfo(vector<CBinaryGroup> vecModelGrpInfo);
	short GetModelGrpInfo(vector<CBinaryGroup> vecOrgModelGrpInfo,vector<CBinaryGroup> &vecModelGrpInfo,CBinaryGroup &vecItemIndex);
	short ExistInVecGrp(CBinary binItem,vector<CBinaryGroup> vecGrpInfo,short iIndex,short iSelLevel);
	short GetVehicleID(CBinaryGroup vecItemIndex);
	short SelectLevelInfo(string strTitle,short iLevel,vector<CBinaryGroup> &vecModelGrpInfo,CBinaryGroup &vecItemIndex);
	short ShowSystemList(void);
	short GetAllSystemInfo(CBinaryGroup &vecSysNameID,vector<CBinaryGroup> &vecGrpSystemID);
	short SystemEnter(CBinary binTitle,CBinaryGroup vecSystemID);

};

#endif
