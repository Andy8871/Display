/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�	VehicleInfo.cpp
	�ĵ���ʶ��	007�������ƽ̨��ϸ���˵����
	ժ    Ҫ��	��ʾ��ĳ�Ա������������ɳ���ECU�汾��Ϣ��ʾ����
	��    �ܣ�
				1��	������ECU�汾��Ϣ������ݰ��涨��ʽд�뻺������֪ͨ��ʾ����ȡ�����ݲ���ʾ��
				2��	���û��ӡ�OK������ʾ����Ż��ͽ����

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			�������ࡣ                                           

**************************************************************************************/


#include "VehicleInfo.h"
#include "Database.h"
#include "assert.h"
#include "LogCat.h"

//����ID���ı��ⷵ���ַ���
extern string adsGetTextString (CBinary binTextID);


/***************************************************************************************
��    �ܣ���ʼ���汾��Ϣѡ���
����˵����char* pTitle �汾��Ϣѡ�����⣬��Ϊ����ʾ����ʹ��ȱʡ����
�� �� ֵ����
˵    ������
**************************************************************************************/
void CVehicleInfo::Init (const char* pTitle)
{
	assert(NULL != pTitle);

	char chFuction = DT_VEHICLE_INFORMATION;
	m_pSendAndReceive->Init(chFuction);  // ��书�ܴ���͵��Ա�־
	
	// ������Ϣ��������
	short iContainNum = 0;
	m_pSendAndReceive->Add(iContainNum);

	// ���������
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
}

/***************************************************************************************
��    �ܣ���ʼ���汾��Ϣѡ���
����˵����string strTitle �汾��Ϣѡ������
�� �� ֵ����
˵    ������
**************************************************************************************/
void CVehicleInfo::Init (string strTitle)
{
	// �������غ���
	Init(strTitle.c_str());
}

bool CVehicleInfo::SetFlag(unsigned char byFlag)
{
	m_pSendAndReceive->SetFlag(byFlag);
	return true;
}

/***************************************************************************************
��    �ܣ���ʼ���汾��Ϣѡ���
����˵����CBinary idTitle �汾��Ϣѡ������
�� �� ֵ����
˵    ������
**************************************************************************************/
void CVehicleInfo::Init (CBinary idTitle)
{
	// ����IDת�����ַ���
	string strTitle = adsGetTextString(idTitle);

	// �������غ���
	Init(strTitle);
}

/***************************************************************************************
��    �ܣ��汾��Ϣ������
����˵����string strExplain �汾��Ϣ��Ŀ��ʾ����ID
		  string strContain �汾��Ϣ��Ŀ��������
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������
**************************************************************************************/
short CVehicleInfo::Add (string strExplain, string strContain)
{
	// ��ʾ�ı���
	m_pSendAndReceive->Add(strExplain);

	// �����ı���
	m_pSendAndReceive->Add(strContain);

	// ����������������
	m_pSendAndReceive->IncreaseNum(4);

	return true;
}

/***************************************************************************************
��    �ܣ��汾��Ϣ������
����˵����CBinary idExplain�汾��Ϣ��Ŀ��ʾ����ID
string strContain �汾��Ϣ��Ŀ��������
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������
**************************************************************************************/
short CVehicleInfo::Add (CBinary idExplain, string strContain)
{
	// ����IDת�����ַ���
	string strExplain = adsGetTextString(idExplain);

	// �������غ���
	return Add (strExplain, strContain);	
}

/***************************************************************************************
��    �ܣ��汾��Ϣ��ȡ���û�����
����˵������
�� �� ֵ���û��������
˵    �������û�����OK��֮��Ի����û��������
**************************************************************************************/
short CVehicleInfo::Show ()
{
	LOG_I("CVehicleInfo", "Show vehicle information");
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

	// ��5���ֽڱ�ʾ:�˵��û�ѡ��
	short iUserKey = pchContain[4];

 	return iUserKey;
}

