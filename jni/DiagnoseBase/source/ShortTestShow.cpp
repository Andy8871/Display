/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�	ShortTestShow.cpp
	�ĵ���ʶ��	007�������ƽ̨��ϸ���˵����
	ժ    Ҫ��	��ʾ��ĳ�Ա������������ɿ��ٲ�����ʾ����
	��    �ܣ�
				1��	�����ٲ������ݰ��涨��ʽд�뻺������֪ͨ��ʾ����ȡ�����ݲ���ʾ��
				2��	������ʾ����������û�������

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			�������ࡣ                                           

**************************************************************************************/

#include "ShortTestShow.h"
#include "display.h"
#include "assert.h"

/***************************************************************************************
��    �ܣ���ʼ�����ٲ���
����˵����char* pTitle ���ٲ��Ա��⣬��Ϊ����ʾ����ʹ��ȱʡ����
�� �� ֵ����
˵    ������
**************************************************************************************/
void CShortTestShow::Init (const char* pTitle,short Hide_Btn_Stop)
{
	assert(NULL != pTitle);

	char chFuction = DT_SHORT_TEST;
	m_pSendAndReceive->Init(chFuction);  // ��书�ܴ���͵��Ա�־

	// ����������
	short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	// ���ٲ��Ա���
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
#ifdef DATA_DRV_EXT
	short dataDrvMode = Hide_Btn_Stop;
	m_pSendAndReceive->Add(dataDrvMode);
#endif

}

/***************************************************************************************
��    �ܣ���ʼ�����ٲ���
����˵����string strTitle ���ٲ��Ա��⣬����ǿմ���ʹ��ȱ�ٱ���
�� �� ֵ����
˵    ������
**************************************************************************************/
void CShortTestShow::Init (string strTitle)
{
	// �������غ���
	Init(strTitle.c_str());
}

/***************************************************************************************
��    �ܣ���ʼ�����ٲ���
����˵����CBinary idTitle���ٲ��Ա���ID�����ID����Ϊ0��ʹ��ȱʡ����
�� �� ֵ����
˵    ������
**************************************************************************************/
void CShortTestShow::Init (CBinary idTitle)
{
	// ����IDת�����ַ���
	string strTitle = adsGetTextString(idTitle);

	// �������غ���
	Init(strTitle);
}



bool CShortTestShow::SetFlag(unsigned char iFlag)
{
	m_pSendAndReceive->SetFlag(iFlag);
	return true;
}



/***************************************************************************************
��    �ܣ���ӿ��ٲ�����������
����˵����char* pContain ���ٲ���������
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ���������Ŀ���ݺ�����ֵ���Զ���Ϊ�մ�
**************************************************************************************/
short CShortTestShow::AddItem (const char* pContain)
{
	assert(NULL != pContain);

	// ��������ʾ
	m_pSendAndReceive->Add(pContain, strlen(pContain)+1);

	// ��������������
	m_pSendAndReceive->IncreaseNum(4);

	return true;
}

/***************************************************************************************
��    �ܣ���ӿ��ٲ�����������
����˵����string strContain ���ٲ���������
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ���������Ŀ���ݺ�����ֵ���Զ���Ϊ�մ�
**************************************************************************************/
short CShortTestShow::AddItem (string strContain)
{
	// �������غ���
	return AddItem(strContain.c_str());
}

/***************************************************************************************
��    �ܣ���ӿ��ٲ�����������
����˵����CBinary idContain ���ٲ���������ID
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ���������Ŀ���ݺ�����ֵ���Զ���Ϊ�մ�
**************************************************************************************/
short CShortTestShow::AddItem (CBinary idContain)
{
	// ����IDת�����ַ���
	string strContain = adsGetTextString(idContain);
	
	// �������غ���
	return AddItem(strContain);
}

/***************************************************************************************
��    �ܣ���ӻ��޸Ŀ��ٲ�������ֵ
����˵����CBirnay idValue ���ٲ�������ֵID
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������
**************************************************************************************/
short CShortTestShow::ChangeValue (CBinary idValue)
{
	// ����IDת�����ַ���
	string strValue = adsGetTextString(idValue);
	
	// �������غ���
	return ChangeValue(strValue);
}

/***************************************************************************************
��    �ܣ���ӻ��޸Ŀ��ٲ�������ֵ
����˵����string strValue ���ٲ�������ֵ
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������
**************************************************************************************/
short CShortTestShow::ChangeValue (string strValue)
{
	// ��������ֵ
	m_pSendAndReceive->Add(strValue);

	return true;
}

/***************************************************************************************
��    �ܣ���ӻ��޸Ŀ��ٲ�������ֵ
����˵����char* pValue ���ٲ�������ֵ
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������
**************************************************************************************/
short CShortTestShow::ChangeValue (const char* pValue)
{
	assert(NULL != pValue);

	// ��������ֵ
	m_pSendAndReceive->Add(pValue, strlen(pValue)+1);

	return true;
}

/***************************************************************************************
��    �ܣ���ʾ���ٲ��Բ������û�����
����˵������
�� �� ֵ���û�������ֵ
˵    ������ʾ����ȡ�����ݺ����������û�������ֵ��
**************************************************************************************/
#if 0
short CShortTestShow::Show ()
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

	// �˵��û�ѡ��
	short iUserKey = pchContain[iLenth-1];

 	return iUserKey;
}
#else

short CShortTestShow::Show ()
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

	// �˵��û�ѡ��
	short iUserKey = pchContain[4];

	if(adsIDSHORTTESTENTER == iUserKey)
	{
		m_nEnterIndex=(unsigned char)pchContain[5]*256 + (unsigned char)pchContain[6];
	}

 	return iUserKey;
}

#endif

short CShortTestShow::GetEnterIndex(void)
{
	return m_nEnterIndex;
}
