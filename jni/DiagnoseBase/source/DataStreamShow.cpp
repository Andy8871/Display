/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�	DataStreamShow.cpp
	�ĵ���ʶ��	007�������ƽ̨��ϸ���˵����
	ժ    Ҫ��	��������ʾ�ࡣ
	��    �ܣ�
				1��	���������������ݰ��涨��ʽд�뻺������֪ͨ��ʾ����ȡ�����ݲ���ʾ��
				2��	������ʾ����������û�������

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			�������ࡣ                                           

**************************************************************************************/

#include "display.h"
#ifdef WIN32
#include "windows.h"
#endif
#include "DataStreamShow.h"
#include "Database.h"
#include "arithmetic.h"
#include <assert.h>

#define INTERVAL_TIME 50


//����ID���ı��ⷵ���ַ���
extern string adsGetTextString (CBinary binTextID);
//extern CArithmetic::SetDebugState (unsigned char uiDebugItem, bool bEnable);

CDataStreamShow::CDataStreamShow()
{
	m_iDataStreamFormat = -1;  // ��������ʽ
}

CDataStreamShow::~CDataStreamShow()
{
}


/***************************************************************************************
��    �ܣ���ʼ��������
����˵����char* pTitle ���������⣬��Ϊ����ʾ����ʹ��ȱʡ����
		  char* pStdValueLibName = NULL ��������׼�����ƣ�ȱʡΪ��
		  unsigned short uiType ��������ť��ʾ��ʽ��ȱʡΪ����ʾǰ��ͨ����ť
�� �� ֵ����
˵    ������
**************************************************************************************/
#if 0
void CDataStreamShow::Init(const char* pTitle, const char* pStdValueLibName, unsigned short uiType)
{
	// ��������ʽ��λ
	m_iDataStreamFormat = -1;

	//added by johnnyling
	m_iDataStreamType=uiType;

	char chFuction = DT_DATA_STREAM;
	m_pSendAndReceive->Init(chFuction);  // ��书�ܴ���͵��Ա�־
	
	// ������ʾ��־	
	char chType = (char)m_iDataStreamType; //uiType;
	m_pSendAndReceive->Add(chType);  

	// ���������ݸ�ʽ
	char chFormat = 0;
	m_pSendAndReceive->Add(chFormat); 

	// ����������
	unsigned short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	// ��������׼������	
	if(NULL == pStdValueLibName)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		m_pSendAndReceive->Add(pStdValueLibName, strlen(pStdValueLibName)+1);
	}

	if(NULL == pTitle)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		// ����������	
		m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
	}
}
#endif

/***************************************************************************************
��    �ܣ���ʼ��������
����˵����string strTitle ����������
		  string strStdValueLibName ��������׼�����ƣ�ȱʡΪ�մ�
		  unsigned short uiType ��������ť��ʾ��ʽ��ȱʡΪ����ʾǰ��ͨ����ť
�� �� ֵ����
˵    ������
**************************************************************************************/
void CDataStreamShow::Init(string strTitle, string strStdValueLibName, unsigned short uiType)
{
	// �������غ���
	//Init(strTitle.c_str(), strStdValueLibName.c_str(), uiType);
	const char* pTitle;
	const char* pStdValueLibName;

	if(strTitle.length())
		pTitle=strTitle.c_str();
	else
		pTitle=NULL;

	if(strStdValueLibName.length())
		pStdValueLibName=strStdValueLibName.c_str();
	else
		pStdValueLibName=NULL;

	// ��������ʽ��λ
	m_iDataStreamFormat = -1;

	//added by johnnyling
	m_iDataStreamType=uiType;

	char chFuction = DT_DATA_STREAM;
	m_pSendAndReceive->Init(chFuction);  // ��书�ܴ���͵��Ա�־
	
	// ������ʾ��־	
	char chType = (char)m_iDataStreamType; //uiType;
	m_pSendAndReceive->Add(chType);  

	// ���������ݸ�ʽ
	char chFormat = 0;
	m_pSendAndReceive->Add(chFormat); 

	// ����������
	unsigned short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	// ��������׼������	
	if(NULL == pStdValueLibName)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		m_pSendAndReceive->Add(pStdValueLibName, strlen(pStdValueLibName)+1);
	}

	if(NULL == pTitle)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		// ����������	
		m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
	}
}


/***************************************************************************************
��    �ܣ���ʼ��������
����˵����
		unsigned short uiTopLine����ʼ�к�
		unsigned short uiAllCount��������������
		string strTitle ����������
		  string strStdValueLibName ��������׼�����ƣ�ȱʡΪ�մ�
		  unsigned short uiType ��������ť��ʾ��ʽ��ȱʡΪ����ʾǰ��ͨ����ť
�� �� ֵ����
˵    ������
**************************************************************************************/
void CDataStreamShow::Init(unsigned short uiTopLine,unsigned short uiAllCount,string strTitle, string strStdValueLibName, unsigned short uiType)
{
	// �������غ���
	//Init(strTitle.c_str(), strStdValueLibName.c_str(), uiType);
	const char* pTitle;
	const char* pStdValueLibName;

	if(strTitle.length())
		pTitle=strTitle.c_str();
	else
		pTitle=NULL;

	if(strStdValueLibName.length())
		pStdValueLibName=strStdValueLibName.c_str();
	else
		pStdValueLibName=NULL;

	// ��������ʽ��λ
	m_iDataStreamFormat = -1;

	//added by johnnyling
	m_iDataStreamType=uiType;

	char chFuction = DT_DATA_STREAM;
	m_pSendAndReceive->Init(chFuction);  // ��书�ܴ���͵��Ա�־

	m_pSendAndReceive->SetFlag(0x01);

	// ������ʾ��־	
	char chType = (char)m_iDataStreamType; //uiType;
	m_pSendAndReceive->Add(chType);  

	// ���������ݸ�ʽ
	char chFormat = 0;
	m_pSendAndReceive->Add(chFormat); 

	// ����������
	unsigned short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	//////////////add by scf 2008.07.09///////////
	//��������������
	m_pSendAndReceive->Add(uiAllCount);

	//��������ʼ�к�
	m_pSendAndReceive->Add(uiTopLine);
	//////////////End add by scf 2008.07.09///////////

	// ��������׼������	
	if(NULL == pStdValueLibName)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		m_pSendAndReceive->Add(pStdValueLibName, strlen(pStdValueLibName)+1);
	}

	if(NULL == pTitle)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		// ����������	
		m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
	}
}

/***************************************************************************************
��    �ܣ�������������ʾ�ķ���
����˵����iFlag 0x01��ʾ�����������ÿ�����ʾ
�� �� ֵ��������룬�ɹ�Ϊ1
˵    ����
**************************************************************************************/
//unsigned short CDataStreamShow::SetFlag(unsigned short iFlag)
//{
	
//	return true;
//}


/***************************************************************************************
��    �ܣ���ʼ��������
����˵����CBinary idTitle ����������ID
		  CBinary idStdValueLibName ��������׼������ID
		  unsigned short uiType ��������ť��ʾ��ʽ��ȱʡΪ����ʾǰ��ͨ����ť
�� �� ֵ����
˵    ������
**************************************************************************************/
void CDataStreamShow::Init (CBinary idTitle, CBinary idStdValueLibName, unsigned short uiType)
{
	// ����IDת�����ַ���
	string strTitle = adsGetTextString(idTitle);

	// ����IDת�����ַ���
	string strStdValueLibName = adsGetTextString(idStdValueLibName);

	// �������غ���
	Init(strTitle, strStdValueLibName, uiType);
}

/***************************************************************************************
��    �ܣ���ʼ��������
����˵����CBinary idTitle ����������ID
�� �� ֵ����
˵    ������
**************************************************************************************/
void CDataStreamShow::Init (CBinary idTitle, string strStdValueLibName, unsigned short uiType)
{
	Init (adsGetTextString(idTitle), strStdValueLibName, uiType);
}


/***************************************************************************************
��    �ܣ����������������
����˵����CBinary idDataStream ������ID
	      string strDataStreamValue ��������ֵ
�� �� ֵ��������룬�ɹ�Ϊ0
˵    �����ڵ�һ���������ֵʱд���ʽ�ֽڣ�������������ݸ�ʽ���һ�β�ͬ���ܼ��벢����
**************************************************************************************/
unsigned short CDataStreamShow::Add (CBinary idDataStream, string strDataStreamValue,unsigned short nIndex,bool bUpdateFlag)
{
	if(BDF_VW == m_iDataStreamFormat)
	{
		return EC_TYPE_DATA_STREAM;  // ��������ʽ��һ��
	}

	if(ID_BINARY_LENTH != idDataStream.GetSize())
	{
		return EC_DATA_FORMAT;  // ���ݸ�ʽ��
	}

	// һ��������
	m_iDataStreamFormat = BDF_NOMAL;

	// ������ID
	m_pSendAndReceive->Add(idDataStream);

	// ��������ֵ��
	m_pSendAndReceive->Add(strDataStreamValue);

	// ��������������
	m_pSendAndReceive->IncreaseNum(6);

	/*
	typedef struct MSG_DATA
	{
		unsigned char CmdId;
		unsigned short iIndex;
		unsigned char	ID[6];
		CHAR	chValue[30];
		CHAR	chUnit[20];
	}MSG_DATA, *PMSGDATA;
	*/

	if(bUpdateFlag)
	{
		MSG_DATA *pMsgData = new MSG_DATA;
		memset(pMsgData,0,sizeof(MSG_DATA));
		pMsgData->CmdId = DT_DATA_STREAM;
		pMsgData->iIndex = nIndex;
		idDataStream.ReadBuffer(pMsgData->chID,0,6);
		sprintf(pMsgData->chValue,"%s",strDataStreamValue.c_str());
		bool bSend = false;
#ifdef WIN32 //need to modify
		COPYDATASTRUCT copyData; 
		copyData.cbData = sizeof(MSG_DATA); 
		copyData.lpData = pMsgData;
		bSend = m_pSendAndReceive->SendDataToDisplayImmediateResponsion((LPARAM)&copyData);
#endif
		if(pMsgData)
		{
			delete []pMsgData;
			pMsgData = NULL;
		}
		return bSend;
	}

#ifdef DATA_DRV_EXT
	return false;
#endif	//	#ifdef DATA_DRV_EXT
}

/***************************************************************************************
��    �ܣ����������������
����˵����CBinary idDataStream ������ID
	      string strDataStreamValue ��������ֵ
		  CBinary idUnit ��������λID
�� �� ֵ��������룬�ɹ�Ϊ0
˵    �����ڵ�һ���������ֵʱд���ʽ�ֽڣ�������������ݸ�ʽ���һ�β�ͬ���ܼ��벢����
**************************************************************************************/
unsigned short CDataStreamShow::Add (CBinary idDataStream, string strDataStreamValue, CBinary idUnit,unsigned short nIndex,bool bUpdateFlag)
{
	if(BDF_NOMAL == m_iDataStreamFormat)
	{
		return EC_TYPE_DATA_STREAM;  // ��������ʽ��һ��
	}
	
	if(ID_BINARY_LENTH != idDataStream.GetSize() || ID_BINARY_LENTH != idUnit.GetSize())
	{
		return EC_DATA_FORMAT;  // ���ݸ�ʽ��
	}

	// ��λ������������
	m_iDataStreamFormat = BDF_VW;

	// ������ID
	m_pSendAndReceive->Add(idDataStream);

	// ��������λID
	m_pSendAndReceive->Add(idUnit);

	// ��������ֵ��
	m_pSendAndReceive->Add(strDataStreamValue);

	// ��������������
	m_pSendAndReceive->IncreaseNum(6);
	
	if(bUpdateFlag)
	{
		MSG_DATA *pMsgData = new MSG_DATA;
		memset(pMsgData,0,sizeof(MSG_DATA));
		pMsgData->CmdId = DT_DATA_STREAM;
		pMsgData->iIndex = nIndex;
		idDataStream.ReadBuffer(pMsgData->chID,0,6);
		sprintf(pMsgData->chValue,"%s",strDataStreamValue.c_str());
		bool bSend = false;
#ifdef WIN32 //need to modify
		COPYDATASTRUCT copyData; 
		copyData.cbData = sizeof(MSG_DATA); 
		copyData.lpData = pMsgData;
		bSend = m_pSendAndReceive->SendDataToDisplayImmediateResponsion((LPARAM)&copyData);
#endif
		if(pMsgData)
		{
			delete []pMsgData;
			pMsgData = NULL;
		}
		return bSend;
	}
	return true;
}

bool CDataStreamShow::SetFlag(unsigned char iFlag)
{
	unsigned char byTmp = m_pSendAndReceive->GetFlag();
	byTmp = byTmp | iFlag;
	m_pSendAndReceive->SetFlag(byTmp);
	return true;
}
/***************************************************************************************
��    �ܣ���ʾ�������������û�����
����˵������
�� �� ֵ���û�������ֵ
˵    ������ʾ����ȡ�����ݺ����������޻�������ֵ��
**************************************************************************************/
unsigned short CDataStreamShow::Show (bool bFlag)
{
	// �޸���������ʽ
	char chDataStreamFormat = (char)m_iDataStreamFormat;
	m_pSendAndReceive->Modify(chDataStreamFormat, 5);
	if(bFlag) //add by scf 2009.12.02
	{
		unsigned char byTmp =	m_pSendAndReceive->GetFlag();
		byTmp = byTmp|0xF0;
		byTmp = byTmp&0xF2;
		m_pSendAndReceive->SetFlag(byTmp);
		//m_pSendAndReceive->SetFlag(0x02);
	}
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
		assert( iLenth < 5 );  
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


	// ��5���ֽڱ�ʾ:�˵��û�ѡ��
	unsigned short iUserKey = pchContain[4];
	
	return iUserKey;
}

/****************************************
// [0] DisplayCount
// [1] DiagnoseCount
// [2] Msg
// [3] Valid
****************************************/
unsigned char CDataStreamShow::AcceptMsg ()
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
��    �ܣ���ʾ�������������û�����
����˵������
�� �� ֵ���û�������ֵ
˵    ������ʾ����ȡ�����ݺ����������޻�������ֵ��
**************************************************************************************/
unsigned short CDataStreamShow::Show (unsigned short &iTop,unsigned short &iNum,bool bFlag)
{
	// �޸���������ʽ
	char chDataStreamFormat = (char)m_iDataStreamFormat;
	m_pSendAndReceive->Modify(chDataStreamFormat, 5);

	//add by scf 2009.12.02
	if(bFlag)
	{
		unsigned char byTmp =	m_pSendAndReceive->GetFlag();
		byTmp = byTmp|0xF0;
		byTmp = byTmp&0xF3;
		m_pSendAndReceive->SetFlag(byTmp);
	}

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
		assert( iLenth < 5 );  
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


	// ��5���ֽڱ�ʾ:�˵��û�ѡ��
	unsigned short iUserKey = pchContain[4];
	
	//��6��7�ڱ�ʾ:��ǰ���
	iTop = (unsigned char)pchContain[5] * 256 + (unsigned char)pchContain[6];

	//��8��9�ڱ�ʾ:��ǰ����
	iNum = (unsigned char)pchContain[7] * 256 + (unsigned char)pchContain[8];

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
unsigned short CDataStreamShow::Add (string strDataStreamName, string strDataStreamValue, string strDataStreamUnit)
{
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

	// ��������������
	m_pSendAndReceive->IncreaseNum(6);
	
	return true;
}
