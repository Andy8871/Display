/******************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�SendFrame.cpp
	�ĵ���ʶ��007�������ƽ̨��ϸ���˵����(��Ϸֲ�)
	ժ    Ҫ������ͨ�������ࡣ

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2005.01.12  zhangsh   1.0			�������ࡣ                                           

******************************************************************************/

#include "SendFrame.h"
#include "Database.h"
#include "assert.h"
//#include "EcuCommException.h"



#define	NOCMDINLIB		-1
#define	CMDLIBERROR		-2

// ��̬��Ա��������
int CSendFrame::m_iDefaultReceiveFrameNumber = 1;


CSendFrame::CSendFrame(void)
{
}

CSendFrame::~CSendFrame(void)
{
}


/******************************************************************
��    �ܣ�	��ֵ
����˵����	const CSendFrame& SendFrame ����֡
�� �� ֵ��	��
˵    ����	��
******************************************************************/
void CSendFrame::operator = (CSendFrame SendFrame)
{
	m_SendFrame.clear();

	if(SendFrame.m_SendFrame.empty())
	{
		return;
	}

	vector<COneSendFrame>::iterator vIter;
	for(vIter = SendFrame.m_SendFrame.begin(); vIter != SendFrame.m_SendFrame.end(); vIter++)
	{
		m_SendFrame.push_back(*vIter);
	}
}


/******************************************************************
��    �ܣ�	��ֵ
����˵����	const vector<CBinary>& binCmd ���������������
�� �� ֵ��	��
˵    ����	������ż��λ��Ϊ�������λ��Ϊ����֡����
******************************************************************/
void CSendFrame::operator = (vector<CBinary>& binCmd)
{
	m_SendFrame.clear();

	if (binCmd.empty())
	{
		return;
	}

	COneSendFrame oneSendFrame;
	if (1 == binCmd.size())	 // ֻ��һ�������һظ�֡��ȱʡ
	{
		oneSendFrame.m_binSendCmd = binCmd[0];
		oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

		m_SendFrame.push_back(oneSendFrame);	
	}
	else
	{
		vector<CBinary>::size_type vIter;
		for (vIter = 0; vIter < binCmd.size(); vIter += 2)
		{
			oneSendFrame.m_binSendCmd = binCmd[vIter];
			oneSendFrame.m_iReceiveFrameNumber = (unsigned char)(binCmd[vIter+1])[0];

			m_SendFrame.push_back(oneSendFrame);
		}		
	}
}


/******************************************************************
��    �ܣ�	�����·���֡����
����˵����	const vector<CBinary>& binCmd ����������������
�� �� ֵ��	��
˵    ����	������ż��λ��Ϊ�������λ��Ϊ����֡����
******************************************************************/
void CSendFrame::operator += (vector<CBinary>& binCmd)
{
	if (binCmd.empty())
	{
		return;
	}

	COneSendFrame oneSendFrame;
	if(1 == binCmd.size())	//ֻ��һ�������һظ�֡��ȱʡ
	{
		oneSendFrame.m_binSendCmd = binCmd[0];
		oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

		m_SendFrame.push_back(oneSendFrame);	
	}
	else
	{
		vector<CBinary>::size_type vIter;
		for (vIter = 0; vIter < binCmd.size(); vIter += 2)
		{
			oneSendFrame.m_binSendCmd = binCmd[vIter];
			oneSendFrame.m_iReceiveFrameNumber = (unsigned char)(binCmd[vIter+1])[0];

			m_SendFrame.push_back(oneSendFrame);
		}
	}
}

/******************************************************************
��    �ܣ�	��ֵ
����˵����	const vector<CBinary>& binCmd ���������������
�� �� ֵ��	��
˵    ����	������ż��λ��Ϊ�������λ��Ϊ����֡����
******************************************************************/
void CSendFrame::operator = (const CBinary binData)
{
	m_SendFrame.clear();

	COneSendFrame oneSendFrame;

	oneSendFrame.m_binSendCmd = binData;
	oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

	m_SendFrame.push_back(oneSendFrame);	
}


/******************************************************************
��    �ܣ�	�����·���֡����
����˵����	const vector<CBinary>& binCmd ����������������
�� �� ֵ��	��
˵    ����	������ż��λ��Ϊ�������λ��Ϊ����֡����
******************************************************************/
void CSendFrame::operator += (CBinary binData)
{
	COneSendFrame oneSendFrame;

	oneSendFrame.m_binSendCmd = binData;
	oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

	m_SendFrame.push_back(oneSendFrame);	
}

/******************************************************************
��    �ܣ�	�����·���֡����
����˵����	const vector<CBinary>& binCmd ����������������
�� �� ֵ��	��
˵    ����	��
******************************************************************/
void CSendFrame::operator += (CSendFrame& SendFrame)
{
	if(SendFrame.m_SendFrame.empty())
	{
		return;
	}

	vector<COneSendFrame>::iterator vIter;
	for(vIter = SendFrame.m_SendFrame.begin(); vIter != SendFrame.m_SendFrame.end(); vIter++)
	{
		m_SendFrame.push_back(*vIter);
	}
}


/******************************************************************
��    �ܣ�	�����·���֡����
����˵����	COneSendFrame& oneSendFrameĳһ���������
�� �� ֵ��	��
˵    ����	��
******************************************************************/
void CSendFrame::operator += (COneSendFrame& oneSendFrame)
{
	if(oneSendFrame.m_binSendCmd.GetSize() < 1)
	{
		return;
	}

	m_SendFrame.push_back(oneSendFrame);
}

/******************************************************************
  ��    �ܣ�ȡ��һ������֡
  ����˵��������֡��� 
  �� �� ֵ��һ������֡
  ˵    ������
******************************************************************/
CSendFrame::COneSendFrame CSendFrame::operator [] (short nIndex)
{
	return m_SendFrame[nIndex];
}


/******************************************************************
��    �ܣ�	����ȱʡ����֡֡��
����˵����	short iDefaultReceiveFrame ȱʡ����֡֡����ȱʡֵΪ1
�� �� ֵ��	ǰ������ȱʡ����֡֡��
˵    ����	��
******************************************************************/
short	CSendFrame::SetDefaultReceiveFrameNumber (short iDefaultReceiveFrame)
{
	short preDefaultReceiveFrameNuber = m_iDefaultReceiveFrameNumber;
	m_iDefaultReceiveFrameNumber = iDefaultReceiveFrame;

	return preDefaultReceiveFrameNuber;
}


/******************************************************************
��    �ܣ�	���������ID��Ӧ�ķ���֡
����˵����	CBinary idCmd �����ID
�� �� ֵ��	�ɹ���
˵    ����	��
******************************************************************/
bool CSendFrame::AddFromCmdLib(CBinary idCmd)
{
	vector<CBinary> vBinCmd;	

	CDatabase dbCmd;
	
	if(!dbCmd.Open(CDatabase::DB_COMMAND))
		return false;

	vBinCmd = dbCmd.SearchId(idCmd);
	dbCmd.Close();

	if (vBinCmd.empty())
	{
		return false;
	}

	COneSendFrame oneSendFrame;
	if(vBinCmd.size() == 1)	//ֻ��һ�������һظ�֡��ȱʡ
	{
		oneSendFrame.m_binSendCmd = vBinCmd[0];
		oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

		m_SendFrame.push_back(oneSendFrame);	
	}
	else
	{
		// ��ͬһ��ID�а����������ʱ����ʡ��: ECU��Ӧ������֡��
		assert( vBinCmd.size()%2 == 0 );

		vector<CBinary>::iterator vIter;
		for (vIter = vBinCmd.begin(); vIter != vBinCmd.end(); vIter += 2)
		{
			oneSendFrame.m_binSendCmd = *vIter;
			oneSendFrame.m_iReceiveFrameNumber = (unsigned char)(vIter+1)->GetAt(0);

			m_SendFrame.push_back(oneSendFrame);
		}
	}

	return true;
}

/******************************************************************
��    �ܣ�	׷�ӷ���֡
����˵����	const CSendFrame& SendFrame ����֡
�� �� ֵ��	���������
˵    ����	��
******************************************************************/
short	CSendFrame::AddSendFrame(CSendFrame& SendFrame)
{
	if(SendFrame.m_SendFrame.empty())
	{
		return 0;
	}

	short nAddSize = 0;
	vector<COneSendFrame>::iterator vIter;
	for(vIter = SendFrame.m_SendFrame.begin(); vIter != SendFrame.m_SendFrame.end(); vIter++, nAddSize++)
	{
		m_SendFrame.push_back(*vIter);
	}

	return nAddSize;
}


/******************************************************************
��    �ܣ�	��շ���֡
����˵����	��
�� �� ֵ��	��
˵    ����	��
******************************************************************/
void CSendFrame::Clear (void)
{
	m_SendFrame.clear();
}


/******************************************************************
��    �ܣ�	ȡ�÷���֡����
����˵����	��
�� �� ֵ��	����֡����
˵    ����	��
******************************************************************/
short	CSendFrame::GetFrameNumber (void)
{
	short iFrameNumber =  m_SendFrame.size();
	return iFrameNumber;
}


/******************************************************************
��    �ܣ�	�õ�����֡�ķ���֡��
����˵����	��
�� �� ֵ��	����֡�ķ���֡��
˵    ����	��
******************************************************************/
short CSendFrame::GetAllReceiveFrameNumber(void)
{
	short recvFrameNum = 0;
	vector<COneSendFrame>::iterator iSendFrame;	
	for(iSendFrame = m_SendFrame.begin(); iSendFrame != m_SendFrame.end(); iSendFrame++)
	{
		recvFrameNum += iSendFrame->m_iReceiveFrameNumber;
	}

	return recvFrameNum;
}


/******************************************************************
��    �ܣ�	�õ�����֡�е�֡�ķ���֡��
����˵����	int iOrder����֡�е�֡��˳���
�� �� ֵ��	����֡�е�֡�ķ���֡��
˵    ����	��
******************************************************************/
short CSendFrame::GetOneReceiveFrameNumber(int iOrder)
{
	return m_SendFrame[iOrder].m_iReceiveFrameNumber;
}


/**************************************************************************************
��    �ܣ�	������֡�������ݰ���[����ȣ�����+�ظ�����]...���ĸ�ʽ�����ָ���Ļ�����
����˵����	��
�� �� ֵ��	��������䳤��
˵    ����	��
**************************************************************************************/
int CSendFrame::PutCmdDataToBuffer(unsigned char* dataBuf, int bufLength)
{
	if (m_SendFrame.size() == 0)
	{
		return 0;
	}

	int dataLength = 0;
	vector<COneSendFrame>::iterator vIter;

	for (vIter = m_SendFrame.begin(); vIter < m_SendFrame.end(); vIter++)
	{
		CBinary cmdBin = vIter->m_binSendCmd;

		dataBuf[dataLength] = (unsigned char)cmdBin.GetSize();
		dataLength++;

		char* pbuf = (char*)cmdBin.GetBuffer();
		memcpy(dataBuf+dataLength, pbuf, cmdBin.GetSize());
		dataLength += cmdBin.GetSize();

		dataBuf[dataLength] = (unsigned char)vIter->m_iReceiveFrameNumber;
		dataLength++;
	}

	return dataLength;
}