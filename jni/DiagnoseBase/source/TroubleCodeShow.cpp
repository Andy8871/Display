/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�	TroubleCodeShow.cpp
	�ĵ���ʶ��	007�������ƽ̨��ϸ���˵����
	ժ    Ҫ��	��������ʾ��

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			��������ڡ�                                           

**************************************************************************************/
#include "Display.h"
#include "TroubleCodeShow.h"
#include "Database.h"
#include "assert.h"


//����ID���ı��ⷵ���ַ���
extern string adsGetTextString (CBinary binTextID);


CTroubleCodeShow::CTroubleCodeShow()
{
	m_pfnGetTroubleCodeString = NULL;
	m_DRV_FileName = "";
}

/***************************************************************************************
��    �ܣ���ʼ��������
����˵����char *pTitle ��������⣬��Ϊ�մ���ʾ����ʹ��ȱʡ����
		  unsigned int bMode  0:��ͨģʽ 1:��������ģʽ
�� �� ֵ����
˵    ������
**************************************************************************************/
void CTroubleCodeShow::Init (const char *pTitle,unsigned int bMode)
{
	assert(NULL != pTitle);
	char chFuction = DT_TROUBLE_CODE;
	m_pSendAndReceive->Init(chFuction);  // ��书�ܴ���͵��Ա�־
	m_dataDrvMode = false;
	// ����������
	short iCodeNum = 0;
	m_pSendAndReceive->Add(iCodeNum);

	// ���������
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
#ifdef DATA_DRV_EXT
	if(bMode == 1)
	{
		// ��������ʹ�õ�PTC�ļ���
		m_dataDrvMode = true;
		if (m_DRV_FileName.length() > 0)
		{
			const char* ptcName = m_DRV_FileName.c_str();
			char t = (char)0xfe;
			m_pSendAndReceive->Add(&t,2);
			m_pSendAndReceive->Add(ptcName, strlen(ptcName)+1);
		}
	}
#endif
}

/***************************************************************************************
��    �ܣ���ʼ��������
����˵����string strTitle��������ʾ����
�� �� ֵ����
˵    ������
**************************************************************************************/
void CTroubleCodeShow::Init (string strTitle,unsigned int bMode)
{
	// �������غ���
	Init(strTitle.c_str(),bMode);
}

/***************************************************************************************
��    �ܣ���ʼ��������
����˵����CBinary idTitle ��������ʾ����ID
�� �� ֵ����
˵    ������
**************************************************************************************/
void CTroubleCodeShow::Init (CBinary idTitle,unsigned int bMode)
{
	// ����IDת�����ַ���
	string strTitle = adsGetTextString(idTitle);

	// �������غ���
	Init(strTitle,bMode);
}

/***************************************************************************************
��    �ܣ����ù�����ص�����ָ��
����˵����pfnCallBack �ص�����ָ��
�� �� ֵ��ԭ�ص�����ָ��
˵    ����ÿ���һ����������ûص�����һ�Σ��ص���������"���ϴ���"�ִ���
          ��������ʾ�����ڹ���������Ҳ����ù�����IDʱ��ʾ���ϴ�������
**************************************************************************************/
void *CTroubleCodeShow::SetTroubleCodeCallBackFunction (string (*pfnCallBack) (CBinary idStroubleCode))
{
	m_pfnGetTroubleCodeString = pfnCallBack;

	return (void *)m_pfnGetTroubleCodeString;
}

/***************************************************************************************
��    �ܣ���ӹ���������
����˵����CBinary idTroubleCode ������ID��string strTroubleStatus ������״̬�ַ���
�� �� ֵ���ɹ�-false ʧ��-true
˵    ������
**************************************************************************************/
bool CTroubleCodeShow::Add (CBinary idTroubleCode, string strTroubleStatus)
{
	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());

	// ������ID
	m_pSendAndReceive->Add(idTroubleCode);

	// ������״̬
	m_pSendAndReceive->Add(strTroubleStatus);

	// ����IDת�����ַ���
	if (!m_dataDrvMode){
		string strTroubleCode;
		if(NULL == m_pfnGetTroubleCodeString)
		{
			strTroubleCode = DefaultStroubleCodeCallBack(idTroubleCode);  // ������ȱʡ�ص�����
		}
		else
		{		
			strTroubleCode = m_pfnGetTroubleCodeString(idTroubleCode);  // ʹ�ú���ָ��
		}	
		
		// ��������봮
		m_pSendAndReceive->Add(strTroubleCode);
	}
	// ��������������
	m_pSendAndReceive->IncreaseNum(4);

	return true;
}
/***************************************************************************************
��    �ܣ���ӹ���������
 �˽ӿ���ʱֻ����������ģʽ����ϳ���ʹ��
����˵����CBinary idTroubleCode ������ID��CBinary idDesc ������������string strTroubleStatus ������״̬�ַ���
�� �� ֵ���ɹ�-false ʧ��-true
˵    ������
**************************************************************************************/
// 
// bool CTroubleCodeShow::Add (CBinary idTroubleCode, CBinary idDesc,string strTroubleStatus)
// {
// 	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());
// 	
// 	// ������ID
// 	m_pSendAndReceive->Add(idTroubleCode);
// 	//����������ID
// 	m_pSendAndReceive->Add(idDesc);
// 	
// 	// ������״̬
// 	if (strTroubleStatus.length() > 0)
// 		m_pSendAndReceive->Add(strTroubleStatus);
// 	else
// 		m_pSendAndReceive->Add("");
// 	
// 	
// 	// ����IDת�����ַ���
// 	// 	if (!m_dataDrvMode){
// 	string strTroubleCode;
// 	CBinary flag = CBinary("\xFF\xFF\xFF\xFF\xFF\xFE",6);
// 	if (idTroubleCode == flag){
// 		//DataDrv especially
// 		unsigned char temp = 0;
// 		if (strTroubleStatus.length() >= 4){
// 			string tempStr = strTroubleStatus.substr(strTroubleStatus.length() - 2);
// 			CHARS2HEX(&temp,tempStr.c_str(),1);
// 			idTroubleCode.SetAt(5,temp);
// 			tempStr = strTroubleStatus.substr(strTroubleStatus.length() - 4,2);
// 			CHARS2HEX(&temp,tempStr.c_str(),1);
// 			idTroubleCode.SetAt(4,temp);
// 		}
// 	}
// 	if(NULL == m_pfnGetTroubleCodeString)
// 	{
// 		
// 		strTroubleCode = DefaultStroubleCodeCallBack(idTroubleCode);  // ������ȱʡ�ص�����
// 	}
// 	else
// 	{		
// 		strTroubleCode = m_pfnGetTroubleCodeString(idTroubleCode);  // ʹ�ú���ָ��
// 	}
// 	
// 	// ��������봮
// 	m_pSendAndReceive->Add(strTroubleCode);
// 	// 	}
// 	// ��������������
// 	m_pSendAndReceive->IncreaseNum(4);
// 	
// 	return true;
// }

bool CTroubleCodeShow::Add (CBinary idTroubleCode, CBinary idDesc,string strTroubleStatus,string strSAEFromConv)
{
	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());

	// ������ID
	m_pSendAndReceive->Add(idTroubleCode);
	//����������ID
	m_pSendAndReceive->Add(idDesc);

	// ������״̬
	if (strTroubleStatus.length() > 0)
		m_pSendAndReceive->Add(strTroubleStatus);
	else
		m_pSendAndReceive->Add("");

	string strTroubleCode = "";
	// ����IDת�����ַ���
 	if (m_dataDrvMode){
		CBinary flag = CBinary("\xFF\xFF\xFF\xFF\xFF\xFE",6);
		if (idTroubleCode == flag){
			//DataDrv especially
			if (strSAEFromConv.length() > 0){
				//DataDrv flag "SAE:"
				strTroubleCode += "SAE:";
				strTroubleCode += strSAEFromConv;
			}
		}
	}else{
		if(NULL == m_pfnGetTroubleCodeString)
		{
			
			strTroubleCode = DefaultStroubleCodeCallBack(idTroubleCode);  // ������ȱʡ�ص�����
		}
		else
		{		
			strTroubleCode = m_pfnGetTroubleCodeString(idTroubleCode);  // ʹ�ú���ָ��
		}
	}
		// ��������봮
	if (strTroubleCode.length() != 0)
		m_pSendAndReceive->Add(strTroubleCode);
// 	}
	// ��������������
	m_pSendAndReceive->IncreaseNum(4);

	return true;
}

/***************************************************************************************
��    �ܣ���ӹ���������
����˵����CBinary idTroubleCode ������ID��CBinary idTroubleStatus ������״̬ID
�� �� ֵ���ɹ�-false ʧ��-true
˵    ������
**************************************************************************************/
bool CTroubleCodeShow::Add (CBinary idTroubleCode, CBinary idTroubleStatus)
{
	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());
	assert(ID_BINARY_LENTH == idTroubleStatus.GetSize());

	// ����IDת�����ַ���
	string strTroubleStatus = adsGetTextString(idTroubleStatus);

	//�������غ���
	Add(idTroubleCode, strTroubleStatus);

	return true;
}

/***************************************************************************************
��    �ܣ�֪ͨ��ʾ����ȡ���ݲ��ȴ���Ӧ
����˵������
�� �� ֵ����
˵    ������
**************************************************************************************/
void CTroubleCodeShow::Show ()
{
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
}

/***************************************************************************************
��    �ܣ����ö������Եı�־λ
����˵����0x01:֧������������ܷ�����ѡ����
�� �� ֵ��������룬�ɹ�Ϊ1
˵    ����
**************************************************************************************/
bool CTroubleCodeShow::SetFlag(unsigned char iFlag)
{
	//m_pSendAndReceive->Add(iFlag);
	m_pSendAndReceive->SetFlag(iFlag);
	return true;
}


/***************************************************************************************
��    �ܣ���ʾ�������Խ��沢�����û���ѡ�˵���ź��û�����
����˵����iSelNum���û���ѡ��Ĳ˵���ţ���0��ʼ������
�� �� ֵ���û��������
˵    ���������Ҫ���û���ѡ�˵���źͰ�������ͬ����
**************************************************************************************/
short CTroubleCodeShow::Show (short &iSelNum)
{
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


string  CTroubleCodeShow::GetSelectedItemText(short iCol)
{	
	// ��ȡ�յ�������
/*	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);
	int i=0;
	int nColLen = 0;
	while(1)
	{
		int nLen = strlen(pchContain+8+nColLen);
		if(nLen == 0) 
			return NULL;
		
		if(i==iCol)
			return pchContain+8+nColLen;
		else
			nColLen += nLen;
	}*/
	return 0;
}

/***************************************************************************************
��    �ܣ�������ȱʡ�ص�����
����˵����CBinary idStroubleCode ������ID
�� �� ֵ����
˵    ��������ϳ���Աδ�趨������ص�����ʱ��ȱʡ�ص�������ID�ĵͶ�λ�ֽڵ�IDֵת����PCBU��
         ����IDΪ 0x01 0x02 0x03 0x04 0x01 0x23 ������Ĺ�����Ϊ"P0123"��
**************************************************************************************/
string CTroubleCodeShow::DefaultStroubleCodeCallBack(CBinary idTroubleCode)
{
	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());
	
	short iLenth = idTroubleCode.GetSize();

	char chHigh;
	char chLow;
	string strTroubleCode;
	for(short i = 0; i < (iLenth-1)/6+1; i++)
	{
		chHigh = idTroubleCode.GetAt((iLenth-1)/6 + 4);  // ȡ��5���ֽ�
		chLow  = idTroubleCode.GetAt((iLenth-1)/6 + 5);  // ȡ��6���ֽ�

		strTroubleCode += TanslateToPCBU(chHigh, chLow);
	}
	return strTroubleCode;
}

/***************************************************************************************
��    �ܣ��������ֽ�ת����PCBU��
����˵����char chHigh, char chLow
�� �� ֵ����
˵    ������
**************************************************************************************/
string CTroubleCodeShow::TanslateToPCBU(char chHigh, char chLow)
{
	string strTroubleCode;

	short iTemp = (chHigh>>6)&0x03;
	switch(iTemp)
	{
	case 0:
		strTroubleCode = "P";
		break;

	case 1:
		strTroubleCode = "C";
		break;

	case 2:
		strTroubleCode = "B";
		break;

	case 3:
		strTroubleCode = "U";
		break;

	default:
		break;
	}

	char chBufferTmp[20];
	string strTmp;

	iTemp = (chHigh>>4)&0x03;
	sprintf(chBufferTmp, "%x", iTemp);
	strTmp = chBufferTmp;	
	strTroubleCode += strTmp.c_str();

	iTemp = chHigh&0x0F;
	sprintf(chBufferTmp, "%x", iTemp);
	strTmp = chBufferTmp;	
	strTroubleCode += strTmp.c_str();

	iTemp = (chLow>>4)&0x0F;
	sprintf(chBufferTmp, "%x", iTemp);
	strTmp = chBufferTmp;	
	strTroubleCode += strTmp.c_str();

	iTemp = chLow&0x0F;
	sprintf(chBufferTmp, "%x", iTemp);
	strTmp = chBufferTmp;	
	strTroubleCode += strTmp.c_str();

	return strTroubleCode;
}

/***************************************************************************************
��    �ܣ���ӹ���������
����˵����string strTroubleCode �������ַ�����
		  string strTroubleDesc �����������ַ�����
		  string strTroubleStatus ������״̬�ַ���
�� �� ֵ���ɹ�-false ʧ��-true
˵    �����˽ӿ���ʱֻ����ϳ����ڱ�Ҫ��ʱ��ʹ�ã��Խ�ʡ����ռ�
��    ����hpy added 20131203
****************************************************************************************/
bool CTroubleCodeShow::Add (string strTroubleCode, string strTroubleDesc, string strTroubleStatus)
{
	m_pSendAndReceive->SetFlag(STRING_ONLY);
	
	// ������������ַ���
	m_pSendAndReceive->Add(strTroubleCode);

	//�����������ַ���
	m_pSendAndReceive->Add(strTroubleDesc);
	
	//������״̬�ַ���
	m_pSendAndReceive->Add(strTroubleStatus);

	//��������������
	m_pSendAndReceive->IncreaseNum(4);
	
	return true;
}


