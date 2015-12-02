///////////////////////////////////////////////////////////
//  CLinkLayer.h
//  Implementation of the Class CLinkLayer
//  Created on:      03-二月-2010 15:14:54
//  Original author: bwang7
///////////////////////////////////////////////////////////

#if !defined(EA_114CCCA3_8004_4a3e_A3DA_C816B6594FDE__INCLUDED_)
#define EA_114CCCA3_8004_4a3e_A3DA_C816B6594FDE__INCLUDED_
// #include <afx.h>
#include "adsStd.h"
#include "CommWithEcu.h"
#include "CmdParse.h"
#include "Binary.h"
#include "PtcAPI.h"
#include "IDListParse.h"
#include "ResStr.h"
#include "CheckPLLFunc.h"

//对应于纬度表中的Protocol Mode sheet.
#define PRO_KWP2000								0x01
#define PRO_KWP2000_VOLVO						0x02
#define PRO_CAN11								0x03
#define PRO_CAN11_AddressCode					0x04
#define PRO_CAN11_SysID							0x05
#define PRO_CAN29								0X06
#define PRO_CAN29_VOLVO							0X07
#define PRO_CAN_SINGLE							0X08
#define PRO_CAN_GM								0x09
#define PRO_ISO9141								0X0A
#define PRO_KWP2000_115200						0X0B
#define PRO_RECEIVEONLY							0X0C
#define PRO_BOSCH								0X0D
#define PRO_MOTOROLA							0X0E
#define PRO_J1850PWM							0X0F
#define PRO_J1850VPW							0X10
#define PRO_UBP									0X11
#define PRO_MB_5BPS								0X12
#define PRO_MB_9BITS							0X13
#define PRO_GM_RING								0X14
#define PRO_GM_NORING							0X15
#define PRO_GM_5BPS								0X16
#define PRO_MIT_POSITIVE						0X17
#define PRO_MIT_NEGATIVE						0X18
#define PRO_NISSAN_OLD							0X19
#define PRO_BMW_MODE2							0X1A
#define PRO_BMW_MODE2_WITHLOWPLUS				0X1B
#define PRO_BMW_MODE3							0X1C
#define PRO_BMW_MODE4_WITHLOWPLUS				0X1D
#define PRO_BMW_MODE5							0X1E
#define PRO_BMW_MODE5_WITHLOWPLUS				0X1F
#define PRO_BMW_MODE6							0X20
#define PRO_TRW									0X21
#define PRO_NORMAL_KLINE						0X22
#define PRO_FLASHCODE_ONEunsigned char					0x23
#define PRO_FLASHCODE_TWOunsigned charS					0x24
#define PRO_FLASHCODE_THREEunsigned charS				0x25
#define PRO_FLASHCODE_FOURunsigned charS					0x26

#define PRO_CHRYSLER_CCD						0x27
#define PRO_SHORT_CAN							0x28

#define PRO_NOT_DEFINEDE						0xFE


// #include <string>
// #pragma warning(disable:4786) 

// #include <vector>
// using namespace std;

//sendCmd(...)的回包
#define  POSI_RESP								1
#define  NO_RESP								0
#define  NEGA_RESP								-1

class CPtcAPI;
class CmdParse;
class EnterExitSysFuncParse;
class PLLParse;
class CEnterSys;
class CRunLog;
/**
 * <ol>
 * 	<li>the logic of link layer</li>
 * 	<li>setup the environment of communication</li>
 * 	<li>provide the interface to send/receive data.</li>
 * </ol>
 */
typedef struct  
{
	CSendFrame sendFrame;
	unsigned char Cmd_SID;
}StLastCmd;
typedef enum 
{
	emHPNone = 0
		,emHPK2000_0x = 1
		,emHPK2000_0x_CS = 1
		,emHPLength = 1
		,emHPK2000_00 = 2
		,emHPUART = 2
		,emHP5Bps = 2
		,emHPISO = 3
		,emHPJ1850VPW = 3
		,emHPJ1850PWM = 3
		,emHPUBP = 3
		,emHPBMW_MODE3 = 3
		,emHPBMW_MODE2 = 2
		,emHPFuncAddrC1 = 3
		,emHPK2000_8x = 4
		,emHPK2000_80 = 4
		,emHPCAN11 = 4
		,emHPCAN29 = 6
		,emHPSHORTCAN = 4
} emPackageHeaderLength;
typedef vector<string> V_STR;
class CLinkLayer
{
public:
	bool SetHoldonSpecial(short iFilterByte);
	CLinkLayer();
	virtual ~CLinkLayer();
	bool VersionJump(string Addrs);
	CRunLog* GetRunLogPointer(void){return m_log;};
	bool InitLink(const char* ptcList = NULL,CPtcAPI* ptcData = NULL,bool FileJump = false);
	/*
	Parameters :
		sendReturn: especially used for KWP2000 Enter System, Some cmds only need to Send once,
					and return immediately!
		sendNotCheck: send but not check the response negative or positive.
		RecvLen     : the length of received the frame.
		RepeateTime : For Datastream use.
		receFrames  : Used for more than one frames received.DTC used.Maybe other functions will use it. receiveFrame = adsCommuECU.SendAndReceive(...);

		            
	*/
	int SendCmd(CmdParse& cmd,unsigned char* RecvData = NULL,bool sendReturn = false,	\
		bool sendNotCheck = false,short* RecvLen = NULL,int Repeattime = 3,	\
		OUT CReceiveFrame* receFrames = NULL,unsigned long* pMethod = NULL);	

	int SendCmd(IN unsigned char* pTx,IN short nTxLen,OUT unsigned char* pRx,OUT short* pnRx,CReceiveFrame* receiveFrame = NULL,bool sendReturn = false); 

	bool ReceiveOnly(CReceiveFrame& receFrame,bool bClearBuffer = false);

	CReceiveFrame CanSendReceive (CSendFrame& SendFrame);
	
	int SendCmd(IN unsigned char* pTx,IN short nTxLen,bool bWholeFlag,int nReturnNumber,CReceiveFrame* receFrame);
	
	bool ReenterSystem(void);
	
	
	
/*Send EnterSys cmd only used */
	bool SendEnterSysCmd(CmdParse& cmd,unsigned char* RecvData = NULL,bool sendReturn = false); // only used for CEnterSys Class.
	bool SendKeepLinkCmd(void);
/*Send EnterSys cmd only used */
	void SetCsLen(unsigned char len){m_csLen = len;};
	unsigned char GetCsLen(void){return m_csLen;};
/*Only Saved for IDList*/
	bool SetVersionID(const RawData &);
	bool CheckVersionID(void);
	string GetECUInforFVersionID(void);
	string GetParametersFVersionID(void);
	string GetFunctionalTestFVersionID(void);
	string GetSpecialTestFVersionID(void);	// JZ ADD 
	string GetRDTCHDAddrFVersionID(void);
	string GetEDTCHDAddrFVersionID(void);	// JZ ADD
	
/***************************************************************************//**
\brief	get available addresses of specified "Function" by VersionID method		
\param[in]	
\param[out]	
\return		bool	true if function succeeded, false otherwise
\warning	
*******************************************************************************/
	bool GetVidAvailAddrs(unsigned long dwFuncId,vector<unsigned long>& vAvailAddrs);// JZ ADD
	bool GetMethod14RxData(stRecvData*);							//JZ needed.
/*Only Saved for IDList*/

/*Open this function because of QuickTestFunc*/
	bool LoadPTC(string);
/*Open this function because of QuickTestFunc*/

	unsigned long GetProtocolID();

	string GetRunningDAT(void){return m_RunningDATFile;};
	void SetEcuInforFromAddrEnterSys(CReceiveFrame buf){m_ecuInforFromAddrEnter = buf;};
	CReceiveFrame GetEcuInforFromAddrEnterSys(void){return m_ecuInforFromAddrEnter;};

	/*Reset the link parameters*/
	bool ResetLink(unsigned long linkLayerAddr,bool EnterSysMode = false);
	bool AddInfors2Cmd(CmdParse& cmd,CSendFrame& outCmd,unsigned char& Cmd_SID,int& num);

	/*ReadDTC method2 and method3 */
	bool ParseRawDataFromReceiveFrame(CReceiveFrame& rfRxf,int* index = NULL);
	/*Get PackageHeaderLength*/
	emPackageHeaderLength GetPackageHeaderLength(void){return m_PackageHeaderLength;};
	
	void SetQuickTestMode(bool mode){m_QuickTestMode = mode;};
	bool GetQuickTestMode(void){return m_QuickTestMode;};
	//IDList special Used
	void SetIDListType(bool val){m_IDListType = val;};
	bool GetIDListType(void){return m_IDListType;};
	void SetNegaInfo(unsigned long val){m_negaInfor = val;if (m_negaInfor != INVALID_DWORD){m_lastNegaInfor = m_negaInfor;}};
	void RecoverNegaInfo(void){m_negaInfor = m_lastNegaInfor;};
	void SetCAN11SysID(unsigned char val){m_CAN11SYSID = val;};
	unsigned char GetCAN11SysID(void){return m_CAN11SYSID;};
	void LinkClean(void);
	bool isLastFile(void);
	void SetFuncID(unsigned long iID); // Add by Pan 20150714
private:
	bool GetPTCList(const char* ptcList = NULL);
// 	bool LoadPTC(string&);
	CEnterSys* m_enterSys;
	bool AddCs2Cmd(CBinary& value);
	CBinary AddCs2Cmd(int CsPos,unsigned char* cmd);
	unsigned char GetVerifyLen(void);
	bool ExitSys(void);
	StLastCmd m_lastCmd;  //judge the cmd should be sent or not.if the new cmd's address 

	//////////////////////////////////////////
	bool SetupCommEnvironment(void);
	bool CanSetFilterID(TP_BTs&,CCommWithEcu::CFilterOrLengthRecognise&);
	TP_BTs m_toolID;
	unsigned char DefaultByte;
	bool flag_vLen;
	string m_RunningDATFile;
	unsigned char m_csLen; //校验位长度
	CSendFrame CanMakeSendFrame(CBinary pcCmd,short RFNum);


	unsigned short ParseRawReceiveDatas(CReceiveFrame& rFrame,CSendFrame,bool& isGM_UUDT,unsigned char SID = 0,unsigned char* ReceData = NULL);

	//KWP2000 refer!
	bool SetHighLowVoltage(string &value);
	//////////////////////////////////////////


	//all used 
	V_STR m_fileList;
	CPtcAPI* m_ptc;
	PLLParse* m_pllParse;
	CmdParse* m_cmdParse;
	EnterExitSysFuncParse* m_funcParse;
	CCheckPLLFunc m_checkPLLFuncs;
// 	CCommWithEcu adsCommEcu;

	/*Special used */
	CIDListParse m_IDList;
	CBinary* m_versionID;
	bool m_IDListType; //true:16进制；FALSE:10进制


	StPLL m_succeedPLL;
	CmdParse m_exitSysCmd;
	bool SetExitSysCmd(void);

	//m_LinkLayerOrgParas[0]: OriginLLParas;
	//m_LinkLayerOrgParas[1]: LatestLLParas;
	//m_LinkLayerOrgParas[2]: CurrentLLParas;
	StLinkLayerParas m_LinkLayerOrgParas[3]; 

	CReceiveFrame m_ecuInforFromAddrEnter;
	emPackageHeaderLength m_PackageHeaderLength;
	bool m_QuickTestMode;
	
	CRunLog* m_log;

	unsigned long m_negaInfor;
	unsigned long m_lastNegaInfor;
	unsigned char  m_CAN11SYSID;
	bool m_bVersionIDValid;	// JZ ADD
	
	unsigned long iFuncID; // Add by Pan 20150714
};
#endif // !defined(EA_114CCCA3_8004_4a3e_A3DA_C816B6594FDE__INCLUDED_)
