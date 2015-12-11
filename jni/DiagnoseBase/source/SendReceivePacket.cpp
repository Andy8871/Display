/******************************************************************************

 Copyright (c) 2005, AUTOBOSS Inc.
 All rights reserved.

 �ļ����ƣ�	SendReceivePacket.cpp
 �ĵ���ʶ��	007�������ƽ̨��ϸ���˵����
 ժ����Ҫ��	������������ݰ���������λ��Ӧ�����
 �������������λ��ͨ��

 ��ʷ��¼��
 ----------------------------------------------------------------------------
 ʱ     ��   ����		�汾��		��    ��    ��    ��
 ----------------------------------------------------------------------------
 2006.01.13	zhangsh		1.0				�������ࡣ

 ******************************************************************************/
//#define  WIN32_LEAN_AND_MEAN
#include "ProFile.h"
#include "EcuCommException.h"
#include "CommWithEcu.h"
#include "SendReceivePacket.h"
#include "RunEnvironment.h"
#include "../../LinkLayer/include/Linklayer.h"
#include <LogCat.h>
#include <time.h>

#ifndef WIN32
void sleep(clock_t wait)
{
	clock_t goal;
	goal = wait + clock();
	while (goal > clock())
	{
		;
	}
}
#endif
#define TAG "CSendReceivePacket"
CSendReceivePacket::CSendReceivePacket(void)
{
	m_iTimeout = 1000; // ��ʱʱ��
	m_uiPacketID = 0;
	m_pCommPort = CreatInstance(2);
}

CSendReceivePacket::~CSendReceivePacket(void)
{
	if (m_pCommPort != NULL)
	{
		delete m_pCommPort;
		m_pCommPort = NULL;
	}
}
/*******************************************************************************
 �������ܣ�	���������е����ݴ��������
 ����˵����	unsigned char* pSendBuffer �������ݵ�ַָ��,
 int iSendLength            ���ݳ��ȣ����������ݰ�ID
 �� �� ֵ��	���͵ĳ���
 ˵    ����	��
 *******************************************************************************/
int CSendReceivePacket::SendData(unsigned char* pSendBuffer, int iSendLength)
{
	unsigned char CmdPack[512];
	unsigned char RecvPack[512];
	unsigned char fifo[2];
	fifo[0] = 0;
	fifo[1] = 0;

	if (CRunEnvironment::GetDemoMode())
		return iSendLength; //add by yxp 2007.11.20

	if (NULL == m_pCommPort)
	{
		return 0;
	}

	m_uiPacketID++;

	// ��䷢�Ͱ�
	//CmdPack[0] = 0xCF;									// ��ʼ��־��0xCF33
	//CmdPack[1] = 0x33;
	//modified by andy huang 2015.5.27
	//��ʼ��־��Ϊ0xAA55
	CmdPack[0] = HEAD_FLAG1; // ��ʼ��־��0xAA55
	CmdPack[1] = HEAD_FLAG2;
	CmdPack[2] = (unsigned char) (iSendLength >> 8); // ���ݰ����ݳ��ȣ����ȸ��ֽ�
	CmdPack[3] = (unsigned char) (iSendLength); // ���ݰ����ݳ��ȣ����ȵ��ֽ�
	CmdPack[4] = ~CmdPack[2]; // ���ݰ����ݳ��ȣ����ȸ��ֽ�ȡ��
	CmdPack[5] = ~CmdPack[3]; // ���ݰ����ݳ��ȣ����ȵ��ֽ�ȡ��
	CmdPack[6] = m_uiPacketID; // ���ݰ����

	memcpy(CmdPack + 7, pSendBuffer, iSendLength); // ���͵���������

	unsigned char bySum = 0;
	int iIndex = 0;
	for (iIndex = 0; iIndex < (iSendLength + 7); iIndex++)
	{
		bySum += CmdPack[iIndex]; // ��У���
	}

	CmdPack[iSendLength + 7] = bySum; // У���

	int nErrorNo = m_pCommPort->Send(CmdPack, iSendLength + 8); // ��������֡
	if (nErrorNo != (iSendLength + 8))
	{
		LOG_DEBUG(TAG, "Send failed %s", strerror(nErrorNo));
	}
	else
	{
		string strCmd;
		for (int i = 0; i < iSendLength + 8; ++i)
		{
			char pBuff[8];
			memset(pBuff, 0, 8);
			sprintf(pBuff, "0x%02x ", CmdPack[i]);
			strCmd += pBuff;
		}
		LOG_DEBUG(TAG, "Send:%s", strCmd.c_str());
	}
	return iSendLength;
// ����������
///////////////////////////////////////////////////////////////////////////////////////////////

//����WifiͨѶ����,ȥ��ȷ��֡
#if 0

	int iReceived = 0;
#ifdef WIN32//hpy for ios
	unsigned long start;
	unsigned long end;
#else
	clock_t start; //hpy for ios
	clock_t end;//hpy for ios
#endif
	bool bDisconnected = false;
	//bool bReconnect = false;

	// �����ǽ�������
	unsigned char chBuffer;
	int iTryCnt = 3;
	if(CRunEnvironment::GetDemoMode() == true)
	{
		iTryCnt = 1;
	}

	for (iIndex = 0; iIndex < iTryCnt; iIndex++) // ��෢������
	{
		if (bDisconnected)
		{
			bDisconnected = false;
		}

		m_pCommPort->Send(CmdPack, iSendLength+8); // ��������֡

		bool bHeadFind = false;
		int iPosition = 0;
		bySum = 0;
#ifdef WIN32//hpy for ios
		start = GetTickCount();//1000*sysTime.wSecond + sysTime.wMilliseconds;
#else
		start = clock(); //hpy for ios
#endif
		while(true)
		{
#ifdef WIN32//hpy for ios
			end = GetTickCount(); //1000*sysTime.wSecond + sysTime.wMilliseconds;
			if((end-start) > m_iTimeout)
#else
			end = clock();
			if((end-start)*1000/CLOCKS_PER_SEC > m_iTimeout)
#endif
			{
				break;
			}
			iReceived = m_pCommPort->Receive(&chBuffer, 1); // ��������
			if(iReceived != 1)
			{
				continue;
			}

			fifo[0]=fifo[1];
			fifo[1]=chBuffer;

			if(!bHeadFind)
			{

//                 if(fifo[0]==0xcf && fifo[1]==0x33)
// 				{
// 					RecvPack[0] = 0xCF;
// 					RecvPack[1] = 0x33;
// 					bHeadFind = true;
// 					iPosition = 2;
// 				}
				//modified by andy huang 2015.5.27
				if(fifo[0]==HEAD_FLAG1 && fifo[1]==HEAD_FLAG2)
				{
					RecvPack[0] = 0xAA;
					RecvPack[1] = 0x55;
					bHeadFind = true;
					iPosition = 2;
				}
				continue;
			}

			RecvPack[iPosition] = chBuffer;

			iPosition++;

			if(9 == iPosition) //Ӧ��֡����
			{
				// ��鳤��
				short length = (int)((RecvPack[2]<<8) + RecvPack[3]);
				if (length != 1)
				{ // ���ݳ���ӦΪ1,  ���ݰ����ݡ�
					bHeadFind = false;// 9/27/2006 by wcx
					continue;//wcx 2006.6.30
//					break;
				}
				// ��鳤�ȱ�־
				if ( RecvPack[2] != (unsigned char)(~RecvPack[4]) || RecvPack[3] != (unsigned char)(~RecvPack[5]) )
				{
					bHeadFind = false; // 9/27/2006 by wcx
					continue;//wcx 2006.6.30
//					break;
				}
				//��鷵������:��ȷ���͵�ECHOΪ���ݰ�IDȡ����
				if (RecvPack[6] != m_uiPacketID || m_uiPacketID != (unsigned char)(~RecvPack[7]) )
				{
					bHeadFind = false; // 9/27/2006 by wcx
//					break;
					iPosition = 0;//2006.11.10;
					continue;
				}
				//���У���
				bySum = 0;
				for(int i = 0; i < iPosition-1; i++)
				{
					bySum += RecvPack[i];
				}

				if ( bySum != RecvPack[8] )
				break;

				return iSendLength; // ������ȷ���͵ĳ���
			} // end if  

		} // end while
		bDisconnected = true;
	} // end for

// ���淢�ͽ���
///////////////////////////////////////////////////////////////////////////////////////////////
	return 0;
#endif
}

/*******************************************************************************
 ��    �ܣ�	����һ�����ݣ����ظ��ջص�֡
 ����˵����	unsigned char* pSendBuffer �������ݻ�����, int iBufferLength����������
 �� �� ֵ��	�յ����ݳ��ȣ���С��0��Ϊ�������
 ˵    ����	��
 *******************************************************************************/
extern CReceiveFrame g_rfDemo;
extern unsigned char g_bDemoFlag;
int CSendReceivePacket::ReceiveData(unsigned char* pReceiveBuffer,
		int iBufferLength)
{
	int iPosition = 0; // �յ���Ч���ݵĳ���
	//int iLenth         = 0;       // ÿ���յ�������
	int iReceiveLength = 0; // ���հ����ݳ���
	unsigned char bySum = 0; // У���

	// �����ַ�����
	unsigned char chBuffer;
	unsigned char fifo[2];
	fifo[0] = 0;
	fifo[1] = 0;

	// ����ͷ��־
	bool bFindHead = false;
	int iRecieved = 0;

#ifdef WIN32
	unsigned long start;
	unsigned long end;
#else
	clock_t start;
	clock_t end;
#endif
	/*
	 if(g_pNet&&g_pNet->IsSlaver())iReceiveLength=g_pNet->ReceiveData(pReceiveBuffer+7,iBufferLength);//add by yxp 2007.4.2
	 if(g_pNet&&g_pNet->IsSlaver())return iReceiveLength;//add by yxp 2007.4.2
	 */
	if (CRunEnvironment::GetDemoMode())
	{
		CBinary bin;
		unsigned char *p = pReceiveBuffer + 7;
		*p++ = 0;
		if (g_bDemoFlag == 2)
			*p++ = g_rfDemo.size(); //����
		else if (g_bDemoFlag == 1)
			*p++ = 0;
		for (int i = 0; i < g_rfDemo.size(); i++)
		{
			if (g_bDemoFlag != 4)
				*p++ = g_rfDemo[i].size(); //����
			for (int j = 0; j < g_rfDemo[i].size(); j++)
			{
				bin = g_rfDemo[i][j];
				*p++ = bin.GetSize(); //����
				memcpy(p, bin.GetBuffer(), bin.GetSize()); //����
				p += bin.GetSize();
			}
		}
		iReceiveLength = p - (pReceiveBuffer + 7);
		if (iReceiveLength < 2)
			iReceiveLength = 2;
		g_rfDemo.clear();
		g_bDemoFlag = 1;
		/*
		 if(g_pNet&&g_pNet->IsMaster())g_pNet->ReceiveData(pReceiveBuffer+7,iReceiveLength);
		 */
#ifdef WIN32//hpy for ios
		Sleep(iReceiveLength);
#else
		sleep((clock_t) iReceiveLength * CLOCKS_PER_SEC / 1000);
#endif        
		return iReceiveLength;
	}

#ifdef WIN32
	start = GetTickCount(); //1000*sysTime.wSecond + sysTime.wMilliseconds;
#else
	start = clock();
#endif

	while (true)
	{
#ifdef WIN32
		end = GetTickCount(); //1000*sysTime.wSecond + sysTime.wMilliseconds;
		if((end-start) > m_iTimeout)
#else
		end = clock(); //hpy for ios
		if ((end - start) * 1000 / CLOCKS_PER_SEC > m_iTimeout) //hpy for ios
#endif
		{
			break;
		}

		iRecieved = m_pCommPort->Receive(&chBuffer, 1);
		//__android_log_print(ANDROID_LOG_DEBUG, TAG, "Receive:0x%02x", chBuffer);
		if (iRecieved != 1)
		{
			continue;
		}

		fifo[0] = fifo[1];
		fifo[1] = chBuffer;
		if (!bFindHead)
		{
			//modified by andy huang 2015.5.27
			if (fifo[0] == HEAD_FLAG1 && fifo[1] == HEAD_FLAG2)
			{
				pReceiveBuffer[0] = HEAD_FLAG1;
				pReceiveBuffer[1] = HEAD_FLAG2;

				iPosition = 2;
				bFindHead = true;
			}
			continue;
		}

		pReceiveBuffer[iPosition] = chBuffer;
//		bySum += chBuffer;  // ����У���
		iPosition++;

		// ����֡ͷ�ȼ��֡ͷ����ȷ��
		if (7 == iPosition)
		{
			unsigned char iHigh = (unsigned char) pReceiveBuffer[2]; // ���ݳ��ȵĸ��ֽڲ���
			unsigned char iLow = (unsigned char) pReceiveBuffer[3]; // ���ݳ��ȵĵ��ֽڲ���

			iReceiveLength = iHigh * 256 + iLow;

			// ���彫���
			if (iReceiveLength > iBufferLength)
			{
				return EC_BUFFER_OVER;
			}

			// ������ݳ���
			if (iReceiveLength < 1)
			{
				bFindHead = false;
				continue;
			}

			// ��鳤�ȱ�־
			if ((pReceiveBuffer[2] != (unsigned char) ~pReceiveBuffer[4])
					|| (pReceiveBuffer[3] != (unsigned char) ~pReceiveBuffer[5]))
			{
				bFindHead = false;
				continue;
			}

			// ����Ƿ���Ҫȡ�����ݰ�
			if (pReceiveBuffer[6] != m_uiPacketID)
			{
				bFindHead = false;
				continue;
			}
		}

		// ���ݰ�����ΪiReceiveLength + 8, ���1���ֽ�ΪУ���
		if (iPosition == (iReceiveLength + 8))
		{
			// ���У���
			bySum = 0;
			for (int i = 0; i < iPosition - 1; i++)
			{
				bySum += pReceiveBuffer[i];
			}

			if (bySum != pReceiveBuffer[iPosition - 1])
			{
				bFindHead = false;
				continue;
			}

			// �ж��Ƿ�Ϊ�ȴ�֡
			if ((iReceiveLength == 4) && (pReceiveBuffer[5] == 0xFF)
					&& (pReceiveBuffer[6] == 0xFF))
			{
				bFindHead = false;
#ifdef WIN32
				start = GetTickCount(); //1000*sysTime.wSecond + sysTime.wMilliseconds;
#else
				start = clock(); //hpy for ios
#endif
				continue;
			}
				string strCmd;
				for (int i = 0; i < iReceiveLength + 8; ++i)
				{
					char pBuff[8];
					memset(pBuff, 0, 8);
					sprintf(pBuff, "0x%02x ", pReceiveBuffer[i]);
					strCmd += pBuff;
				}
				//char* pBuff = new char[iSendLength + 8];
				LOG_DEBUG(TAG, "Receive data[%d]:%s", iReceiveLength, strCmd.c_str());
				/*__android_log_print(ANDROID_LOG_DEBUG, TAG, "Receive:%s", strCmd.c_str());*/
			return iReceiveLength;
		}
	}
	LOG_DEBUG(TAG, "Receive data failed!!!");
	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "Receive data failed!!!");
	return -1;
}

/*******************************************************************************
 ��    �ܣ���ȡ�����ļ���������λ��ͨ�Ŷ˿�
 ����˵������
 �� �� ֵ���ɹ���true��ʧ�ܡ�false
 ˵    ������
 *******************************************************************************/
bool CSendReceivePacket::OpenPort(void)
{
	bool bRet = false;

	if (CRunEnvironment::GetDemoMode())
	{
		bRet = true;
	}
	else
	{
		/**
		 * 0  	δ֪ͨѶ��ʽ
		 * 1 	����ͨѶ��ʽ
		 * 2	WifiͨѶ��ʽ
		 */
		bRet = m_pCommPort->Init(2);
		if (bRet)
			LOG_DEBUG(TAG, "OpenPort successfully");
		else
			LOG_DEBUG(TAG, "OpenPort failed");
	}

	return bRet;
}

/*******************************************************************************
 ��    �ܣ���˿��Ѵ���رգ������޶���
 ����˵������
 �� �� ֵ����
 ˵    ������
 *******************************************************************************/
void CSendReceivePacket::ClosePort(void)
{
	m_pCommPort->Close();
}

/*******************************************************************************
 ��    �ܣ����ó�ʱʱ��
 ����˵������
 �� �� ֵ��
 ˵    �����ú���ֻ������ϳ����в�ʹ��
 *******************************************************************************/
void CSendReceivePacket::SetTimeout(int msTimeout)
{
	m_iTimeout = msTimeout;
	/*
	 if(g_pNet)g_pNet->SetTimeout(msTimeout);
	 */
}

bool CSendReceivePacket::ClearBuffer()
{
	return m_pCommPort->ClearBuffer();
}

