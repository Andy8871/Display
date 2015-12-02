/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�	BaseDisplay.cpp
	�ĵ���ʶ��	007�������ƽ̨��ϸ���˵����
	ժ    Ҫ��	��ʾ����

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			��������ڡ�                                           

**************************************************************************************/

#include "BaseDisplay.h"
#include "assert.h"

CBaseDisplay::CBaseDisplay() 
{ 
	m_pSendAndReceive = NULL; 	
}
	
CBaseDisplay::~CBaseDisplay()
{
 
}

void CBaseDisplay::SetCommunicationPointer(CCommunicationWithDisplay* pSendAndReceive)
{
	assert(NULL != pSendAndReceive);
	m_pSendAndReceive = pSendAndReceive;
}



