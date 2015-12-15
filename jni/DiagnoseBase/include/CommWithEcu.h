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
		N0_CYCLE     = 0,  // ��ѭ��
		SINGLE_CYCLE = 1,  // ��֡ѭ��
		MULTI_CYCLE  = 2,  // ��֡ѭ��
		FLASH_CYCLE  = 3,   // ������ѭ��
		QUICK_FLASH_CYCLE = 4 //�ɿ���������ѭ��
	};
	
	// ���ݽ��շ��;��
	CSendReceivePacket *m_SendRecv;

	bool m_bAssembledFlag;		// �Ϸ��ͱ�ʶ��ִ��Begin()Ϊtrue, ִ��End()Ϊfalse 
	int m_iEcuNum;                 // ��Ϸ����У���ECUͨ�ŵ��������

	bool m_bRepeating;			// ѭ�����ͱ�־

#ifdef DATA_DRV_EXT

	/************************************************************************/
	/* Bin ��� RunLog����                                                  */
	/************************************************************************/

	CRunLog* m_runLog;
#endif
	unsigned char*	m_SendCommBuf;			// ����ͨ�Ż�����
	unsigned char*	m_RecieveCommBuf;	    // ����ͨ�Ż�����

	unsigned char*	m_pchBuf;		// ָ��ǰͨ�Ż�������ƫ��λ��

	typedef struct _tagAssembleChain
	{
		int		retType;		//����ֵ���ͣ�1-short(��������)��2-CReceiveFrame(������������)
		void*	inValueAddres;	//����ֵ�ĵ�ַָ�룺��������3���������ͣ���Ҫ��������ֵ����Ϣ�����շ���ֵ������
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

	// ��������λ��������, ����ͨ�Ż�����
	void Init (void);
#ifdef DATA_DRV_EXT
	/************************************************************************/
	/* Bin ����RunLog													     */
	/************************************************************************/

	void SetRunLog(CRunLog* log){m_runLog = log;};
	CRunLog* GetRunLog(void){return m_runLog;};
#endif
	// ��������λ��������
	void Destroy (void);

	// ִ�д˺�����ִ�еĸ���λ�������ݻ����ͬһ���ݰ��У�ֱ��ִ��End()�ŷ���Ϊֹ
	void Begin (void);

	// �˺������ʹ�Begin()������ʼ������������λ������ȡ��Ӧ��
	CReceiveFrame End (void);

	// ���õȴ���λ���ظ������ʱ�䣬��λ����
	void WaitCommPacketMaxTime (unsigned int iMaxTime);

	// ������������˿ڼ�ͨ�Ų���(0xEFΪ��Ч����)
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


	#define DB15_CANBUS														  0x86	//Ϊ���ݶ�������������DB15_CAN_PIN6_14�����

	//�����ҽ�����GetAdcStatus(),GetChannelVoltage()��������
	#define DB15_NO_SWITCH_CHANNEL											  0xFF
	
	#define IOS_INPUT_POSITIVE_LOGIC         0x01
	#define IOS_INPUT_NEGATIVE_LOGIC         0x00

	#define IOS_OUTPUT_POSITIVE_LOGIC        0x02
	#define IOS_OUTPUT_NEGATIVE_LOGIC        0x00
	
	#define IOS_PULL						 0x00 //��Ϊ��ǰһ�汾���ݣ�����ʵ������
	#define IOS_INPUT_PULL_UP				 0x40 //�ղ���������������2006.7.19
	#define IOS_OUTPUT_REVERSE				 0x80

	short SetIoPort (	unsigned char iIoOutputPort,
						unsigned char iIoInputPort, 						
						char iOutputVoltage = 12,
						unsigned short ui16IoSelectItem = IOS_INPUT_POSITIVE_LOGIC | IOS_OUTPUT_POSITIVE_LOGIC);

	// ������������˿�ѡͨ��Ͽ�
	short EnableOutputIoLine (unsigned char iIoOutputPort, bool bIsConnect = true);

	//PB_  party bit 
	#define PB_SPACE														4 //0У��
	#define PB_MARK															3 //1У��
	#define PB_EVEN															2 //żУ��
	#define PB_ODD															1 //��У��
	#define PB_NONE															0 //��У��
	// ������ECUͨ�Ų����ʼ�λ��ʽ
	short SetBps (double fBps, char iParityBit = PB_NONE, char iDataBit = 8);
	short SetBRs (unsigned char BR1,unsigned char BR2,char iParityBit = PB_NONE, char iDataBit = 8);

	// ������ECUͨ��ʱ�����
	short TimeIntervalOver (int iMaxWaitReceivePacket,
							  int iSendPacketMinWaitTime,
							  double fSendByteMinWaitTime,
							  int iNegativePacketWaitTime=0);

	// ������·����
	short KeepLink (short iKeepTime = 0,  CSendFrame *pKeepLinkCommand = NULL);

	// ����BOSCH��·����
	short BoschKeepLink (bool bEnableKeep = false);

	// ���������ƽ�ߵ�ʱ�䣬ʡ�ԵĲ�������Ϊunsigned short����
	short VoltageHighLowTime (short iHighLowTimes, ...);
	// ���������ƽ�ߵ�ʱ�䣬ʡ�ԵĲ�������Ϊunsigned short����, added by bin for data driven
	short VoltageHighLowTime (short count,vector<short>& Times);

	//add by scf 2008.07.18
	// ���������ƽ�ߵ�ʱ�䣬ʡ�ԵĲ�������Ϊunsigned short����
	short VoltageHighLowTenthTime (short iHighLowTimes, ...);

	
	//���ø���ͨ�ŷ�ʽ 0 ����  1 ���� ע�����˳����ٵ���Ϸ�ʽʱһ��Ҫ��ԭΪ��������SetHighSpeed(0);
	short SetHighSpeed(unsigned char pParameter);
	
	//����BMW���ⷽʽ(������֮ǰ�ȸߵ͵�ƽ����)ͨ�ŷ�ʽ������ 0 ��Ч  1 ��Ч ע�����˳�ʱһ��Ҫ��ԭΪ��Ч����SetBMWSpecial(0);
	short SetBMWSpecial(unsigned char pParamerer);
	
	//����CAN ��Ӳ������ģʽ
	short SetCanHardMaskFilter(unsigned char pParamerer);

	//��������Ӧ��Ĵ���ģʽ
	//short SetNegativeHandleMethod(unsigned char iPosition1, unsigned char iNegByte, unsigned char iPosition2, unsigned char iNegParameter, unsigned char iHandleType);//hpy modified 20131010
	short SetNegativeHandleMethod(unsigned char iPosition1, unsigned char iNegByte, unsigned char iPosition2, unsigned char iNegParameter, unsigned char iHandleType, unsigned short iMaxCnt = 100);

	// ��ַ�����ϵͳ��ʡ�ԵĲ�������Ϊunsigned short����
	#define ACE_BPS_AUTO_RECOGNIZE           0x01
	#define ACE_KW2_REVERSE_TO_ECU           0x02
	#define ACE_GET_ADDRESS_REVERSE_FROM_ECU 0x04
	#define ACE_RECEIVE_KW5					 0x08
	#define ACE_RECEIVE_ONE_FRAME			 0x10
	#define ACE_KW1_REVERSE_TO_ECU			 0x20
	#define ACE_RECEIVE_N_FRAME				 0x40
	#define ACE_CLOSE_TRIGGER_CHANNEL		 0x80

    /********************************************************************
      ��    �ܣ���ַ�����ϵͳ
      ����˵����unsigned char ucAddressCode ��ַ��
	            unsigned short ui16Parameter ��ַ�����ѡ�ȡֵ��ΧΪ���µ����
                  ACE_BPS_AUTO_RECOGNIZE           -- �Զ�ʶ������
                  ACE_KW2_REVERSE_TO_ECU           -- ��KW2ȡ����ECU
                  ACE_GET_ADDRESS_REVERSE_FROM_ECU -- ��ECU���յ�ַ��ȡ��ֵ
                  ACE_RECEIVE_KW5                  -- ����5���ֽ�KeyWord, 
				                                      ��δѡ�б��������2��KW�ֽ�
                  ACE_RECEIVE_ONE_FRAME            -- ��ECU����һ֡����
                  ACE_KW1_REVERSE_TO_ECU           -- �ظ�KW1ȡ����ECU
                  ACE_RECEIVE_N_FRAME              -- ���ն�֡
                  ACE_CLOSE_TRIGGER_CHANNEL        -- �رմ����ź�ͨ��
                double fBps  ����ַ��Ĳ�����(bps)
                short i16SendKw2TimeInterval  KW2ȡ�����ص�ʱ����(mS)
      �� �� ֵ��ECU  �ظ�֡����
      ˵    ������
    ********************************************************************/
	CReceiveFrame AddressCodeEnter (unsigned char ucAddressCode, 									
									unsigned short ui16Parameter = ACE_BPS_AUTO_RECOGNIZE | ACE_KW2_REVERSE_TO_ECU | ACE_GET_ADDRESS_REVERSE_FROM_ECU,
									double fBps = 5,
									short i16ReverseKw2TimeInterval = 30);


	/********************************************************************
	  ��    �ܣ�BOSCH��ַ�����ϵͳ
	  ����˵����unsigned char ucAddressCode  ��ַ��
				short i16SendKw2TimeInterval  KW2ȡ�����ص�ʱ����(mS)
	  �� �� ֵ��ECU�ظ��İ汾��Ϣ
	  ˵    ������
	********************************************************************/
    //CReceiveFrame CCommWithEcu::BoschEnter (unsigned char ucAddressCode,	short i16ReverseKw2TimeInterval = 30);
    CReceiveFrame BoschEnter (unsigned char ucAddressCode,	short i16ReverseKw2TimeInterval = 30);
	// ȡ����λ��״̬, �����ڷ���Ϸ���
	//GS_ Get Status
	#define GS_CHANNEL_SIGNAL											  0x00
	#define GS_CHANNEL_VOLTAGE											  0x01
	short GetStatus(unsigned char ui8StatusWay, unsigned char ucIoInputPort = DB15_NO_SWITCH_CHANNEL);


	// ������ECUͨ��Э�鼰����
	#define PROTOCOL_NORMAL					0x00	// һ���ʽ�շ�Э��
													//ȱʡ�������� ��

	#define	PROTOCOL_KWP					0x01	// KWPЭ��
													//ȱʡ�������� ��

	#define	PROTOCOL_ISO					0x02	// ISOЭ��
													//ȱʡ�������� ��

	#define	PROTOCOL_VPW					0x03	// VPWЭ��
													//ȱʡ�������� ��ʼλ��0, ����2, �ֽ�0x486B

	#define	PROTOCOL_PWM					0x04	// PWMЭ��
													//ȱʡ�������� ��ʼλ��0, ����2, �ֽ�0x416B

	#define	PROTOCOL_CANBUS					0x05	// CANBUSЭ��
	#define	PROTOCOL_CAN_SINGLE_LINE		0x85	// CANBUSЭ��, ����ͨ��
													//ȱʡ�������� ��

	#define	PROTOCOL_BOSCH					0x06	// BOSCHЭ��
													//ȱʡ�������� ��
	#define PROTOCOL_CCD					0x08	//CCDЭ��
	//#define PROTOCOL_CCD					0x07	//OLD EЭ��

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
	PROTOCOL_NORMAL		// һ���ʽ�շ�Э��;	ȱʡ��������: ��
	PROTOCOL_KWP		// KWPЭ��;				ȱʡ��������: ��
	PROTOCOL_ISO		// ISOЭ��;				ȱʡ��������: ��
	PROTOCOL_VPW		// VPWЭ��;				ȱʡ��������: ��ʼλ��0, ����2, �ֽ�0x486B
	PROTOCOL_PWM		// PWMЭ��;				ȱʡ�������� ��ʼλ��0, ����2, �ֽ�0x416B
	PROTOCOL_CANBUS		// CANBUSЭ��;			ȱʡ�������� ��
	PROTOCOL_CAN_SINGLE_LINE	// CANBUSЭ��, ����ͨ��;			ȱʡ�������� ��
	PROTOCOL_BOSCH		// BOSCHЭ��;			ȱʡ�������� ��
*/
	short ProtocolSet (short iProtocolWay, CFilterOrLengthRecognise *pFilterOrLengthRecognise = NULL,unsigned char bFlag = 0);
	short SetProtocol (short iProtocolWay);//for K-CAN  for VolVo
	short ControlKLine (short iEnable);//for K-CAN  for VolVo
	short SetVWCanSpecial(short iEnable);//for VW CAN
	short Set24VSpecial(short iEnable);//for 24V add by scf 2009.12.01

	short SetBenzSpecial(short iEnable);
	short SetHoldonSpecial(short iFilterByte);

	// �������ݸ�ECU����Ӧ��
	CReceiveFrame SendReceive (CSendFrame& SendFrame, bool bRepeat = false);
	CReceiveFrame CanSendReceive (CSendFrame& SendFrame, bool bRepeat = false);
  CReceiveFrame CumSendReceive (CSendFrame& SendFrame, bool bRepeat = false);
 
	// ����ECUӦ������
	//CReceiveFrame CCommWithEcu::ReceiveOnly (bool bClearBuffer=false);
    CReceiveFrame ReceiveOnly (bool bClearBuffer=false);

	// ������λ���Զ�Ӧ��
	/*****************************************************************************
	  ��    �ܣ�������λ������Ӧ��ECU���� 
	  ����˵����CFilterOrLengthRecognise flrFilterCondition ��ECU���յ�����Ҫ�Զ�Ӧ
															�������֡������
				CSendFrame sfSendFrame �ظ���ECU�ķ���֡
	  �� �� ֵ���������
	  ˵    ��������Ҫֹͣ�Զ�Ӧ������֡������Ϊ0����
				ע�⣺��ظ�ECU����ECU����Ҫ�ٻش��֡��sfSendFrame�еĽ���֡��
					  Ӧ��Ϊ0
	*****************************************************************************/
	short AutoAnswer (CFilterOrLengthRecognise flrFilterCondition, CSendFrame sfSendToEcu);
	

	/*****************************************************************************
	  ��    �ܣ�������λ���Զ����͵�����֡ 
	  ����˵����CFilterOrLengthRecognise flrFilterCondition ��ECU���յ�����Ҫ�Զ�Ӧ
															�������֡������
				CSendFrame sfSendContinuedFrame �ظ���ECU������֡
	  �� �� ֵ���������
	  ˵    ��������Ҫֹͣ�Զ�Ӧ������֡������֡������Ϊ0����
				ע�⣺��ظ�ECU����ECU����Ҫ�ٻش��֡��sfSendContinuedFrame�еĽ���֡��
					  Ӧ��Ϊ0
	*****************************************************************************/
	short SetAutoSendContinuedFrame (CFilterOrLengthRecognise flrFilterCondition, CSendFrame sfSendContinuedFrame);

	#define QF_TOYOTA													0x0001
	/********************************************************************
	  ��    �ܣ�������������
	  ����˵����unsigned short ui16Type  ����������
				   ȡֵ��ΧĿǰֻ֧�֣�QF_TOYOTA----����������
				bool bRepeat  true--��λ�����϶�ȡ������ false--����ȡһ��
	  �� �� ֵ��ECU  �ظ�֡���ݣ�
				����QF_TOYOTA(����������)�ظ���ʽ���£�
				��һ���±��µģ�
					��һ��CBinary����ΪID(��������Ϊ2���ֽڣ�ǰ12bit��Ч),
					�ڶ���CBinary����Ϊ����������(���������Ϊ12���ֽ�)
	  ˵    �������bRepeatΪtrue, ��ȡ�������Ҫ��ʹ����������λ��
	********************************************************************/
	//CReceiveFrame CCommWithEcu::QuickFlashCode(unsigned short uiFlashType, bool bRepeat);
    CReceiveFrame QuickFlashCode(unsigned short uiFlashType, bool bRepeat);
	// ��������
	/********************************************************************
	  ��    �ܣ���������
	  ����˵����W_INT32 iBitMaxInterval ����λ(bit)����������ʱ�䣻
							Ҫ��С�����ʵ��ʱ��������λ������	
  				int iByteMaxInterval ����ֵ(digit)����С���ʱ�䣬
							���λʮλ֮�䣬ʮλ��λ֮���ʱ������
							Ҫ��С�����ʵ��ʱ��������λ������
				int iCodeMaxInterval ������������ʱ��
							Ҫ��С�����ʵ��ʱ��������λ������
							�˲��������������ȡ��ʱ
				int iMaxCodeTime ��˸һ��������������������ռ��ʱ��
							Ҫ��С�����ʵ��ʹ��ʱ�䣬��λ������
	  �� �� ֵ������������
	  ˵    ����1��ÿ�ε����յ�һ�������룬����ϳ���Ա��һ��������ʱ��
				   ���Ͳ�����֮��ÿ���ٵ�������ʱ��ֻ�Ǵӻ������н���һ��
				   ���������ֱ���������ϡ�
				2�����յ��������볤��Ϊ0��ʾ����/���������˳�
				3����λ���������ȴ�ʱ�䲻ӦС������ֵ��
					  CodeMaxInterval+iMaxCodeTime
	********************************************************************/
	CBinary FlashCode ( unsigned int uiBitMaxInterval, 
					 unsigned int uiByteMaxInterval, 
					 unsigned int uiCodeMaxInterval = 6000, 
					 unsigned int uiMaxCodeTime = 12000,
					 unsigned char uiFlashType = 1);

	// �ȴ�ֱ��ECU���ָ�����ƽʱ�䵽
	#define WVT_HIGH_VOLTAGE												0x02
	#define WVT_LOW_VOLTAGE													0x01
	/********************************************************************
	  ��    �ܣ��ȴ���ֱ�����յ�ƽ���仯��ʱ��ﵽĳһ�������ݺ��˳�
	  ����˵����W_UINT32 uiVoltageKeepTime ��ƽ�ޱ仯���ֵ�ʱ��
				   unsigned int uiOverTime �ȴ����ʱʱ��
				   short VoltageLevelStatus Ҫ��ĵ�ƽ, 
						   �ߵ�ƽ��WVT_HIGH_VOLTAGE
						   �͵�ƽ��WVT_LOW_VOLTAGE
						ȱʡΪ��WVT_HIGH_VOLTAGE|WVT_LOW_VOLTAGE����û�е�ƽҪ��
	  �� �� ֵ���������
	  ˵    ������λ�����ȴ�ʱ��һ��Ҫ�������ʱʱ�䣭��uiOverTime
				�ɹ������жϷ���ֵ������λ����Ӧ�����׳��쳣
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
	unsigned char  statusOfPLL;   //0:û�б��;1:���ֱ��;2:��·ȫ�������ù�
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
