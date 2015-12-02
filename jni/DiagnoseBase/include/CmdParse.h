///////////////////////////////////////////////////////////
//  CmdParse.h
//  Implementation of the Class CmdParse
//  Created on:      03-二月-2010 10:29:49
//  Original author: bwang7
///////////////////////////////////////////////////////////

#if !defined(EA_9E337918_B0E4_4c0b_B7CB_DAADF69A2DEE__INCLUDED_)
#define EA_9E337918_B0E4_4c0b_B7CB_DAADF69A2DEE__INCLUDED_

#define JZ_ADD
#ifdef JZ_ADD
#pragma warning(disable:4786)
#endif	// #ifdef JZ_ADD

#include "sendframe.h"
#include "PtcAPI.h"
#define FLAG_COMPLETE_FRAME			0x01
#define FLAG_UNCOMPLETE_FRAME		0x00
#define MAX_RCV_BUFF_SIZE			255
#define WORD_DEFALUT_FOR_POSIRESP_NUM			0xFFFF
typedef struct DTCInfo{
	int Num;
	unsigned short Interval;
	int sizeof1Dtc;
	DTCInfo(){
		Num = 0;
		Interval = 0;
		sizeof1Dtc = 0;
	}
}DTCInfo;
#define  ADDHEX2STR(pos,instr)	do{if(pos%2==0){instr.insert(pos,"\\x");pos = pos + 2;}pos++;} while(pos < instr.length());

typedef struct  tsRawData
{
	unsigned char rawData[MAX_RCV_BUFF_SIZE];
	unsigned short len; 
	int sizeInBlock;     //一段有效数据的长度."010203040506",2个字节为一段有效数据,则sizeInBlock = 2;
	tsRawData(){
		//ZeroMemory(rawData,sizeof(unsigned char)*MAX_RCV_BUFF_SIZE);
		memset(rawData, 0, sizeof(unsigned char)*MAX_RCV_BUFF_SIZE);
		len = 0;
		sizeInBlock = 0;
	};
}RawData;
typedef struct TSCmdData{
	unsigned long  CmdAddr;
	TP_BTs Cmd;	
	unsigned long  NumOfPP;
	unsigned long  PRAddr;
	unsigned long  NRAddr;
	bool   Status;
	unsigned long  PMethod;
	unsigned long  ByteOffSet;
	unsigned long  BitOffSet;
	unsigned long  SizeInBits;
	unsigned long RealSendIndex;
	unsigned long ReserveData;
	TSCmdData():NumOfPP(WORD_DEFALUT_FOR_POSIRESP_NUM){};
}TSCmdData;



#if 0
typedef struct tpStCommand
{
	unsigned long	dwCmdBytesDataAddr;	/**< Tx-data of the command */
	unsigned long	dwEntireFlag;		/**< flag to identify if a complete frame */
1	unsigned long	dwByteOfst;		/**< byte offset of Rx-data to extract raw-data */			
1	unsigned long	dwBitOfst;		/**< bit offset of Rx-data to extract raw-data */
1	unsigned long	dwSizeInBits;	/**< count of bits of Rx-data to extract raw-data */
1	unsigned long	dwNumOfPositiveResp;/**< desired number of Rx-frames */
1	unsigned long	dwPositiveRespAddr;	/**< positive response header address */
1	unsigned long	dwNegativeRespAddr;	/**< negative response header address */
	unsigned long	dwRealSendIndex;	/**< command physical sending index, 1-based */
	unsigned long	dwReservedData;		/**< reserved */
} StCmd;
#endif


#ifdef JZ_ADD

/***************************************************************************//**
\brief	accommodate one package of received-data, provide buffer & length by now
*******************************************************************************/
typedef struct tpStRecvData
{
	unsigned char buf[MAX_RCV_BUFF_SIZE];/**< buffer for received-data */
	unsigned char len;					/**< length of received-data, not used by now */
}stRecvData,*pstRecvData,**ppstRecvData;

/***************************************************************************//**
\brief	provide indexed multiple commands & received-data mapping relationship
*******************************************************************************/
typedef struct tpStCmdsRecv
{
	StCmd*		pstCmd;
	pstRecvData	pstRD;
	unsigned long		dwIdx;
	tpStCmdsRecv(StCmd* pstCmdIn,unsigned long dwIdxIn)
		:pstCmd(pstCmdIn),dwIdx(dwIdxIn),pstRD(NULL) {}
}stCmdsRecv;
typedef vector<stCmdsRecv> V_CRs;

/***************************************************************************//**
\brief		comparing function used by STL sort() for vector<stCmdsRecv> by
			pstCmd->?	
\param[in]	stCrL	compared stCmdsRecv-Left
\param[in]	rdeR	compared stCmdsRecv-Right
\return		bool	true if "stCmdsRecv-Left < stCmdsRecv-Right", false 
					otherwise
\warning	
*******************************************************************************/
bool CR_PredCmdRealSendIndex(const stCmdsRecv& stCrL,const stCmdsRecv& stCrR);
// bool CR_PredCmdBytesDataAddr(const stCmdsRecv& stCrL,const stCmdsRecv& stCrR);


/***************************************************************************//**
\brief	maximum piece of commands associated with 1 command-header. Morna says 
		this number is below 5
*******************************************************************************/
#define MAX_CMDS_OF_1CMDHDR			16

class CLinkLayer;
class CReceiveFrame;
#endif	// #ifdef JZ_ADD

// #include "adsStd.h"
/**
 * Function:Parse all the cmds which send to ECU.
 * Input:unsigned char array.
 * Output:CSendFrame
 */
typedef vector<unsigned short> V_WORD;
class CmdParse
{

public:
	CmdParse();
	virtual ~CmdParse();
	CmdParse& operator=(const CmdParse& cmd);
	
	bool  SetV30RawData(unsigned char*,DTCInfo* info = NULL,int index = -1);
	bool  SetV30RawData(CBinary&);
	void  SetCmdStatus(bool val = false){m_cmdData.Status = val;}

	RawData GetV30RawData(void){return m_V30rawData;}
	//return length of bits
	unsigned long ExtractV30RawData(unsigned long& convBuf,unsigned long cmdIndex = -1);

	int   Init(unsigned long addr,CPtcAPI* ptcData = NULL);
	int	  Init(unsigned char* data,short len,CPtcAPI* ptcData,unsigned char numOfCmds = 1,bool bCompleteFrame=false,int nReturnNumber=-1);

	void  SetCmdIndex(unsigned char index = 0);
	void  SetNumOfPositiveResp(unsigned char index,unsigned long dwNumber);
	unsigned long GetCmdNumOfPP(void);
	unsigned long GetCmdPosiRespAddr(void);
	unsigned long GetCmdNegaRespAddr(void);
	bool  GetCmdStatus(void);
	bool  GetCmdValue(TP_BTs& cmd);
	unsigned long GetCmdNum(void){return m_header.dwNums;}
	unsigned long GetCmdValueAddr(void);
	bool  GetFlagInit(void){return m_flagInit;};
	unsigned long GetCmdByteOffSet(void);
	unsigned long GetCmdBitOffSet(void);
	unsigned long GetCmdSizeInBits(void);
	StCmd* GetCurrCmd(void);
	//设置命令回帧个数
	void  SetCmdPPNum(unsigned long receNum){m_cmdData.NumOfPP = receNum;};
	unsigned long GetCmdRealSendIndex(void){return m_cmdData.RealSendIndex;}
	
	void  SetCmdValue(const TP_BTs& cmd);

private:
	void Clean(void);

	unsigned long GetCmdParseMethod(void);
	bool  GetSendCmd(unsigned char index = 0);
	void  FillCmdData(void);
	void  SetCmdValue(unsigned char* value,short len);

	
	TSCmdData m_cmdData;
	/**
	 * For CanBus protocol , it's used to fill the addtional defaulty bytes for a
	 * complete cmd.
	 */
	unsigned char m_defaultByte;
	/**
	 * ECUID
	 * 1:KWP Protocol has only one ECUID
	 * 2:CanBus Protocol has more than one ECUID,means Filter IDs
	 */
	V_WORD m_ecuIDs;
	/**
	 * the final format of cmd send to ECU, can be used directly by adsComm
	 */
	CSendFrame m_sendFrame;
	/**
	 * Tool ID
	 */
	unsigned short		m_toolID;
	StCmdHdr	m_header;
	StCmd*		m_cmdList;
	CPtcAPI*	m_ptc;
	unsigned char		m_cmdIndex;
	unsigned char		m_cmdParseType;
	RawData     m_V30rawData;
	bool		m_flagInit;

#ifdef JZ_ADD

public:
	/***************************************************************************//**
\brief	send multiple commands of a CmdHdr, then return multiple received-data
\param[in]	pPtcApi		to use this retrieve multiple commands of the CmdHdr 
\param[in]	pLinkLayer	to use this do real send-receive work 
\param[in]	dwCmdHdrAddr	address of command-header 
\param[out]	ppRxData		array of pointers of received-data, one can prepare
							this parameter like this:
							pstRecvData ppRxData[MAX_CMDS_OF_1CMDHDR] = {NULL};
\param[out]	dwRxDataCnt		counts of received-data
\return	bool	true if function succeeded, false otherwise
\warning   
\Added by Bin
*******************************************************************************/
// 	bool SendCmds(IN CPtcAPI* pPtcApi,IN CLinkLayer* pLinkLayer
// 		,IN unsigned long dwCmdHdrAddr,OUT ppstRecvData ppRxData
// 		,OUT unsigned long& dwRxDataCnt,OUT CReceiveFrame& receFrame);
/***************************************************************************//**
\brief	send multiple commands of a CmdHdr, then return multiple received-data
\param[in]	pPtcApi		to use this retrieve multiple commands of the CmdHdr 
\param[in]	pLinkLayer	to use this do real send-receive work 
\param[in]	dwCmdHdrAddr	address of command-header 
\param[out]	ppRxData		array of pointers of received-data, one can prepare
							this parameter like this:
							pstRecvData ppRxData[MAX_CMDS_OF_1CMDHDR] = {NULL};
\param[out]	dwRxDataCnt		counts of received-data
\param[out]	pMethod			point to "Error_Code_Handle_Method" when return -1 
\return	int					no(0) / positive(1) / negative(-1) response	
\warning
*******************************************************************************/
	int SendCmds(IN CPtcAPI* pPtcApi,IN CLinkLayer* pLinkLayer
		,IN unsigned long dwCmdHdrAddr,OUT ppstRecvData ppRxData
		,OUT unsigned long& dwRxDataCnt,OUT unsigned long* pMethod = NULL);

/***************************************************************************//**
\brief	after a CmdHdr finish "SendCmds()", we extract raw-data for it 	
\param[in]	pCmds		multiple commands of a CmdHdr
\param[in]	ppRxData	array of pointers of received-data
\param[in]	dwCmdCnt	count of commands
\param[out]	stRde		after function succeeded, "stRde.pbtRawData" and
						"stRde.dwRawDataBits" will be filled in.
\return		bool	true if function succeeded, false otherwise
\warning	
*******************************************************************************/
	bool ExtractRawData(IN StCmd* pCmds,IN ppstRecvData ppRxData
		,IN unsigned long dwCmdCnt,OUT RDE& stRde);
	bool ExtractRawData_DTC(IN StCmd* pCmds,IN ppstRecvData ppRxData
		,IN unsigned long dwCmdCnt,OUT RDE& stRde);

/***************************************************************************//**
\brief			send multiple CmdHdr, get multiple raw-data
\param[in]		pPtcApi		to use this retrieve StCmdHdr, StCmd or others 
\param[in]		pLinkLayer	to use this do real send-receive work 
\param[in,out]	vRDEs		prepare dwCmdHdrAddr & dwCmdGroup beforehand. e.g.
								V_RDEs v_rdes;
								for(i=0;i<hdr.dwNums;i++)
								{
									v_rdes.push_back(RDE(m_pEIs[i].dwCmdHdrAddr
									,m_pEIs[i].dwCmdGroup));
								}
							, and after function successfully done, raw-data
							info. will also be filled in this parameter.
\return	bool	true if function succeeded, false otherwise
\warning
*******************************************************************************/
	bool ExtractRawDatas(IN CPtcAPI* pPtcApi,IN CLinkLayer* pLinkLayer
		,IN OUT V_RDEs& vRDEs);

/***************************************************************************//**
\brief		
\param[in]	
\param[out]	
\return		bool	true if ready-cmd OK, false otherwise
\warning	
*******************************************************************************/
	bool SendReadyCmd(IN CPtcAPI* pPtcApi,IN CLinkLayer* pLinkLayer
		,IN unsigned long CmdHdrAddr);

private:
	stRecvData	m_RxData[MAX_CMDS_OF_1CMDHDR];
#endif	// #ifdef JZ_ADD

};
#endif // !defined(EA_9E337918_B0E4_4c0b_B7CB_DAADF69A2DEE__INCLUDED_)
