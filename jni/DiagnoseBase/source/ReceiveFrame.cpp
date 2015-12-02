/******************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�ReceiveFrame.cpp
	�ĵ���ʶ��007�������ƽ̨��ϸ���˵����
	ժ    Ҫ���������ڴ�ȡ����ECU���ص�����

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.01.03  zhangsh		1.0			�������ࡣ                                           

******************************************************************************/
#include <assert.h>
#include "receiveframe.h"



CReceiveFrame::CReceiveFrame(void)
{
	m_ReceiveFrame.clear();
}

CReceiveFrame::~CReceiveFrame(void)
{
}

/*****************************************************************
��    �ܣ����ƽ���֡
����˵����const CReceiveFrame& ReceiveFrame ����֡��
�� �� ֵ����
˵    ������
*****************************************************************/
void CReceiveFrame::operator = (CReceiveFrame ReceiveFrame )
{
	CBinaryGroup groupBin;
	vector< CBinaryGroup >::iterator vIter;

	m_ReceiveFrame.clear();
	for(vIter = ReceiveFrame.m_ReceiveFrame.begin(); vIter != ReceiveFrame.m_ReceiveFrame.end(); vIter++)
	{
		groupBin.assign(vIter->begin(), vIter->end());
		m_ReceiveFrame.push_back(groupBin);
	}
	m_CanParseInfo = ReceiveFrame.GetCanParseInfo();
}


/*****************************************************************
��    �ܣ������·���֡����
����˵����CBinaryGroup& ReceiveGroup ����������
�� �� ֵ����
˵    ������
*****************************************************************/
void CReceiveFrame::operator += ( CBinaryGroup& ReceiveGroup )
{
	m_ReceiveFrame.push_back(ReceiveGroup);
}


/*****************************************************************
��    �ܣ�ȡ��ĳһ����֡��Ӧ��֡
����˵��������֡���
�� �� ֵ���÷���֡��Ӧ�Ľ���֡���������
˵    ������
*****************************************************************/
CBinaryGroup& CReceiveFrame::operator[] (const short iFrameOrder)
{
	assert( (0 <= iFrameOrder) && (iFrameOrder < (short)m_ReceiveFrame.size()) );

	return m_ReceiveFrame[iFrameOrder];
}


/*****************************************************************
��    �ܣ�ȡ�÷���֡������
����˵������
�� �� ֵ������֡������
˵    ������
*****************************************************************/
short CReceiveFrame::GetSendFrameNumber (void)
{
	short iSize = m_ReceiveFrame.size();
	return iSize;
}


///*****************************************************************
//��    �ܣ����÷���֡������
//����˵����unsigned short iSendFrameNumber����֡������
//�� �� ֵ���������
//˵    ������
//*****************************************************************/
//short CReceiveFrame::SetSendFrameNumber (short iSendFrameNumber)
//{
//	short iRet = 0;
//
//	return iRet;
//}
//

/*****************************************************************
��    �ܣ�ȡ�õ�һ����֡��Ӧ��֡
����˵����
�� �� ֵ��
˵    ������
*****************************************************************/
CBinary CReceiveFrame::GetFirstAnswerFrame()
{
	CBinary binReceive;

	if(0 == m_ReceiveFrame.size())
	{
		return binReceive;
	}

	if(0 == m_ReceiveFrame[0].size())
	{
		return binReceive;
	}

	return m_ReceiveFrame[0][0];
}


/*****************************************************************
��    �ܣ�ȡ��ĳһ����֡��Ӧ��֡
����˵��������֡���
�� �� ֵ���÷���֡��Ӧ�Ľ���֡���������
˵    ������
*****************************************************************/
CBinaryGroup& CReceiveFrame::GetOneFrameAnswer (const short iFrameOrder)
{
	assert( (0<=iFrameOrder) && (iFrameOrder < (short)m_ReceiveFrame.size()) );

	return m_ReceiveFrame[iFrameOrder];
}


/*****************************************************************
��    �ܣ����ؽ���֡������ߴ�
����˵������
�� �� ֵ������֡����ĳߴ�
˵    ������
*****************************************************************/
short CReceiveFrame::size()
{
	short iSize = m_ReceiveFrame.size();
	return iSize;
}


/*****************************************************************
��    �ܣ���ս���֡����
����˵������
�� �� ֵ����
˵    ������
*****************************************************************/
void CReceiveFrame::clear()
{
	m_ReceiveFrame.clear();
}
