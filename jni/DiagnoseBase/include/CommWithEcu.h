#ifndef	__COMMWITHECU_H__
#define	__COMMWITHECU_H__

#include "adsStd.h"
#include "ReceiveFrame.h"
#include "SendFrame.h"
#include "SendReceivePacket.h"
#include "Binary.h"
#include "RunLog.h"
#ifdef WIN32
#include "PtcAPI.h"
#endif
/*
#ifdef DATA_DRV_EXT
class CRunLog;
#endif
*/

/*
class CCommWithEcu;

CCommWithEcu* getCommWithEcuObject();
*/

class CCommWithEcu
{
protected:
	enum 
	{
		N0_CYCLE     = 0,  // 不循环
		SINGLE_CYCLE = 1,  // 单帧循环
		MULTI_CYCLE  = 2,  // 多帧循环
		FLASH_CYCLE  = 3,   // 闪光码循环
		QUICK_FLASH_CYCLE = 4 //成快速闪光码循环
	};
	
	// 数据接收发送句柄
	CSendReceivePacket *m_SendRecv;

	bool m_bAssembledFlag;		// 合发送标识，执行Begin()为true, 执行End()为false 
	int m_iEcuNum;                 // 组合发送中，更ECU通信的命令个数

	bool m_bRepeating;			// 循环发送标志

#ifdef DATA_DRV_EXT

	/************************************************************************/
	/* Bin 添加 RunLog功能                                                  */
	/************************************************************************/

	CRunLog* m_runLog;
#endif
	unsigned char*	m_SendCommBuf;			// 发送通信缓冲区
	unsigned char*	m_RecieveCommBuf;	    // 接收通信缓冲区

	unsigned char*	m_pchBuf;		// 指向当前通信缓冲区的偏移位置

	typedef struct _tagAssembleChain
	{
		int		retType;		//返回值类型：1-short(错误码型)、2-CReceiveFrame(接收类数据型)
		void*	inValueAddres;	//输入值的地址指针：对于类型3的数据类型，需要根据输入值的信息来接收返回值的数据
	}ASSEMBLECHAIN;

	vector<ASSEMBLECHAIN> m_vAssembleChain;
	
public:
	static CCommWithEcu* GetInstance();
private:
	static CCommWithEcu* m_pInstance;
	CCommWithEcu(void) { m_bAssembledFlag = false;  m_bRepeating = false; m_bCommBlockStart=false; m_hBlockHandle = 0; };
	~CCommWithEcu(void) { };

	class Garbo
	{
		~Garbo()
		{
			if (NULL != CCommWithEcu::m_pInstance)
				delete CCommWithEcu::m_pInstance;
		}
	};

	static Garbo m_garbo;
public:
	////////////////////////////////////////////////////////////////////////
	int m_hBlockHandle;
	bool m_bCommBlockStart;
	void CommBlockEnd(void);
	void CommBlockStart(void);
	///////////////////////////////////////////////////////////////////////
	void SetDemoFrame(CBinaryGroup &bg);

	bool Debug(bool bIsDebug);

	// 建立与下位机的连接, 分配通信缓冲区
	void Init (void);
#ifdef DATA_DRV_EXT
	/************************************************************************/
	/* Bin 设置RunLog													     */
	/************************************************************************/

	void SetRunLog(CRunLog* log){m_runLog = log;};
	CRunLog* GetRunLog(void){return m_runLog;};
#endif
	// 销毁与下位机的连接
	void Destroy (void);

	// 执行此函数后，执行的给下位机的数据会打在同一数据包中，直到执行End()才发送为止
	void Begin (void);

	// 此函数后发送从Begin()函数开始打包的命令给下位机，并取得应答
	CReceiveFrame End (void);

	// 设置等待下位机回复的最大时间，单位毫秒
	void WaitCommPacketMaxTime (unsigned int iMaxTime);

	// 设置输入输出端口及通信参数(0xEF为无效数据)
	#define DB15_PIN0_INPUT_GROUND											  0x00
	#define DB15_PIN1														  0x01
	#define DB15_PIN2_INPUT													  0x02
	#define DB15_PIN3														  0x03
	#define DB15_PIN4_OUTPUT_INVALIDATION									  0x04
	#define DB15_PIN4_INPUT_UNCONNECT										  0x04
	#define DB15_PIN5														  0x05
	#define DB15_PIN6														  0x06
	#define DB15_PIN7														  0x07
	#define DB15_PIN8														  0x08
	#define DB15_PIN9														  0x09
	#define DB15_PIN10_INPUT												  0x0A
	#define DB15_PIN11														  0x0B
	#define DB15_PIN12														  0x0C
	#define DB15_PIN13														  0x0D
	#define DB15_PIN14														  0x0E
	#define DB15_PIN15_INPUT_WORK_VOLTAGE									  0x0F

	#define DB15_PIN14_153_6kHz												  0x1E
	#define DB15_PIN1_HIGH_ELECTRICITY										  0x11
	#define DB15_PIN5_HIGH_ELECTRICITY										  0x15

	#define DB15_VPW														  0x22
	#define DB15_PWM														  0x42
	#define DB15_CAN_PIN6_14												  0x86
	#define DB15_CAN_PIN3_11												  0x83
	#define DB15_CAN_PIN3_8													  0x88	//hpy add for Allen requirement 2013-5-9
	#define DB15_SINGAL_CAN													  0x91	//hpy add for Allen requirement 2013-10-31


	#define DB15_CANBUS														  0x86	//为兼容而保留，建议用DB15_CAN_PIN6_14来替代

	//用于且仅用于GetAdcStatus(),GetChannelVoltage()函数参数
	#define DB15_NO_SWITCH_CHANNEL											  0xFF
	
	#define IOS_INPUT_POSITIVE_LOGIC         0x01
	#define IOS_INPUT_NEGATIVE_LOGIC         0x00

	#define IOS_OUTPUT_POSITIVE_LOGIC        0x02
	#define IOS_OUTPUT_NEGATIVE_LOGIC        0x00
	
	#define IOS_PULL						 0x00 //仅为与前一版本兼容，已无实际意义
	#define IOS_INPUT_PULL_UP				 0x40 //日产加入输入上拉。2006.7.19
	#define IOS_OUTPUT_REVERSE				 0x80

	short SetIoPort (	unsigned char iIoOutputPort,
						unsigned char iIoInputPort, 						
						char iOutputVoltage = 12,
						unsigned short ui16IoSelectItem = IOS_INPUT_POSITIVE_LOGIC | IOS_OUTPUT_POSITIVE_LOGIC);

	// 设置输入输出端口选通或断开
	short EnableOutputIoLine (unsigned char iIoOutputPort, bool bIsConnect = true);

	//PB_  party bit 
	#define PB_SPACE														4 //0校验
	#define PB_MARK															3 //1校验
	#define PB_EVEN															2 //偶校验
	#define PB_ODD															1 //奇校验
	#define PB_NONE															0 //无校验
	// 设置与ECU通信波特率及位格式
	short SetBps (double fBps, char iParityBit = PB_NONE, char iDataBit = 8);
	short SetBRs (unsigned char BR1,unsigned char BR2,char iParityBit = PB_NONE, char iDataBit = 8);

	// 设置与ECU通信时间参数
	short TimeIntervalOver (int iMaxWaitReceivePacket,
							  int iSendPacketMinWaitTime,
							  double fSendByteMinWaitTime,
							  int iNegativePacketWaitTime=0);

	// 设置链路保持
	short KeepLink (short iKeepTime = 0,  CSendFrame *pKeepLinkCommand = NULL);

	// 设置BOSCH链路保持
	short BoschKeepLink (bool bEnableKeep = false);

	// 设置输出电平高低时间，省略的参数必须为unsigned short类型
	short VoltageHighLowTime (short iHighLowTimes, ...);
	// 设置输出电平高低时间，省略的参数必须为unsigned short类型, added by bin for data driven
	short VoltageHighLowTime (short count,vector<short>& Times);

	//add by scf 2008.07.18
	// 设置输出电平高低时间，省略的参数必须为unsigned short类型
	short VoltageHighLowTenthTime (short iHighLowTimes, ...);

	
	//设置高速通信方式 0 低速  1 高速 注：在退出高速的诊断方式时一定要还原为低速设置SetHighSpeed(0);
	short SetHighSpeed(unsigned char pParameter);
	
	//设置BMW特殊方式(发命令之前先高低电平触发)通信方式命令字 0 无效  1 有效 注：在退出时一定要还原为无效设置SetBMWSpecial(0);
	short SetBMWSpecial(unsigned char pParamerer);
	
	//设置CAN 的硬件过滤模式
	short SetCanHardMaskFilter(unsigned char pParamerer);

	//设置消极应答的处理模式
	//short SetNegativeHandleMethod(unsigned char iPosition1, unsigned char iNegByte, unsigned char iPosition2, unsigned char iNegParameter, unsigned char iHandleType);//hpy modified 20131010
	short SetNegativeHandleMethod(unsigned char iPosition1, unsigned char iNegByte, unsigned char iPosition2, unsigned char iNegParameter, unsigned char iHandleType, unsigned short iMaxCnt = 100);

	// 地址码进入系统，省略的参数必须为unsigned short类型
	#define ACE_BPS_AUTO_RECOGNIZE           0x01
	#define ACE_KW2_REVERSE_TO_ECU           0x02
	#define ACE_GET_ADDRESS_REVERSE_FROM_ECU 0x04
	#define ACE_RECEIVE_KW5					 0x08
	#define ACE_RECEIVE_ONE_FRAME			 0x10
	#define ACE_KW1_REVERSE_TO_ECU			 0x20
	#define ACE_RECEIVE_N_FRAME				 0x40
	#define ACE_CLOSE_TRIGGER_CHANNEL		 0x80

    /********************************************************************
      功    能：地址码进入系统
      参数说明：unsigned char ucAddressCode 地址码
	            unsigned short ui16Parameter 地址码进入选项，取值范围为以下的组合
                  ACE_BPS_AUTO_RECOGNIZE           -- 自动识别波特率
                  ACE_KW2_REVERSE_TO_ECU           -- 发KW2取反至ECU
                  ACE_GET_ADDRESS_REVERSE_FROM_ECU -- 从ECU接收地址码取反值
                  ACE_RECEIVE_KW5                  -- 接收5个字节KeyWord, 
				                                      如未选中本项则接收2个KW字节
                  ACE_RECEIVE_ONE_FRAME            -- 从ECU接收一帧数据
                  ACE_KW1_REVERSE_TO_ECU           -- 回复KW1取反至ECU
                  ACE_RECEIVE_N_FRAME              -- 接收多帧
                  ACE_CLOSE_TRIGGER_CHANNEL        -- 关闭触发信号通道
                double fBps  发地址码的波特率(bps)
                short i16SendKw2TimeInterval  KW2取反发回的时间间隔(mS)
      返 回 值：ECU  回复帧数据
      说    明：无
    ********************************************************************/
	CReceiveFrame AddressCodeEnter (unsigned char ucAddressCode, 									
									unsigned short ui16Parameter = ACE_BPS_AUTO_RECOGNIZE | ACE_KW2_REVERSE_TO_ECU | ACE_GET_ADDRESS_REVERSE_FROM_ECU,
									double fBps = 5,
									short i16ReverseKw2TimeInterval = 30);


	/********************************************************************
	  功    能：BOSCH地址码进入系统
	  参数说明：unsigned char ucAddressCode  地址码
				short i16SendKw2TimeInterval  KW2取反发回的时间间隔(mS)
	  返 回 值：ECU回复的版本信息
	  说    明：无
	********************************************************************/
    //CReceiveFrame CCommWithEcu::BoschEnter (unsigned char ucAddressCode,	short i16ReverseKw2TimeInterval = 30);
    CReceiveFrame BoschEnter (unsigned char ucAddressCode,	short i16ReverseKw2TimeInterval = 30);
	// 取得下位机状态, 仅用于非组合发送
	//GS_ Get Status
	#define GS_CHANNEL_SIGNAL											  0x00
	#define GS_CHANNEL_VOLTAGE											  0x01
	short GetStatus(unsigned char ui8StatusWay, unsigned char ucIoInputPort = DB15_NO_SWITCH_CHANNEL);


	// 设置与ECU通信协议及参数
	#define PROTOCOL_NORMAL					0x00	// 一般格式收发协议
													//缺省过滤条件 无

	#define	PROTOCOL_KWP					0x01	// KWP协议
													//缺省过滤条件 无

	#define	PROTOCOL_ISO					0x02	// ISO协议
													//缺省过滤条件 无

	#define	PROTOCOL_VPW					0x03	// VPW协议
													//缺省过滤条件 起始位置0, 长度2, 字节0x486B

	#define	PROTOCOL_PWM					0x04	// PWM协议
													//缺省过滤条件 起始位置0, 长度2, 字节0x416B

	#define	PROTOCOL_CANBUS					0x05	// CANBUS协议
	#define	PROTOCOL_CAN_SINGLE_LINE		0x85	// CANBUS协议, 单线通信
													//缺省过滤条件 无

	#define	PROTOCOL_BOSCH					0x06	// BOSCH协议
													//缺省过滤条件 无
	#define PROTOCOL_CCD					0x08	//CCD协议
	//#define PROTOCOL_CCD					0x07	//OLD E协议

#ifndef NEW_DRV
	class CFilterOrLengthRecognise
	{
	public:
		CFilterOrLengthRecognise();

	public:
		void Empty();
		void SetNormalFilterMode(short iStartPosition, short iFilterSizeLength, char *chFilterContain);
		void SetCanFilterMode(short iFilterPosition,short iFilterMask,short iLengthPosition);
		void AddCanFilterId(CBinary binCanFilterId);
		void SetNormalLengthRecogniseMode(short iBytePosition, unsigned char ucMark, short iAddValue);
		void SetSpecifiedLengthMode();

	private:
		short m_iLenth;
		short m_iNum;
		char m_chBuffer[300];

	public:
		short GetLenth();
		short GetNum();
		char *GetBuffer();

	};
#endif
/*
	PROTOCOL_NORMAL		// 一般格式收发协议;	缺省过滤条件: 无
	PROTOCOL_KWP		// KWP协议;				缺省过滤条件: 无
	PROTOCOL_ISO		// ISO协议;				缺省过滤条件: 无
	PROTOCOL_VPW		// VPW协议;				缺省过滤条件: 起始位置0, 长度2, 字节0x486B
	PROTOCOL_PWM		// PWM协议;				缺省过滤条件 起始位置0, 长度2, 字节0x416B
	PROTOCOL_CANBUS		// CANBUS协议;			缺省过滤条件 无
	PROTOCOL_CAN_SINGLE_LINE	// CANBUS协议, 单线通信;			缺省过滤条件 无
	PROTOCOL_BOSCH		// BOSCH协议;			缺省过滤条件 无
*/
	short ProtocolSet (short iProtocolWay, CFilterOrLengthRecognise *pFilterOrLengthRecognise = NULL,unsigned char bFlag = 0);
	short SetProtocol (short iProtocolWay);//for K-CAN  for VolVo
	short ControlKLine (short iEnable);//for K-CAN  for VolVo
	short SetVWCanSpecial(short iEnable);//for VW CAN
	short Set24VSpecial(short iEnable);//for 24V add by scf 2009.12.01

	short SetBenzSpecial(short iEnable);
	short SetHoldonSpecial(short iFilterByte);

	// 发送数据给ECU接收应答
	CReceiveFrame SendReceive (CSendFrame& SendFrame, bool bRepeat = false);
	CReceiveFrame CanSendReceive (CSendFrame& SendFrame, bool bRepeat = false);
  CReceiveFrame CumSendReceive (CSendFrame& SendFrame, bool bRepeat = false);
 
	// 接收ECU应答数据
	//CReceiveFrame CCommWithEcu::ReceiveOnly (bool bClearBuffer=false);
    CReceiveFrame ReceiveOnly (bool bClearBuffer=false);

	// 设置下位机自动应答
	/*****************************************************************************
	  功    能：设置下位机自运应答ECU数据 
	  参数说明：CFilterOrLengthRecognise flrFilterCondition 从ECU接收到的需要自动应
															答的数据帧条件；
				CSendFrame sfSendFrame 回复给ECU的发送帧
	  返 回 值：错误代码
	  说    明：如需要停止自动应答，则发送帧数设置为0即可
				注意：如回复ECU后，如ECU不需要再回答该帧，sfSendFrame中的接收帧数
					  应设为0
	*****************************************************************************/
	short AutoAnswer (CFilterOrLengthRecognise flrFilterCondition, CSendFrame sfSendToEcu);
	

	/*****************************************************************************
	  功    能：设置下位机自动发送的续发帧 
	  参数说明：CFilterOrLengthRecognise flrFilterCondition 从ECU接收到的需要自动应
															答的数据帧条件；
				CSendFrame sfSendContinuedFrame 回复给ECU的续发帧
	  返 回 值：错误代码
	  说    明：如需要停止自动应答续发帧，则发送帧数设置为0即可
				注意：如回复ECU后，如ECU不需要再回答该帧，sfSendContinuedFrame中的接收帧数
					  应设为0
	*****************************************************************************/
	short SetAutoSendContinuedFrame (CFilterOrLengthRecognise flrFilterCondition, CSendFrame sfSendContinuedFrame);

	#define QF_TOYOTA													0x0001
	/********************************************************************
	  功    能：读快速闪光码
	  参数说明：unsigned short ui16Type  快速码类型
				   取值范围目前只支持：QF_TOYOTA----丰田闪光码
				bool bRepeat  true--下位机不断读取向上送 false--仅读取一次
	  返 回 值：ECU  回复帧数据，
				参数QF_TOYOTA(丰田闪光码)回复格式如下：
				第一个下标下的：
					第一个CBinary内容为ID(正常长度为2个字节，前12bit有效),
					第二个CBinary内容为闪光码内容(正常情况下为12个字节)
	  说    明：如果bRepeat为true, 读取闪光码后，要求使用命令打断下位机
	********************************************************************/
	//CReceiveFrame CCommWithEcu::QuickFlashCode(unsigned short uiFlashType, bool bRepeat);
    CReceiveFrame QuickFlashCode(unsigned short uiFlashType, bool bRepeat);
	// 读闪光码
	/********************************************************************
	  功    能：读闪光码
	  参数说明：W_INT32 iBitMaxInterval 数据位(bit)脉冲间最大间隔时间；
							要求不小于最大实际时间间隔，单位：毫秒	
  				int iByteMaxInterval 数据值(digit)间最小间隔时间，
							如个位十位之间，十位百位之间的时间间隔；
							要求不小于最大实际时间间隔，单位：毫秒
				int iCodeMaxInterval 闪光码间最大间隔时间
							要求不小于最大实际时间间隔，单位：毫秒
							此参数用于闪光码读取超时
				int iMaxCodeTime 闪烁一个完整的最大的闪光码所占用时间
							要求不小于最大实际使用时间，单位：毫秒
	  返 回 值：闪光码内容
	  说    明：1、每次调会收到一个闪光码，即诊断程序员第一调本函数时，
				   发送参数，之后每次再调本函数时，只是从缓冲区中接收一个
				   闪光码包，直至新命令打断。
				2、如收到的闪光码长度为0表示出错/新命令打断退出
				3、上位机接收最大等待时间不应小于如下值：
					  CodeMaxInterval+iMaxCodeTime
	********************************************************************/
	CBinary FlashCode ( unsigned int uiBitMaxInterval, 
					 unsigned int uiByteMaxInterval, 
					 unsigned int uiCodeMaxInterval = 6000, 
					 unsigned int uiMaxCodeTime = 12000,
					 unsigned char uiFlashType = 1);

	// 等待直到ECU保持给定电平时间到
	#define WVT_HIGH_VOLTAGE												0x02
	#define WVT_LOW_VOLTAGE													0x01
	/********************************************************************
	  功    能：等待，直到接收电平不变化的时间达到某一给定数据后退出
	  参数说明：W_UINT32 uiVoltageKeepTime 电平无变化保持的时间
				   unsigned int uiOverTime 等待最大超时时间
				   short VoltageLevelStatus 要求的电平, 
						   高电平：WVT_HIGH_VOLTAGE
						   低电平：WVT_LOW_VOLTAGE
						缺省为：WVT_HIGH_VOLTAGE|WVT_LOW_VOLTAGE，即没有电平要求
	  返 回 值：错误代码
	  说    明：上位机最大等待时间一定要大于最大超时时间－－uiOverTime
				成功否需判断返回值，如下位机无应答则抛出异常
	********************************************************************/
	short WaitVoltageHighLowTime ( unsigned int uiVoltageKeepTime, 
									 unsigned int uiOverTime, 
									 unsigned short VoltageLevelStatus = WVT_HIGH_VOLTAGE|WVT_LOW_VOLTAGE
									);

	short InitAndResetCOMM(void);

#ifdef NEW_DRV
	virtual void SetSendFrameCtlCmd(SF_CTRCMD cmd){
		m_ctlCmd = cmd;
	}
	SF_CTRCMD m_ctlCmd;
// 	virtual bool ContinueReceive(CSendFrame& sendFrm);
#endif

};

typedef struct{
	unsigned char  statusOfPLL;   //0:没有变过;1:部分变过;2:链路全重新设置过
	//bool CCheckPLLFunc::CheckAndSetCommPacketMaxTime(void){
	unsigned int CommPacketMaxTimeP1;
	//bool CCheckPLLFunc::CheckAndSetIoPort(void){		
	unsigned long			ProtocolID;
	unsigned char			SetIoPortP1;
	unsigned char			SetIoPortP2;
	char			SetIoPortP3;
	unsigned short		SetIoPortP4;
	
	unsigned char			EnableOutputIoLineP1;
	bool			EnableOutputIoLineP2;
	
	vector<unsigned char>	EnableOutputIoLineGP1;
	vector<bool>	EnableOutputIoLineGP2;
	vector<short>	VoltageHighLowTimeGP1;
	vector<short>	VoltageHighLowTimeGP2;
	//bool CCheckPLLFunc::CheckAndSetBps(void){
	unsigned char			SetBRsP1;
	unsigned char			SetBRsP2;
	char			SetBRsP3;
	char			SetBRsP4;
	
	double			SetBpsP1;
	char			SetBpsP2;
	char			SetBpsP3;
	//bool CCheckPLLFunc::CheckAndSetTimeIntervalOver(void){
	int			TimeIntervalOverP1;
	int			TimeIntervalOverP2;
	double			TimeIntervalOverP3;
	//bool CCheckPLLFunc::CheckAndSetAddressCodeEnter(void){
	unsigned char			BoschEnterP1;
	short			BoschEnterP2;
	
	unsigned char			AddressCodeEnterP1;
	unsigned short		AddressCodeEnterP2;
	double			AddressCodeEnterP3;
	short			AddressCodeEnterP4;
	//bool CCheckPLLFunc::CheckAndProtocolSet(void){		
	unsigned char			SetHighSpeedP1;
	
	short			ProtocolSetP1;
#ifndef NEW_DRV
	CCommWithEcu::CFilterOrLengthRecognise		ProtocolSetP2;  // 	CCommWithEcu::CFilterOrLengthRecognise		ProtocolSetP2;
	CCommWithEcu::CFilterOrLengthRecognise	CanSetFlowCtrlFrameP2;// 	CCommWithEcu::CFilterOrLengthRecognise
	CCommWithEcu::CFilterOrLengthRecognise		AddCanFilterIDP2;// 	CCommWithEcu::CFilterOrLengthRecognise
#else
	CCommInterFace::CFilterOrLengthRecognise ProtocolSetP2;  // 	CCommWithEcu::CFilterOrLengthRecognise		ProtocolSetP2;
	CCommInterFace::CFilterOrLengthRecognise		AddCanFilterIDP2;  // CCommWithEcu::CFilterOrLengthRecognise
	CCommInterFace::CFilterOrLengthRecognise	CanSetFlowCtrlFrameP2;// 	CCommWithEcu::CFilterOrLengthRecognise
#endif	
	short		ControlKLineP1;
	
	//TP_BTs		AddCanFilterIDP1;
	
	short		SetNormalLengthRecogniseModeP1;
	unsigned char		SetNormalLengthRecogniseModeP2;
	short		SetNormalLengthRecogniseModeP3;
	
	short		 SetNormalFilterModeP1;
	short		 SetNormalFilterModeP2;
	vector<char> SetNormalFilterModeP3;
	//bool CCheckPLLFunc::CheckAndSetAutoSendContinuedFrame(void){
	short		SetCanFilterModeP1;
	unsigned char		SetCanFilterModeP2;
	short		SetCanFilterModeP3;
	
	unsigned long		CanSetFlowCtrlFrameP1;
	//KeepLink cmd's address.
	unsigned short		keepLinkTime;
	unsigned long		keepLinkCmdAddr;
} StLinkLayerParas;


#endif	//__COMMWITHECU_H_
