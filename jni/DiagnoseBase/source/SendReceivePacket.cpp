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
	m_iTimeout = 1000; // 超时时间
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
 功　　能：	将缓冲区中的数据打包并发送
 参数说明：	unsigned char* pSendBuffer 发送数据地址指针,
 int iSendLength            数据长度，不包括数据包ID
 返 回 值：	发送的长度
 说    明：	无
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

	// 填充发送包
	//CmdPack[0] = 0xCF;									// 起始标志：0xCF33
	//CmdPack[1] = 0x33;
	//modified by andy huang 2015.5.27
	//起始标志改为0xAA55
	CmdPack[0] = HEAD_FLAG1; // 起始标志：0xAA55
	CmdPack[1] = HEAD_FLAG2;
	CmdPack[2] = (unsigned char) (iSendLength >> 8); // 数据包内容长度：长度高字节
	CmdPack[3] = (unsigned char) (iSendLength); // 数据包内容长度：长度低字节
	CmdPack[4] = ~CmdPack[2]; // 数据包内容长度：长度高字节取反
	CmdPack[5] = ~CmdPack[3]; // 数据包内容长度：长度低字节取反
	CmdPack[6] = m_uiPacketID; // 数据包编号

	memcpy(CmdPack + 7, pSendBuffer, iSendLength); // 发送的数据内容

	unsigned char bySum = 0;
	int iIndex = 0;
	for (iIndex = 0; iIndex < (iSendLength + 7); iIndex++)
	{
		bySum += CmdPack[iIndex]; // 求校验和
	}

	CmdPack[iSendLength + 7] = bySum; // 校验和

	int nErrorNo = m_pCommPort->Send(CmdPack, iSendLength + 8); // 发送数据帧
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
// 上面打包结束
///////////////////////////////////////////////////////////////////////////////////////////////

//减少Wifi通讯负荷,去掉确认帧
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

	// 下面是接收数据
	unsigned char chBuffer;
	int iTryCnt = 3;
	if(CRunEnvironment::GetDemoMode() == true)
	{
		iTryCnt = 1;
	}

	for (iIndex = 0; iIndex < iTryCnt; iIndex++) // 最多发送三次
	{
		if (bDisconnected)
		{
			bDisconnected = false;
		}

		m_pCommPort->Send(CmdPack, iSendLength+8); // 发送数据帧

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
			iReceived = m_pCommPort->Receive(&chBuffer, 1); // 接收数据
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

			if(9 == iPosition) //应答帧收完
			{
				// 检查长度
				short length = (int)((RecvPack[2]<<8) + RecvPack[3]);
				if (length != 1)
				{ // 内容长度应为1,  数据包内容〉
					bHeadFind = false;// 9/27/2006 by wcx
					continue;//wcx 2006.6.30
//					break;
				}
				// 检查长度标志
				if ( RecvPack[2] != (unsigned char)(~RecvPack[4]) || RecvPack[3] != (unsigned char)(~RecvPack[5]) )
				{
					bHeadFind = false; // 9/27/2006 by wcx
					continue;//wcx 2006.6.30
//					break;
				}
				//检查返回内容:正确发送的ECHO为数据包ID取返回
				if (RecvPack[6] != m_uiPacketID || m_uiPacketID != (unsigned char)(~RecvPack[7]) )
				{
					bHeadFind = false; // 9/27/2006 by wcx
//					break;
					iPosition = 0;//2006.11.10;
					continue;
				}
				//检查校验和
				bySum = 0;
				for(int i = 0; i < iPosition-1; i++)
				{
					bySum += RecvPack[i];
				}

				if ( bySum != RecvPack[8] )
				break;

				return iSendLength; // 返回正确发送的长度
			} // end if  

		} // end while
		bDisconnected = true;
	} // end for

// 上面发送结束
///////////////////////////////////////////////////////////////////////////////////////////////
	return 0;
#endif
}

/*******************************************************************************
 功    能：	接收一包数据，并回复收回到帧
 参数说明：	unsigned char* pSendBuffer 接收数据缓冲区, int iBufferLength缓冲区长度
 返 回 值：	收到数据长度，如小于0则为错误代码
 说    明：	无
 *******************************************************************************/
extern CReceiveFrame g_rfDemo;
extern unsigned char g_bDemoFlag;
int CSendReceivePacket::ReceiveData(unsigned char* pReceiveBuffer,
		int iBufferLength)
{
	int iPosition = 0; // 收到有效数据的长度
	//int iLenth         = 0;       // 每次收到的数据
	int iReceiveLength = 0; // 接收包数据长度
	unsigned char bySum = 0; // 校验和

	// 接收字符缓冲
	unsigned char chBuffer;
	unsigned char fifo[2];
	fifo[0] = 0;
	fifo[1] = 0;

	// 数据头标志
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
			*p++ = g_rfDemo.size(); //组数
		else if (g_bDemoFlag == 1)
			*p++ = 0;
		for (int i = 0; i < g_rfDemo.size(); i++)
		{
			if (g_bDemoFlag != 4)
				*p++ = g_rfDemo[i].size(); //贞数
			for (int j = 0; j < g_rfDemo[i].size(); j++)
			{
				bin = g_rfDemo[i][j];
				*p++ = bin.GetSize(); //长度
				memcpy(p, bin.GetBuffer(), bin.GetSize()); //数据
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
//		bySum += chBuffer;  // 计算校验和
		iPosition++;

		// 收完帧头先检测帧头的正确性
		if (7 == iPosition)
		{
			unsigned char iHigh = (unsigned char) pReceiveBuffer[2]; // 数据长度的高字节部分
			unsigned char iLow = (unsigned char) pReceiveBuffer[3]; // 数据长度的低字节部分

			iReceiveLength = iHigh * 256 + iLow;

			// 缓冲将溢出
			if (iReceiveLength > iBufferLength)
			{
				return EC_BUFFER_OVER;
			}

			// 检查内容长度
			if (iReceiveLength < 1)
			{
				bFindHead = false;
				continue;
			}

			// 检查长度标志
			if ((pReceiveBuffer[2] != (unsigned char) ~pReceiveBuffer[4])
					|| (pReceiveBuffer[3] != (unsigned char) ~pReceiveBuffer[5]))
			{
				bFindHead = false;
				continue;
			}

			// 检查是否是要取的数据包
			if (pReceiveBuffer[6] != m_uiPacketID)
			{
				bFindHead = false;
				continue;
			}
		}

		// 数据包长度为iReceiveLength + 8, 最后1个字节为校验和
		if (iPosition == (iReceiveLength + 8))
		{
			// 检查校验和
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

			// 判断是否为等待帧
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
 功    能：读取配置文件，打开与下位机通信端口
 参数说明：无
 返 回 值：成功—true，失败—false
 说    明：无
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
		 * 0  	未知通讯方式
		 * 1 	串口通讯方式
		 * 2	Wifi通讯方式
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
 功    能：如端口已打开则关闭，否则无动作
 参数说明：无
 返 回 值：无
 说    明：无
 *******************************************************************************/
void CSendReceivePacket::ClosePort(void)
{
	m_pCommPort->Close();
}

/*******************************************************************************
 功    能：设置超时时间
 参数说明：无
 返 回 值：
 说    明：该函数只有在诊断程序中才使用
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

