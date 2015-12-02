/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�	ActiveTestShow.cpp
	�ĵ���ʶ��	007�������ƽ̨��ϸ���˵����
	ժ    Ҫ��	��ʾ��ĳ�Ա������������ɶ���������ʾ����
	��    �ܣ�
				1��	��������������Ϣ����ť��Ϣ���涨��ʽд�뻺������֪ͨ��ʾ����ȡ�����ݲ���ʾ��
				2��	������ʾ����������û�������

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			�������ࡣ                                           

**************************************************************************************/
#ifdef WIN32
#include "windows.h"
#endif

#include "arithmetic.h"
#include "ActiveTestShow.h"
#include "Database.h"
#include "assert.h"
#include "display.h"

#define INTERVAL_TIME 50


//����ID���ı��ⷵ���ַ���
extern string adsGetTextString (CBinary binTextID);
//extern CArithmetic::SetDebugState (unsigned char uiDebugItem, bool bEnable);


CActiveTestShow::CActiveTestShow()
{
	m_iButtonNum = 0;
	m_iHeaderNum = 0;
	m_iAllHeaderTextLen = 0;
	m_iTitleLenth       = 0;     // �������Ա��ⳤ��
	m_iDataStreamFormat = -1;    // ��������ʽ
	m_bAddDataStream    = false; // �Ƿ��Ѿ��������������
}

/***************************************************************************************
��    �ܣ���ʼ����������
����˵����char* pTitle �������Ա��⣬��Ϊ����ʾ����ʹ��ȱʡ����
�� �� ֵ����
˵    ������Ϊδ���붯�����԰�ť����Ҫ���AddButton ()����һ��ʹ��
**************************************************************************************/
void CActiveTestShow::Init (const char* pTitle)
{
	assert(NULL != pTitle);

	// ��ť������0
	m_iButtonNum = 0;

	// ����������0
	m_iHeaderNum = 0;
	m_iAllHeaderTextLen = 0;

	// ��������ʽ��λ
	m_iDataStreamFormat = -1;

	// �Ƿ��Ѿ��������������
	m_bAddDataStream    = false; 


	m_strMsg = "";

	m_iTitleLenth = strlen(pTitle)+1;  // ������ⳤ��

	char chFuction = DT_ACTIVE_TEST;
	m_pSendAndReceive->Init(chFuction);  // ��书�ܴ���͵��Ա�־

	// ��ť����
	short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	// �������Ա���
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
}

/***************************************************************************************
��    �ܣ���ʼ����������
����˵����string strTitle �������Ա��⣬
�� �� ֵ����
˵    ������Ϊδ���붯�����԰�ť����Ҫ���AddButton ()����һ��ʹ��
**************************************************************************************/
void CActiveTestShow::Init (string strTitle)
{	
	// �������غ���
	Init(strTitle.c_str());
}

/***************************************************************************************
��    �ܣ���ʼ����������
����˵����CBinary idTitle �������Ա��⣬
�� �� ֵ����
˵    ������Ϊδ���붯�����԰�ť����Ҫ���AddButton ()����һ��ʹ��
**************************************************************************************/
void CActiveTestShow::Init (CBinary idTitle)
{
	// ����IDת�����ַ���
	string strTitle = adsGetTextString(idTitle);

	// �������غ���
	Init(strTitle);
}


/***************************************************************************************
��    �ܣ���Ӷ������Խ��水ť
����˵����CBinary idButtonText ��ӵİ�ť���⣬
�� �� ֵ���������
˵    ������
**************************************************************************************/
short CActiveTestShow::AddButton (CBinary idButtonText, char byStatus)
{
	assert(!m_bAddDataStream);

	assert(ID_BINARY_LENTH == idButtonText.GetSize());

	// ��ť�ı�ID��
	m_pSendAndReceive->Add(idButtonText);

	// ��ť״̬
	m_pSendAndReceive->Add(byStatus);

	// ������Ŧ����
	m_pSendAndReceive->IncreaseNum(4);
	
	// ��¼��Ŧ����
	m_iButtonNum++;

	return true;
}

/***************************************************************************************
��    �ܣ����ö������Եı�־λ
����˵����iFlag 0xFF��ʾ�˶�������֧�����⹦��

0x01: ��������˵�����ı���
0x02: ֧���Զ�����ͷ
0x03: ���У�����ͷ
0xFF: ֧����굥��
�� �� ֵ��������룬�ɹ�Ϊ1
˵    ����
**************************************************************************************/
bool CActiveTestShow::SetFlag(unsigned char byFlag)
{
	unsigned char byTmp = m_pSendAndReceive->GetFlag();//hpy modified 20131216
	byTmp = byTmp | byFlag;
	m_pSendAndReceive->SetFlag(byTmp);
	return true;

//	m_pSendAndReceive->SetFlag(byFlag);
//	return true;
}

/***************************************************************************************
��    �ܣ���Ӷ�ѡ���˵��
����˵���������ı�
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������ add by scf 2008\2\29
**************************************************************************************/
short CActiveTestShow::AddMsg(string strMsg)
{
	//m_pSendAndReceive->Add(strMsg);
	m_strMsg = strMsg;
	return 0;
}


/***************************************************************************************
��    �ܣ���Ӷ�ѡ����ͷ������˵��
����˵���������ı�
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������ add by scf 2008\10\31,
ÿ���һ�����Ŀ++��ͬʱ����ģʽΪ�Զ���ͷģʽ
**************************************************************************************/

short CActiveTestShow::AddHeaderText(string strHeaderMsg)
{
	//m_pSendAndReceive->SetFlag(0x02);
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum;
	if(m_iHeaderNum == 0)
		m_pSendAndReceive->Add(m_iHeaderNum);
	
	m_iHeaderNum++;
	m_pSendAndReceive->Add(strHeaderMsg);
	m_iAllHeaderTextLen += strlen(strHeaderMsg.c_str())+1;
	m_pSendAndReceive->IncreaseNum(iNum);
	return m_iAllHeaderTextLen;
}


/***************************************************************************************
��    �ܣ����������������
����˵����CBinary idDataStream ������ID
		  string strDataStreamValue ��������ֵ
�� �� ֵ��������룬�ɹ�Ϊ0
˵    �����ڵ�һ���������ֵʱд���ʽ�ֽڣ�������������ݸ�ʽ���һ�β�ͬ���ܼ��벢����
**************************************************************************************/
short CActiveTestShow::Add (CBinary idDataStream, string strDataStreamValue)
{
	assert(BDF_VW != m_iDataStreamFormat);

//	if(BDF_VW == m_iDataStreamFormat)
//	{
//		return EC_TYPE_DATA_STREAM;  // ��������ʽ��һ��
//	}
	
	// һ��������
	m_iDataStreamFormat = BDF_NOMAL;

	// ����ǵ�һ���������������
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;
		
//		// ���㰴ťѡ�������ֽ���
//		short iByteNum = (m_iButtonNum-1)/8 +1;		
//		char chMask = 0x00;		
//		for(short iIndex = 0; iIndex < iByteNum; iIndex++)
//		{
//			m_pSendAndReceive->Add(chMask);
//		}

		// ��������ʽ
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// ����������
		short iNum = 0;
		m_pSendAndReceive->Add(iNum);
	}

	// ������ID
	m_pSendAndReceive->Add(idDataStream);

	// ��������ֵ��
	m_pSendAndReceive->Add(strDataStreamValue);

	// ��������������  ����(2�ֽ�) + ���ܴ���(1�ֽ�) + ���Ա�־(1�ֽ�) + ��ť����(2�ֽ�) + ���ⳤ�� + ��Ŧ�ı�ID + ��Ŧ״̬ + ���������ݸ�ʽ(1�ֽ�)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	if(m_iHeaderNum>0)
		    iNum = 2 + 2 + 2 + m_iTitleLenth + 2 + m_iAllHeaderTextLen + 6*m_iButtonNum + m_iButtonNum + 1;

	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}

/***************************************************************************************
��    �ܣ����������������
����˵����CBinary idDataStream ������ID
		  string strDataStreamValue ��������ֵ
		  CBinary idUnit ��������λID
�� �� ֵ��������룬�ɹ�Ϊ0
˵    �����ڵ�һ���������ֵʱд���ʽ�ֽڣ�������������ݸ�ʽ���һ�β�ͬ���ܼ��벢����
**************************************************************************************/
short CActiveTestShow::Add (CBinary idDataStream, string strDataStreamValue, CBinary idUnit)
{
	assert(BDF_NOMAL != m_iDataStreamFormat);

//	if(BDF_NOMAL == m_iDataStreamFormat)
//	{
//		return EC_TYPE_DATA_STREAM;  // ��������ʽ��һ��
//	}
	
	// ��λ������������
	m_iDataStreamFormat = BDF_VW;
	
	// ����ǵ�һ���������������
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;
		
//		// ���㰴ťѡ�������ֽ���
//		short iByteNum = (m_iButtonNum-1)/8 +1;		
//		char chMask = 0x00;		
//		for(short iIndex = 0; iIndex < iByteNum; iIndex++)
//		{
//			m_pSendAndReceive->Add(chMask);
//		}

		// ��������ʽ
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// ����������
		short iNum = 0;
		m_pSendAndReceive->Add(iNum);
	}
	

	// ������ID
	m_pSendAndReceive->Add(idDataStream);

	// ��������λID
	m_pSendAndReceive->Add(idUnit);

	// ��������ֵ��
	m_pSendAndReceive->Add(strDataStreamValue);

	// ��������������  ����(2�ֽ�) + ���ܴ���(1�ֽ�) + ���Ա�־(1�ֽ�) + ��ť����(2�ֽ�) + ���ⳤ�� + ��Ŧ�ı�ID + ��Ŧ״̬ + ���������ݸ�ʽ(1�ֽ�)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	if(m_iHeaderNum>0)
		    iNum = 2 + 2 + 2 + m_iTitleLenth + 2 + m_iAllHeaderTextLen + 6*m_iButtonNum + m_iButtonNum + 1;

	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}



/***************************************************************************************
��    �ܣ����������������
����˵����CBinary idDataStream ������ID
		  string strDataStreamValue ��������ֵ
		  CBinary idUnit ��������λID
�� �� ֵ��������룬�ɹ�Ϊ0
˵    �����ڵ�һ���������ֵʱд���ʽ�ֽڣ�������������ݸ�ʽ���һ�β�ͬ���ܼ��벢����
**************************************************************************************/
short CActiveTestShow::Add (CBinary idDataStream, string strValue1, string strValue2)
{
	assert(BDF_NOMAL != m_iDataStreamFormat);

//	if(BDF_NOMAL == m_iDataStreamFormat)
//	{
//		return EC_TYPE_DATA_STREAM;  // ��������ʽ��һ��
//	}
	
	// ��λ������������
	m_iDataStreamFormat = BDF_VW;
	
	// ����ǵ�һ���������������
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;
		
//		// ���㰴ťѡ�������ֽ���
//		short iByteNum = (m_iButtonNum-1)/8 +1;		
//		char chMask = 0x00;		
//		for(short iIndex = 0; iIndex < iByteNum; iIndex++)
//		{
//			m_pSendAndReceive->Add(chMask);
//		}

		// ��������ʽ
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// ����������
		short iNum = 0;
		m_pSendAndReceive->Add(iNum);
	}
	

	// ������ID
	m_pSendAndReceive->Add(idDataStream);

	// ��������λ��
	m_pSendAndReceive->Add(strValue2);

	// ��������ֵ��
	m_pSendAndReceive->Add(strValue1);

	// ��������������  ����(2�ֽ�) + ���ܴ���(1�ֽ�) + ���Ա�־(1�ֽ�) + ��ť����(2�ֽ�) + ���ⳤ�� + ��Ŧ�ı�ID + ��Ŧ״̬ + ���������ݸ�ʽ(1�ֽ�)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	if(m_iHeaderNum>0)
		iNum = 2 + 2 + 2 + m_iTitleLenth + 2 + m_iAllHeaderTextLen +6*m_iButtonNum + m_iButtonNum + 1;

	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}


/****************************************
// [0] DisplayCount
// [1] DiagnoseCount
// [2] Msg
// [3] Valid
****************************************/
unsigned char CActiveTestShow::AcceptMsg ()
{
	unsigned char *pMsg;
	unsigned char DispCount;
	unsigned char DispValid;
	unsigned char Msg;
	
    //step 1 : ��ù����ڴ��ض���Ԫ��ַ
    char *pchContain = m_pSendAndReceive->GetBuffer();
    pMsg=(unsigned char *)&pchContain[0x10000-4];

	//step 2 : �ж�[3]=? 0x55, [0]=?[1]�Ƿ���ͬ
    DispValid = pMsg[3];
    DispCount = pMsg[0];
	if(DispValid==0x55)
	{
		if(DispCount!=pMsg[1])
		{
	        //step 3 : ����ͬ ȡ[2] Msg  and [1]=[0]//��Ҫʵ��;
            Msg=pMsg[2];
			pMsg[1]=DispCount;
			return Msg;
		}
	}
    return 0;
}



/***************************************************************************************
��    �ܣ���ʾ�������Խ��沢�����û�����
����˵����char *pMarkCode ��ť����ָ�룬��ΪNULL����ѡ����
�� �� ֵ���û��������
˵    ������ʾ����ȡ�����ݺ����������޻��������
**************************************************************************************/
short CActiveTestShow::Show ()
{
	//SYSTEMTIME sysTime;
	//GetSystemTime( &sysTime);

	// 7/18/2006 add by wcx
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;
		// ��������ʽ
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// ����������
		short iNum = 0;
		m_pSendAndReceive->Add(iNum);
	}
	// 7/18/2006 add end

	m_pSendAndReceive->Add(m_strMsg);

	// ������Ϣ
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������d
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���

	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}

	// ��4���ֽڱ�ʾ:���Ա�־
	char chDebugFlag = pchContain[3];
	bool bEnable = false;
	if(chDebugFlag == 0x01)
	{
		bEnable = true;
	}

	unsigned char uiDebugItem = CArithmetic::AD_DataStream;
	CArithmetic::SetDebugState (uiDebugItem, bEnable);

	// ��5���ֽڱ�ʾ:�û��������
	short iUserKey = pchContain[4];

 	return iUserKey;
}


/***************************************************************************************
��    �ܣ���ʾ�������Խ��沢�����û���ѡ�˵���ź��û�����
����˵����iSelNum���û���ѡ��Ĳ˵���ţ���0��ʼ������
�� �� ֵ���û��������
˵    ���������Ҫ���û���ѡ�˵���źͰ�������ͬ����
**************************************************************************************/
short CActiveTestShow::Show (short &iSelNum)
{
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;
		// ��������ʽ
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// ����������
		short iNum = 0;
		m_pSendAndReceive->Add(iNum);
	}
	// 7/18/2006 add end
//	m_pSendAndReceive->SetFlag(iFlag);
	m_pSendAndReceive->Add(m_strMsg);	//hpy added 20140113

	// ������Ϣ
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���

	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}

	// ��4���ֽڱ�ʾ:���Ա�־
	char chDebugFlag = pchContain[3];
	bool bEnable = false;
	if(chDebugFlag == 0x01)
	{
		bEnable = true;
	}

	unsigned char uiDebugItem = CArithmetic::AD_DataStream;
	CArithmetic::SetDebugState (uiDebugItem, bEnable);

	// ��5, 6���ֽڱ�ʾ:�˵��û�ѡ��
	unsigned char chHigh = pchContain[6];    // �˵��û�ѡ��ĸ��ֽڲ���
    unsigned char chLow  = pchContain[7];    // �˵��û�ѡ��ĵ��ֽڲ���
	if(chHigh == 0xff && chLow == 0xff)
	{
		iSelNum = -1;
	}
	else
	{
		iSelNum = 256*chHigh + chLow;
	}

	// ��5���ֽڱ�ʾ:�û��������
	short iUserKey = pchContain[4];

 	return iUserKey;
}


/***************************************************************************************
��    �ܣ����������������
����˵����string strDataStreamName ����������
		  string strDataStreamValue ������ֵ
		  string strDataStreamUnit ��������λ
�� �� ֵ��������룬�ɹ�Ϊ0
˵    �����ڵ�һ���������ֵʱд���ʽ�ֽڣ�������������ݸ�ʽ���һ�β�ͬ���ܼ��벢����
��    ����hpy added 20131203
**************************************************************************************/
short CActiveTestShow::Add (string strDataStreamName, string strDataStreamValue, string strDataStreamUnit)
{	
	// ����ǵ�һ���������������
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;

		// ��������ʽ
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// ����������
		short iCnt = 0;
		m_pSendAndReceive->Add(iCnt);
	}

	//��ϳ�����Ʊ�ʶ
	unsigned char flag = m_pSendAndReceive->GetFlag();
	flag |= STRING_ONLY;
	
	m_pSendAndReceive->SetFlag(flag);
//	m_pSendAndReceive->SetFlag(STRING_ONLY);
	
	// ����������
	m_pSendAndReceive->Add(strDataStreamName);

	// ������ֵ
	m_pSendAndReceive->Add(strDataStreamValue);

	// ��������λ
	m_pSendAndReceive->Add(strDataStreamUnit);

	// ��������������  ����(2�ֽ�) + ���ܴ���(1�ֽ�) + ���Ա�־(1�ֽ�) + ��ť����(2�ֽ�) + ���ⳤ�� + ��Ŧ�ı�ID + ��Ŧ״̬ + ���������ݸ�ʽ(1�ֽ�)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	if(m_iHeaderNum>0)
		    iNum = 2 + 2 + 2 + m_iTitleLenth + 2 + m_iAllHeaderTextLen + 6*m_iButtonNum + m_iButtonNum + 1;

	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}
