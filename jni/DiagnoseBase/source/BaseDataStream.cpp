/******************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�BaseDataStream.cpp
	�ĵ���ʶ��007�������ƽ̨��ϸ���˵����
	ժ    Ҫ���������Ķ�ȡ����ʾ�����	

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.01.11  zhangsh		1.0			�������ࡣ                                           

******************************************************************************/
#include "BaseDataStream.h"


CBaseDataStream::CBaseDataStream(void)
{
	m_bShowDataStreamSelectWindow = true;
}

inline CBaseDataStream::~CBaseDataStream(void)
{
}


/******************************************************************************
��    �ܣ�	�����Ƿ���ʾ������ѡ����
����˵����	bool bEnable �Ƿ���ʾ����ʾ��true ����ʾ��false 
�� �� ֵ��	��
˵    ����	m_ bShowDataStreamSelectWindow ��= bEnable
******************************************************************************/
void CBaseDataStream::EnableShowMultiSelected (bool bEnable = true)
{
	m_bShowDataStreamSelectWindow = bEnable;
}
