/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�	SpecialFuncTestShow.cpp
	�ĵ���ʶ��	007�������ƽ̨��ϸ���˵����
	ժ    Ҫ��	��ʾ��ĳ�Ա������������ɶ���������ʾ����
	��    �ܣ�
				1��	��������������Ϣ����ť��Ϣ���涨��ʽд�뻺������֪ͨ��ʾ����ȡ�����ݲ���ʾ��
				2��	������ʾ����������û�������

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2007.08.23  fufushang	1.0			�������ࡣ                                           

**************************************************************************************/
#ifdef WIN32
#include "windows.h"
#endif
#include "arithmetic.h"
#include "SpecialFuncTestShow.h"
#include "Database.h"
#include "assert.h"
#include "display.h"

#define INTERVAL_TIME 50


//����ID���ı��ⷵ���ַ���
extern string adsGetTextString (CBinary binTextID);
//extern CArithmetic::SetDebugState (unsigned char uiDebugItem, bool bEnable);


CSpecialFuncTestShow::CSpecialFuncTestShow()
{
	m_iButtonNum = 0;
	
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
void CSpecialFuncTestShow::Init (const char* pTitle)
{
	assert(NULL != pTitle);

	// ��ť������0
	m_iButtonNum = 0;

	// ��������ʽ��λ
	m_iDataStreamFormat = -1;

	// �Ƿ��Ѿ��������������
	m_bAddDataStream    = false; 

	m_iTitleLenth = strlen(pTitle)+1;  // ������ⳤ��

	char chFuction = DT_SPECIAL_TEST;
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
void CSpecialFuncTestShow::Init (string strTitle)
{	
	// �������غ���
	Init(strTitle.c_str());
}

/***************************************************************************************
��    �ܣ��������⹦��˵��
����˵����string Msg �������Ա��⣬
�� �� ֵ����
˵    ������Ϊδ���붯�����԰�ť����Ҫ���AddButton ()����һ��ʹ��
**************************************************************************************/
short CSpecialFuncTestShow::AddPrerequisites(const char* strMsg )
{	
	m_pSendAndReceive->Add(strMsg, strlen(strMsg)+1);
	return true;
}

/***************************************************************************************
��    �ܣ��������⹦��˵��
����˵����string Msg �������Ա��⣬
�� �� ֵ����
˵    ������Ϊδ���붯�����԰�ť����Ҫ���AddButton ()����һ��ʹ��
**************************************************************************************/
short CSpecialFuncTestShow::AddProcessCtrl (short nMin,short nMax,short nSteps)
{
	m_pSendAndReceive->Add(nMin);
	m_pSendAndReceive->Add(nMax);
	m_pSendAndReceive->Add(nSteps);
		return true;
}

/***************************************************************************************
��    �ܣ���ʼ����������
����˵����CBinary idTitle �������Ա��⣬
�� �� ֵ����
˵    ������Ϊδ���붯�����԰�ť����Ҫ���AddButton ()����һ��ʹ��
**************************************************************************************/
void CSpecialFuncTestShow::Init (CBinary idTitle)
{
	// ����IDת�����ַ���
	string strTitle = adsGetTextString(idTitle);

	// �������غ���
	Init(strTitle);
}


bool CSpecialFuncTestShow::SetFlag(unsigned char byFlag)
{
	m_pSendAndReceive->SetFlag(byFlag);
	return true;
}


/***************************************************************************************
��    �ܣ���Ӷ������Խ��水ť
����˵����CBinary idButtonText ��ӵİ�ť���⣬
�� �� ֵ���������
˵    ������
**************************************************************************************/
short CSpecialFuncTestShow::AddButton (CBinary idButtonText, char byStatus)
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
��    �ܣ����������������
����˵����CBinary idDataStream ������ID
		  string strDataStreamValue ��������ֵ
�� �� ֵ��������룬�ɹ�Ϊ0
˵    �����ڵ�һ���������ֵʱд���ʽ�ֽڣ�������������ݸ�ʽ���һ�β�ͬ���ܼ��벢����
**************************************************************************************/
short CSpecialFuncTestShow::Add (CBinary idDataStream, string strDataStreamValue)
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
short CSpecialFuncTestShow::Add (CBinary idDataStream, string strDataStreamValue, CBinary idUnit)
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
	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}

/***************************************************************************************
��    �ܣ���ʾ�������Խ��沢�����û�����
����˵����char *pMarkCode ��ť����ָ�룬��ΪNULL����ѡ����
�� �� ֵ���û��������
˵    ������ʾ����ȡ�����ݺ����������޻��������
**************************************************************************************/
short CSpecialFuncTestShow::Show ()
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

	static unsigned long last = 0;

	timeval startTm;
	gettimeofday(&startTm, 0);
	static unsigned long start = startTm.tv_usec;
	//static unsigned long start = GetTickCount(); //1000*sysTime.wSecond + sysTime.wMilliseconds;
	//if(start - last > INTERVAL_TIME)

#if 0
	if(start - last < INTERVAL_TIME)
	{
		last = start;
		return 0xFF; //adsMB_NoButton;
	}
#endif

	// �����ϴε�ʱ��
	last = start;

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
short CSpecialFuncTestShow::Add (string strDataStreamName, string strDataStreamValue, string strDataStreamUnit)
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
	m_pSendAndReceive->SetFlag(STRING_ONLY);

	// ����������
	m_pSendAndReceive->Add(strDataStreamName);

	// ������ֵ
	m_pSendAndReceive->Add(strDataStreamValue);
	
	// ��������λ
	m_pSendAndReceive->Add(strDataStreamUnit);

	// ��������������  ����(2�ֽ�) + ���ܴ���(1�ֽ�) + ���Ա�־(1�ֽ�) + ��ť����(2�ֽ�) + ���ⳤ�� + ��Ŧ�ı�ID + ��Ŧ״̬ + ���������ݸ�ʽ(1�ֽ�)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}
