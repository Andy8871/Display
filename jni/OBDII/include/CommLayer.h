// CommLayer.h: interface for the CCommLayer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CommLayer__
#define __CommLayer__

#include "DiagnosisUnit.h"


class CCommLayer
{
public:
	short SetupComm(void);
	short InitLinkLayer(void);
	short SetProtocolID(void);
	short SetFrameLength(CCommWithEcu::CFilterOrLengthRecognise &FilterCond);
	short SetReceFilter(CCommWithEcu::CFilterOrLengthRecognise &FilterCond);
	short SetCanIDFilter(CCommWithEcu::CFilterOrLengthRecognise &FilterCond,unsigned char IDLen);
	short SetTimeParameter(void);
	short SetBaudrate(void);
	short SetIOPort(void);
	short SetFlowControl(void);
	short SendEnterSysCmd(void);
	short SendExitSysCmd(void);
	virtual short MakeSendFrame(CSendFrame &SendFrame,CBinaryGroup vecCmdInfo);
	virtual short MakeSendFrame(CSendFrame &SendFrame,CBinary binCmdID);
	short GetbinCmdInfo(CBinaryGroup vecCmdInfo,CBinary &binCmdInfo);
	short MakeSendFrameCmd(CSendFrame &SendFrame,CBinary binCmdInfo);
	short CombineSendFrame(CSendFrame &SendFrame,CBinary binSendData,short iReceNum);
	short VerifyCommand(CBinary &Command);
	short VerifyCMD_CRC(CBinary &Command);
	short GetCommandInfo(CBinary binCmdID,CBinaryGroup &vecCmdInfo);
	short SendReceive(CBinary binCmdID,CBinaryGroup &vecReceFrm);
	short SendReceiveCmd(CBinary binCmdInfo,CBinaryGroup &vecReceFrm);
	short SendCommand(CSendFrame SendFrame,CBinaryGroup &vecReceFrm);
	short MatchCommMode(CBinaryGroup vecCmdInfo);
	short ParseReveFrame(CReceiveFrame ReceFrame,vector<CBinary> &vecReceFrm,short iIndex=0);
	short CheckReceFrm(CBinaryGroup &vecReceFrm);
	short GetEnterCmdID(void);
	short ChangeCommStatus(unsigned char bCurStatus,unsigned char bObjStatus);
	short OpenIdle(void);
	short CloseIdle(void);
	short InitHisData(void);
	short ReadAndUpdateHisData(CBinary binCmdID,CBinaryGroup &vecReceCmd);
	short ReadAndUpdateHisDataCmd(CBinary binCmdInfo,CBinaryGroup &vecReceCmd);
	short UpdateHisReceDataCmd(CBinary binCmdInfo,short iRet,CBinaryGroup vecReceCmd);
	short OBDIICommSet_15765(CBinary &binECUIDs);
	short OBDIICommSet_KLine(CBinary &binECUIDs);
	short OBDIICommSet_J1850_VPW(CBinary &binECUIDs);
	short OBDIICommSet_J1850_PWM(CBinary &binECUIDs);
	short OBDIISetIdle(void);
	short OBDIIResetFilter(unsigned char bECUID);
	CCommLayer();
	virtual ~CCommLayer();

	typedef struct CMD_INFO
	{
		CBinary binCmdID;
		CBinaryGroup vecReceCmd;
		short iSesult;
	}CMD_SESULT;

	typedef struct CMD_INFO1
	{
		CBinary binCmdInfo;
		CBinaryGroup vecReceCmd;
		short iSesult;
	}CMD_SESULT1;

	unsigned char bCommStatus;
	CBinaryGroup Link_Info; //用于记录LinkLayer的配置信息(Protocol_Link_Layer)
	CBinary binEnterCmdID;
	vector<CMD_SESULT> vecHisReceData;
	vector<CMD_SESULT1> vecHisReceDataCmd;
	short iRece_Result;
	int iPosReceTimeOut;
	int iNegReceTimeOut;
protected:

};

#endif
