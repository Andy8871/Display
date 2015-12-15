#include <pthread.h>
#include "assert.h"
#include "EcuCommException.h"
#include "CommWithEcu.h"
#include "RunEnvironment.h"
#include "LogCat.h"

/*
 #ifndef WIN32
 //#include <sys/semaphore.h>
 #endif
 */
//#include "Net.h"
#ifdef DATA_DRV_EXT
#include "RunLog.h"
#endif
#define	COMMBUFSIZE		0xFFFF

// 接收的数据到“包内容”的偏移
#define MOVESIZE        8

#ifdef WIN32
#include "Display.h"
extern CDisplay * g_pDisplay;
#endif

#define TAG  "CCommWithEcu"
/*CCommWithEcu * g_pCommWithEcu;
CCommWithEcu* getCommWithEcuObject()
{
	g_pCommWithEcu = new CCommWithEcu();
	return g_pCommWithEcu;
}*/

/********************************************************************
 功    能：	1、建立与下位机的连接；
 2、分配通信缓冲区。
 参数说明：	无
 返 回 值：	错误代码
 说    明：	无
 ********************************************************************/
CReceiveFrame g_rfDemo;
unsigned char g_bDemoFlag = 1;
unsigned char g_bProtocol = 1;


CCommWithEcu* CCommWithEcu::m_pInstance = NULL;

CCommWithEcu* CCommWithEcu::GetInstance()
{
	pthread_mutex_t mutex_t = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&mutex_t, NULL);
	if (NULL == m_pInstance)
	{
		pthread_mutex_lock(&mutex_t);
		if (NULL == m_pInstance)
			m_pInstance = new CCommWithEcu();
		pthread_mutex_unlock(&mutex_t);
	}
	pthread_mutex_destroy(&mutex_t);
	return m_pInstance;
}

void CCommWithEcu::SetDemoFrame(CBinaryGroup &bg)
{
	if (CRunEnvironment::GetDemoMode())
	{
		g_rfDemo.clear();
		g_rfDemo += bg;
		bg.clear();
	}
}
void CCommWithEcu::Init(void)
{
	m_SendRecv = new CSendReceivePacket;
	assert(m_SendRecv != NULL);

	m_SendCommBuf = (unsigned char*) new unsigned char[COMMBUFSIZE];
	assert(m_SendCommBuf != NULL);
	memset(m_SendCommBuf, 0, COMMBUFSIZE);

	m_RecieveCommBuf = (unsigned char*) new unsigned char[COMMBUFSIZE];
	assert(m_RecieveCommBuf != NULL);
	memset(m_RecieveCommBuf, 0, COMMBUFSIZE);
	m_SendRecv->OpenPort();
#ifdef NEW_DRV
	m_ctlCmd = SF_CTRCMD_DEFAULT;
#endif

}

/********************************************************************
 功    能：	1、销毁与下位机的连接；
 2、释放通信缓冲区。zzzzzzzzzzzzzz
 参数说明：	无
 返 回 值：	无
 说    明：	无
 ********************************************************************/
void CCommWithEcu::Destroy(void)
{
	if (CRunEnvironment::GetDemoMode())
		return;
	m_SendRecv->ClosePort();
#ifdef DATA_DRV_EXT
	if (m_runLog)
	{
		delete m_runLog;
		m_runLog = NULL;
	}
#endif
	delete[] m_SendCommBuf;
	delete[] m_RecieveCommBuf;
	delete m_SendRecv;
}

/********************************************************************
 功    能：	执行此函数后，执行的给下位机的数据会打在同一数据包中，
 直到执行End()才发送为止。
 参数说明：	无
 返 回 值：	无
 说    明：	无
 ********************************************************************/
void CCommWithEcu::Begin(void)
{
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::Begin()";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 清空缓冲区
	memset(m_SendCommBuf, 0, COMMBUFSIZE);
	memset(m_RecieveCommBuf, 0, COMMBUFSIZE);

	// 清空组合发送链
	m_vAssembleChain.clear();

	// 设置组合发送标志
	m_bAssembledFlag = true;

	// 循环标志清0
	m_bRepeating = 0;

	// 组合发送中与ECU通信相关命令
	m_iEcuNum = 0;

	// 设置通信缓冲区指针指向缓冲区首字节
	m_pchBuf = m_SendCommBuf;
}

/********************************************************************
 功    能：	执行此函数后发送从Begin()函数开始打包的命令给下位机，并取得应答
 参数说明：	无
 返 回 值：	-1 --组合发送失败，0-- 组合发送成功
 说    明：	无
 ********************************************************************/
CReceiveFrame CCommWithEcu::End(void)
{

#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::End(void)";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	assert(m_iEcuNum < 2);

	CReceiveFrame recvFrame;

	// 恢复组合发送标志
	m_bAssembledFlag = false;

	// 循环标志清0
	m_bRepeating = 0;

	// 发送数据长度
	int iSendDataLength = m_pchBuf - m_SendCommBuf;

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, COMMBUFSIZE);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	if (0 == recvLength)
	{
		/*throw CEcuCommException(CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
				"  ", "CCommWithEcu::End()");*/
	}
	CSendFrame* pSendFrame;

	unsigned char* pchBuffer = m_RecieveCommBuf + MOVESIZE;

	vector<ASSEMBLECHAIN>::iterator vIter;
	for (vIter = m_vAssembleChain.begin(); vIter != m_vAssembleChain.end();
			vIter++)
	{
		switch (vIter->retType)
		{
		case 1: // short(错误码型)
		{
			if (CRunEnvironment::GetDemoMode())
				break;

			short retval = *pchBuffer;
			if (retval != 0)
			{
				// 返回空的
				return recvFrame;
			}

			pchBuffer++;
			break;
		}

		case 2: // CReceiveFrame(接收类数据型)
		{

			bool bNeedAnalyseInVal = false;

			if (NULL != vIter->inValueAddres)
			{
				bNeedAnalyseInVal = true;
				pSendFrame = (CSendFrame*) vIter->inValueAddres;
			}

			// 接收组数
			short iTeamNum = *pchBuffer;
			pchBuffer++;

			if (bNeedAnalyseInVal)
			{
				// 遍历每个发送命令
				for (int i = 0; i < iTeamNum; i++)
				{
					// 实际每个命令回复的帧数
					short iReceivedFrameNumber = pchBuffer[0];
					pchBuffer++;

					CBinary binData;
					vector<CBinary> binVector;
					for (int j = 0; j < iReceivedFrameNumber; j++)
					{
						// 每帧数据长度
						short iFrameLength = *pchBuffer;
						pchBuffer++;
						if (iFrameLength == 0xFF)
						{
							iFrameLength = (*pchBuffer << 8)
									+ (*(pchBuffer + 1));
							pchBuffer += 2;
						}

						if (iFrameLength > 0)
						{ //2006.6.30 wcx
							binData.WriteBuffer((char*) pchBuffer,
									iFrameLength);
							binVector.push_back(binData);
						}
						pchBuffer += iFrameLength;
					}

					recvFrame += binVector;
				}
			}
			else
			{
				short iRecvFrameNum = *pchBuffer;
				pchBuffer++;

				CBinary binData;
				vector<CBinary> binVector;

				// 便历接收帧数
				for (int iIndex = 0; iIndex < iRecvFrameNum; iIndex++)
				{
					// 接收帧长
					short iLength = *pchBuffer;
					pchBuffer++;
					if ((1 < iRecvFrameNum) || (0 < iLength))
					{
						binData.WriteBuffer((char*) pchBuffer, iLength);
						binVector.push_back(binData);
					}

					/*if((1==iRecvFrameNum) && (0==iLength))
					 ;
					 else {
					 binData.WriteBuffer((char*)pchBuffer, iLength);
					 binVector.push_back(binData);
					 }*/
					pchBuffer += iLength;
				}

				if (binVector.size() > 0)
					recvFrame += binVector;
			}
			break;
		}

		default:
			break;
		} // end switch
	}

	//wcx add 7/3/2006 
	if (1 == recvFrame.size())
	{
		if (0 == recvFrame[0].size())
			recvFrame.clear();
		else if (1 == recvFrame[0].size())
		{
			if (recvFrame[0][0].IsEmpty())
				recvFrame.clear();
		}
	}
	//add end
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "";
			char s[1024] =
			{ 0 };
			const char* data = NULL;
			for (int j = 0; j < recvFrame.size(); j++)
			{
				for (int i = 0; i < recvFrame[j].size(); i++)
				{
					CBinary bin = recvFrame[j][i];
					data = (char*) bin.GetBuffer();
					for (int k = 0; k < bin.GetSize(); k++)
					{
						sprintf(s + 3 * k, "%.2X ", (unsigned char) data[k]);
					}
					temp = "Ans:";
					temp += s;
					m_runLog->WriteContent(temp.c_str());
				}
			}
		}
	}
#endif
	return recvFrame;
}

/********************************************************************
 功    能：	设置等待下位机回复的最大时间，单位毫秒
 参数说明：	unsigned int iMaxTime 等待的时间
 返 回 值：	无
 说    明：	无
 ********************************************************************/
void CCommWithEcu::WaitCommPacketMaxTime(unsigned int iMaxTime)
{
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::WaitCommPacketMaxTime(";
			char t[25];
			//temp += _itoa(iMaxTime,t,10);//hpy modified for ios
			sprintf(t, "%d", iMaxTime);
			temp += t;
			temp += ")";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	m_SendRecv->SetTimeout(iMaxTime);
}

/********************************************************************
 功    能：	设置输入输出端口及通信参数
 参数说明：	unsigned char iIoOutputPort		输出端口
 unsigned char iIoInputPort		输入端口
 bool	bLLineMode
 〈双线CAN〉|〈单线CAN〉，双线CAN为false，单线CAN为true
 char iOutputVoltage		工作电平，缺省为5V
 unsigned char ui8IoSelectItem
 返 回 值：	错误代码
 说    明：	无
 ********************************************************************/
short CCommWithEcu::SetIoPort(unsigned char iIoOutputPort,
		unsigned char iIoInputPort, char iOutputVoltage,
		unsigned short ui16IoSelectItem)
{
	LOG_DEBUG(TAG, "********Begin SetIoPort****************");
	assert(
			(0x00<iIoInputPort && iIoInputPort<0x0D) || iIoInputPort != DB15_CANBUS || iIoInputPort != DB15_CAN_PIN3_11 || iIoInputPort != DB15_VPW || iIoInputPort != DB15_PWM);

	assert(
			(0x00<iIoOutputPort && iIoOutputPort<0x0D) || iIoOutputPort != DB15_CANBUS || iIoOutputPort != DB15_CAN_PIN3_11 || iIoOutputPort != DB15_VPW || iIoOutputPort != DB15_PWM);

#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetIoPort(";
			char t[25];
			/*
			 temp += _itoa(iIoOutputPort,t,10);
			 temp += ",";
			 temp += _itoa(iIoInputPort,t,10);
			 temp += ",";
			 temp += _itoa(iOutputVoltage,t,10);
			 temp += ",";
			 temp += _itoa(ui16IoSelectItem,t,10);
			 */
			//hpy modified for ios
			sprintf(t, "%d,%d,%d,%d", iIoOutputPort, iIoInputPort,
					iOutputVoltage, ui16IoSelectItem);
			temp += t;
			temp += ")";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// CANBUS/PWM/VPW协议的"输入IO口"与"输出IO口"必须相同
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 命令字
	m_pchBuf[0] = 0x01;
	m_pchBuf[1] = 0x01;

	// 输入IO口
	m_pchBuf[2] = iIoInputPort;

	// 输出IO口
	m_pchBuf[3] = iIoOutputPort;

	assert(iOutputVoltage == 5 || iOutputVoltage == 8 || iOutputVoltage == 12);

	m_pchBuf[4] = iOutputVoltage;
	;

	m_pchBuf[5] = ui16IoSelectItem & 0xFF;

	iSendDataLength += 6;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);
		return (short) 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.26
		string strTemp;
		strTemp = "unsigned char iIoOutputPort = ";
		switch (iIoOutputPort)
		{
		case DB15_PIN1:
		case DB15_PIN3:
		case DB15_PIN5:
		case DB15_PIN6:
		case DB15_PIN7:
		case DB15_PIN8:
		case DB15_PIN9:
		case DB15_PIN11:
		case DB15_PIN12:
		case DB15_PIN13:
		case DB15_PIN14:
		{
			char szTemp[30];
			sprintf(szTemp, "DB15_PIN%d", iIoOutputPort);
			strTemp += szTemp;
			break;
		}
		case DB15_PIN10_INPUT:
			strTemp += "DB15_PIN10_INPUT";
			break;
		case DB15_PIN0_INPUT_GROUND:
			strTemp += "DB15_PIN0_INPUT_GROUND";
			break;
		case DB15_PIN2_INPUT:
			strTemp += "DB15_PIN2_INPUT";
			break;
		case DB15_PIN4_OUTPUT_INVALIDATION:
			strTemp += "DB15_PIN4_OUTPUT_INVALIDATION";
			break;
		case DB15_PIN14_153_6kHz:
			strTemp += "DB15_PIN14_153_6kHz";
			break;
		case DB15_PIN1_HIGH_ELECTRICITY:
			strTemp += "DB15_PIN1_HIGH_ELECTRICITY";
			break;
		case DB15_PIN5_HIGH_ELECTRICITY:
			strTemp += "DB15_PIN5_HIGH_ELECTRICITY";
			break;
		case DB15_VPW:
			strTemp += "DB15_VPW";
			break;
		case DB15_PWM:
			strTemp += "DB15_PWM";
			break;
		case DB15_CAN_PIN6_14:
			strTemp += "DB15_CAN_PIN6_14";
			break;
		case DB15_CAN_PIN3_11:
			strTemp += "DB15_CAN_PIN3_11";
			break;
		case DB15_NO_SWITCH_CHANNEL:
			strTemp += "DB15_NO_SWITCH_CHANNEL";
			break;
			/*
			 #define DB15_PIN4_INPUT_UNCONNECT										  0x04
			 #define DB15_PIN15_INPUT_WORK_VOLTAGE									  0x0F
			 */
		}
		strTemp += ", ";

		strTemp = "\n  unsigned char iIoInputPort = ";
		switch (iIoInputPort)
		{
		case DB15_PIN1:
		case DB15_PIN3:
		case DB15_PIN5:
		case DB15_PIN6:
		case DB15_PIN7:
		case DB15_PIN8:
		case DB15_PIN9:
		case DB15_PIN11:
		case DB15_PIN12:
		case DB15_PIN13:
		case DB15_PIN14:
		{
			char szTemp[30];
			sprintf(szTemp, "DB15_PIN%d", iIoOutputPort);
			strTemp += szTemp;
			break;
		}
		case DB15_PIN10_INPUT:
			strTemp += "DB15_PIN10_INPUT";
			break;
		case DB15_PIN0_INPUT_GROUND:
			strTemp += "DB15_PIN0_INPUT_GROUND";
			break;
		case DB15_PIN2_INPUT:
			strTemp += "DB15_PIN2_INPUT";
			break;
//		case DB15_PIN4_OUTPUT_INVALIDATION:
//			strTemp += "DB15_PIN4_OUTPUT_INVALIDATION";
//			break;
		case DB15_PIN14_153_6kHz:
			strTemp += "DB15_PIN14_153_6kHz";
			break;
		case DB15_PIN1_HIGH_ELECTRICITY:
			strTemp += "DB15_PIN1_HIGH_ELECTRICITY";
			break;
		case DB15_PIN5_HIGH_ELECTRICITY:
			strTemp += "DB15_PIN5_HIGH_ELECTRICITY";
			break;
		case DB15_VPW:
			strTemp += "DB15_VPW";
			break;
		case DB15_PWM:
			strTemp += "DB15_PWM";
			break;
		case DB15_CAN_PIN6_14:
			strTemp += "DB15_CAN_PIN6_14";
			break;
		case DB15_CAN_PIN3_11:
			strTemp += "DB15_CAN_PIN3_11";
			break;
		case DB15_NO_SWITCH_CHANNEL:
			strTemp += "DB15_NO_SWITCH_CHANNEL";
			break;
		case DB15_PIN4_INPUT_UNCONNECT:
			strTemp += "DB15_PIN4_INPUT_UNCONNECT";
			break;
		case DB15_PIN15_INPUT_WORK_VOLTAGE:
			strTemp += "DB15_PIN15_INPUT_WORK_VOLTAGE";
			break;
		default:
			strTemp += "DB15_UNDEFINE";
			break;
		}
		strTemp += ", ";

		char szTemp[50];
		sprintf(szTemp, "\n  char iOutputVoltage = %d, ", iOutputVoltage);
		strTemp += szTemp;

		strTemp += "\n  unsigned short ui16IoSelectItem = ";
		if (ui16IoSelectItem & IOS_INPUT_POSITIVE_LOGIC)
			strTemp += "IOS_INPUT_POSITIVE_LOGIC";
		else
			strTemp += "IOS_INPUT_NEGATIVE_LOGIC";

		strTemp += "|";
		if (ui16IoSelectItem & IOS_OUTPUT_POSITIVE_LOGIC)
			strTemp += "IOS_OUTPUT_POSITIVE_LOGIC";
		else
			strTemp += "IOS_OUTPUT_NEGATIVE_LOGIC";

		if (ui16IoSelectItem & IOS_OUTPUT_REVERSE)
			strTemp += "|IOS_OUTPUT_REVERSE";

		if (ui16IoSelectItem & IOS_INPUT_PULL_UP)
			strTemp += "|IOS_INPUT_PULL_UP";

		//将iIoOutputPort ＝ DB15_PIN4_OUTPUT_INVALIDATION的异常忽略。Tiger
		if (DB15_PIN4_OUTPUT_INVALIDATION == iIoOutputPort)
			return (short) 0;
		return 0;
		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION, strTemp,
					"CCommWithEcu::SetIoPort()");
		else
			throw CEcuCommException(ErrCode, strTemp,
					"CCommWithEcu::SetIoPort()");*/
	}

	return ErrCode;
}

/********************************************************************************
 功    能：	设置输出端口选通或断开
 参数说明：	unsigned char iIoOutputPort 输出端口
 bool bIsConnect 选通否
 返 回 值：	错误代码
 说    明：	无
 ********************************************************************************/
short CCommWithEcu::EnableOutputIoLine(unsigned char iIoOutputPort,
		bool bIsConnect)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::EnableOutputIoLine(";
			char t[25];
			//temp += _itoa(iIoOutputPort,t,16);//hpy modified for ios
			sprintf(t, "%d", iIoOutputPort);
			temp += t;
			temp += ",";
			if (bIsConnect)
				temp += "true";
			else
				temp += "false";
			temp += ")";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 选通控制命令字
	m_pchBuf[0] = 0x01;
	m_pchBuf[1] = 0x02;

	m_pchBuf[2] = iIoOutputPort;

	if (bIsConnect)
	{
		m_pchBuf[3] = 0xFF;
	}
	else
	{
		m_pchBuf[3] = 0x00;
	}

	iSendDataLength += 4;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);
	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.26
		string strTemp;
		strTemp = "unsigned char iIoOutputPort = ";
		switch (iIoOutputPort)
		{
		case DB15_PIN1:
		case DB15_PIN3:
		case DB15_PIN5:
		case DB15_PIN6:
		case DB15_PIN7:
		case DB15_PIN8:
		case DB15_PIN9:
		case DB15_PIN11:
		case DB15_PIN12:
		case DB15_PIN13:
		case DB15_PIN14:
		{
			char szTemp[30];
			sprintf(szTemp, "DB15_PIN%d", iIoOutputPort);
			strTemp += szTemp;
			break;
		}
		case DB15_PIN10_INPUT:
			strTemp += "DB15_PIN10_INPUT";
			break;
		case DB15_PIN0_INPUT_GROUND:
			strTemp += "DB15_PIN0_INPUT_GROUND";
			break;
		case DB15_PIN2_INPUT:
			strTemp += "DB15_PIN2_INPUT";
			break;
		case DB15_PIN4_OUTPUT_INVALIDATION:
			strTemp += "DB15_PIN4_OUTPUT_INVALIDATION";
			break;
		case DB15_PIN14_153_6kHz:
			strTemp += "DB15_PIN14_153_6kHz";
			break;
		case DB15_PIN1_HIGH_ELECTRICITY:
			strTemp += "DB15_PIN1_HIGH_ELECTRICITY";
			break;
		case DB15_PIN5_HIGH_ELECTRICITY:
			strTemp += "DB15_PIN5_HIGH_ELECTRICITY";
			break;
		case DB15_VPW:
			strTemp += "DB15_VPW";
			break;
		case DB15_PWM:
			strTemp += "DB15_PWM";
			break;
		case DB15_CAN_PIN6_14:
			strTemp += "DB15_CAN_PIN6_14";
			break;
		case DB15_CAN_PIN3_11:
			strTemp += "DB15_CAN_PIN3_11";
			break;
		case DB15_NO_SWITCH_CHANNEL:
			strTemp += "DB15_NO_SWITCH_CHANNEL";
			break;
			/*
			 #define DB15_PIN4_INPUT_UNCONNECT										  0x04
			 #define DB15_PIN15_INPUT_WORK_VOLTAGE									  0x0F
			 */
		}
		strTemp += ", ";
		strTemp += "\n  bool bIsConnect = ";
		if (bIsConnect)
			strTemp += "true";
		else
			strTemp += "false";

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION, strTemp,
					"CCommWithEcu::EnableOutputIoLine()");
		else
			throw CEcuCommException(ErrCode, strTemp,
					"CCommWithEcu::EnableOutputIoLine()");*/
	}

	return ErrCode;
}

/*************************************************************************************
 功    能：	设置与ECU通信波特率及位格式
 参数说明：	double fBps		 波特率
 char iParityBit    校验格式，缺省无为校验
 char iDataBit      数据位数，缺省为8个数据位
 返 回 值：	错误代码
 说    明：	无
 *************************************************************************************/
short CCommWithEcu::SetBps(double fBps, char iParityBit, char iDataBit)
{
/*
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetBps(";
			char t[25];

			 sprintf(t,"%.2f",fBps);
			 temp += t;
			 temp += ",";
			 temp += _itoa(iParityBit,t,16);
			 temp += ",";
			 temp += _itoa(iDataBit,t,16);
			 temp += ")";

			//hpy modified for ios
			sprintf(t, "%.2f,%d,%d)", fBps, iParityBit, iDataBit);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
*/
	m_bRepeating = 0;
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	m_pchBuf[0] = 0x01;
	m_pchBuf[1] = 0x03;

	// 工作变量
	double fBpsTmp = fBps;

	char expNum = 0;
	while (fBpsTmp > 10.0)
	{
		fBpsTmp = (double) (fBpsTmp / 10.0);
		expNum++;
	}

	while (fBpsTmp < 65535.0)
	{
		fBpsTmp = (double) (fBpsTmp * 10.0);
		expNum--;
	}

	expNum++;

	// 有效数值
	unsigned short bps = (unsigned short) (fBpsTmp / 10.0);

	m_pchBuf[2] = (unsigned char) (bps >> 8);
	m_pchBuf[3] = (unsigned char) (bps & 0xFF);

	// 小数点位置
	m_pchBuf[4] = expNum;

	// 通信位格式    1个字节，低4位表示校验，高4位数据位数；
	m_pchBuf[5] = (unsigned char) ((iDataBit << 4) + (iParityBit & 0x0F));

	iSendDataLength += 6;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);
	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);
	if ((0 == recvLength) || (ErrCode != 0))
	{
		string strTemp;
		char szTemp[50];

		sprintf(szTemp, "double fBps = %f, ", fBps);
		strTemp = szTemp;

		strTemp += "\n  char iParityBit = ";
		switch (iParityBit)
		{
		case PB_SPACE:
			strTemp += "PB_SPACE";
			break;
		case PB_MARK:
			strTemp += "PB_MARK";
			break;
		case PB_EVEN:
			strTemp += "PB_EVEN";
			break;
		case PB_ODD:
			strTemp += "PB_ODD";
			break;
		case PB_NONE:
			strTemp += "PB_NONE";
			break;
		default:
			strTemp += "UNDEFINE";
			break;
		}
		strTemp += ",";

		sprintf(szTemp, "\n  char iDataBit = %d ", iDataBit);
		strTemp += szTemp;
		return 0;
		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION, strTemp,
					"CCommWithEcu::SetBps()");
		else
			throw CEcuCommException(ErrCode, strTemp, "CCommWithEcu::SetBps()");*/
	}
	return ErrCode;
}
short CCommWithEcu::SetBRs(unsigned char BR1, unsigned char BR2,
		char iParityBit, char iDataBit)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetBRs(";
			char t[25];
			/*
			 temp += _itoa(BR1,t,16);
			 temp += ",";
			 temp += _itoa(BR2,t,16);
			 temp += ",";
			 temp += _itoa(iParityBit,t,16);
			 temp += ",";
			 temp += _itoa(iDataBit,t,16);
			 temp += ")";
			 */
			//hpy modified for ios
			sprintf(t, "%d,%d,%d,%d)", BR1, BR2, iParityBit, iDataBit);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x05;

	m_pchBuf[2] = BR1;
	m_pchBuf[3] = BR2;
	m_pchBuf[4] = 0;

	// 通信位格式    1个字节，低4位表示校验，高4位数据位数；
	m_pchBuf[5] = (unsigned char) ((iDataBit << 4) + (iParityBit & 0x0F));

	iSendDataLength += 6;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.27
		string strTemp;
		char szTemp[50];

		sprintf(szTemp, "double BR1=%.2X BR2=%.2X, ", BR1, BR2);
		strTemp = szTemp;

		strTemp += "\n  char iParityBit = ";
		switch (iParityBit)
		{
		case PB_SPACE:
			strTemp += "PB_SPACE";
			break;
		case PB_MARK:
			strTemp += "PB_MARK";
			break;
		case PB_EVEN:
			strTemp += "PB_EVEN";
			break;
		case PB_ODD:
			strTemp += "PB_ODD";
			break;
		case PB_NONE:
			strTemp += "PB_NONE";
			break;
		default:
			strTemp += "UNDEFINE";
			break;
		}
		strTemp += ",";

		sprintf(szTemp, "\n  char iDataBit = %d ", iDataBit);
		strTemp += szTemp;

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION, strTemp,
					"CCommWithEcu::SetBRs()");
		else
			throw CEcuCommException(ErrCode, strTemp, "CCommWithEcu::SetBRs()");*/
	}

	return ErrCode;
}

/*********************************************************************************
 功    能：	设置与ECU通信时间参数
 参数说明：	int iMaxWaitReceivePacket  接收最大等待时间
 int iSendPacketMinWaitTime 发送数据包最小间隔时间，缺省为0
 double fSendByteMinWaitTime   发送数据字节间最小间隔时间，缺省为0
 返 回 值：	错误代码
 说    明：	无
 *********************************************************************************/
short CCommWithEcu::TimeIntervalOver(int iMaxWaitReceivePacket,
		int iSendPacketMinWaitTime, double fSendByteMinWaitTime,
		int iNegativePacketWaitTime)
{
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::TimeIntervalOver(";
			char t[25];
			/*
			 temp += _itoa(iMaxWaitReceivePacket,t,10);
			 temp += ",";
			 temp += _itoa(iSendPacketMinWaitTime,t,10);
			 temp += ",";
			 sprintf(t,"%.2f",fSendByteMinWaitTime);
			 */
			//hpy modified for ios
			sprintf(t, "%d,%d,%.2f", iMaxWaitReceivePacket,
					iSendPacketMinWaitTime, fSendByteMinWaitTime);
			temp += t;
			temp += t;
			temp += ")";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	m_bRepeating = 0;
	int iSendDataLength = 0;

	// 如果不是组合发送
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	m_pchBuf[0] = 0x01;
	m_pchBuf[1] = 0x04;

	unsigned short val;
	val = (unsigned short) iMaxWaitReceivePacket / 10;
	m_pchBuf[2] = (unsigned char) (val >> 8);
	m_pchBuf[3] = (unsigned char) (val & 0xFF);

	val = (unsigned short) iSendPacketMinWaitTime;
	m_pchBuf[4] = (unsigned char) (val >> 8);
	m_pchBuf[5] = (unsigned char) (val & 0xFF);

	val = (unsigned short) (fSendByteMinWaitTime * 10);
	m_pchBuf[6] = (unsigned char) (val >> 8);
	m_pchBuf[7] = (unsigned char) (val & 0xFF);

	val = (unsigned short) (iNegativePacketWaitTime / 10);
	m_pchBuf[8] = (unsigned char) (val >> 8);
	m_pchBuf[9] = (unsigned char) (val & 0xFF);

	iSendDataLength += 10;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.27
		string strTemp;
		char szTemp[50];
		sprintf(szTemp, "int iMaxWaitReceivePacket = %d,",
				iMaxWaitReceivePacket);
		strTemp = szTemp;

		sprintf(szTemp, "\n  int iSendPacketMinWaitTime = %d, ",
				iSendPacketMinWaitTime);
		strTemp += szTemp;

		sprintf(szTemp, "\n  double fSendByteMinWaitTime = %5.1f ",
				fSendByteMinWaitTime);
		strTemp += szTemp;
		return 0;
		/*
		 if(0 == recvLength)
		 throw CEcuCommException(CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
		 strTemp,
		 "CCommWithEcu::TimeIntervalOver()"
		 );
		 else
		 throw CEcuCommException(ErrCode, strTemp, "CCommWithEcu::TimeIntervalOver()");
		 */
	}

	return ErrCode;
}

/*****************************************************************************
 功    能：	设置链路保持
 参数说明：	short iKeepTime        链路保持最小时间间隔，如为0停止链路保持
 CSendFrame *pKeepLinkCmd 链路保持命令，如为NULL停止链路保持
 返 回 值：	错误代码
 说    明：	无
 *****************************************************************************/
short CCommWithEcu::KeepLink(short iKeepTime, CSendFrame *pKeepLinkCommand)
{
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::KeepLink(";
			char t[25];
			//temp += _itoa(iKeepTime,t,10);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d)", iKeepTime);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

#if 1
#define KEEP_LINK_CMD_MAX_LENGTH                50

	if (0 == iKeepTime)
		pKeepLinkCommand = NULL;
	if (NULL == pKeepLinkCommand)
		iKeepTime = 0;

	// 控制命令字
	m_pchBuf[iSendDataLength++] = 0x02;
	m_pchBuf[iSendDataLength++] = 0x01;

	//内容长度
	m_pchBuf[iSendDataLength++] = 0;
	m_pchBuf[iSendDataLength++] = 0;

	//链路保持间隔时间
	m_pchBuf[iSendDataLength++] = (unsigned char) (iKeepTime >> 8);
	m_pchBuf[iSendDataLength++] = (unsigned char) (iKeepTime & 0xFF);

	if (NULL == pKeepLinkCommand)
		m_pchBuf[iSendDataLength++] = 0; // 链路保持命令数量
	else
	{
		m_pchBuf[iSendDataLength++] = pKeepLinkCommand->GetFrameNumber(); // 链路保持命令数量
		iSendDataLength += pKeepLinkCommand->PutCmdDataToBuffer(
				m_pchBuf + iSendDataLength, KEEP_LINK_CMD_MAX_LENGTH);
	}

	//内容长度
	m_pchBuf[2] = (iSendDataLength - 2 - 2) / 256;
	m_pchBuf[3] = (iSendDataLength - 2 - 2) % 256;

#else
	// 控制命令字
	m_pchBuf[0] = 0x02;
	m_pchBuf[1] = 0x01;

	iSendDataLength += 2;

	if (NULL == pKeepLinkCommand)
	{
		// 链路保持命令长度
		m_pchBuf[2] = 0;

		// 链路保持命令内容
		m_pchBuf[3] = 0;

		iSendDataLength += 2;
	}
	else
	{
		char cmdBuf[256]; //命令最长为255个字节
		memset(cmdBuf, 0, 256);

		int dataLength = pKeepLinkCommand->PutCmdDataToBuffer((unsigned char*)cmdBuf, 256);

		// 链路保持命令内容
		memcpy(m_pchBuf+2, cmdBuf, dataLength);

		iSendDataLength += dataLength;
	}

	// 链路保持间隔时间
	m_pchBuf[iSendDataLength] = (unsigned char)(iKeepTime>>8);
	m_pchBuf[iSendDataLength+1] = (unsigned char)(iKeepTime&0xFF);

	iSendDataLength += 2;
#endif

	if (m_bAssembledFlag) // 如果是组合发送，直接返回
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.26
		string strTemp;
		char szTemp[50];
		sprintf(szTemp, "short iKeepTime = %d (ms),", iKeepTime);
		strTemp = szTemp;

		strTemp += "\n  CSendFrame *pKeepLinkCommand = ";
		if (pKeepLinkCommand)
			strTemp += "NOT NULL";
		else
			strTemp += "NULL";
	}

	return ErrCode;
}

/*****************************************************************************
 功    能：设置下位机自运应答ECU数据
 参数说明：CFilterOrLengthRecognise flrFilterCondition 从ECU接收到的需要自动应
 答的数据帧条件；
 CSendFrame sfSendFrame 回复给ECU的发送帧
 返 回 值：错误代码
 说    明：如需要停止自动应答，则发送帧数设置为0即可
 注意：如回复ECU后，如ECU不需要再回答该帧，sfSendFrame中的接收帧数
 应设为0
 *****************************************************************************/
short CCommWithEcu::AutoAnswer(CFilterOrLengthRecognise flrFilterCondition,
		CSendFrame sfSendToEcu)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	m_pchBuf[iSendDataLength++] = 0x02;
	m_pchBuf[iSendDataLength++] = 0x0A;

	if (0 == sfSendToEcu.GetFrameNumber())
	{
		m_pchBuf[iSendDataLength++] = 0; // 过滤条件：无
		m_pchBuf[iSendDataLength++] = 0; // 从ECU接收到的数据帧长度
	}
	else
	{
		m_pchBuf[iSendDataLength++] = flrFilterCondition.GetNum(); // 过滤内容数量
		memcpy(m_pchBuf + iSendDataLength, flrFilterCondition.GetBuffer(),
				flrFilterCondition.GetLenth());
		iSendDataLength += flrFilterCondition.GetLenth();

		// 发送帧
		m_pchBuf[iSendDataLength++] = sfSendToEcu.GetFrameNumber();
		for (int i = 0; i < sfSendToEcu.GetFrameNumber(); i++)
			iSendDataLength += sfSendToEcu.PutCmdDataToBuffer(
					m_pchBuf + iSendDataLength, 100);
	}

	if (m_bAssembledFlag) // 如果是组合发送，直接返回
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.26
		string strTemp = "CFilterOrLengthRecognise flrFilterCondition, CSendFrame sfSendToEcu";

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION, strTemp,
					"CCommWithEcu::AutoAnswer()");
		else
			throw CEcuCommException(ErrCode, strTemp,
					"CCommWithEcu::AutoAnswer()");*/
	}

	return ErrCode;

}

/*****************************************************************************
 功    能：设置下位机自动发送的续发帧
 参数说明：CFilterOrLengthRecognise flrFilterCondition 从ECU接收到的需要自动应
 答的数据帧条件；
 CSendFrame sfSendContinuedFrame 回复给ECU的发送帧
 返 回 值：错误代码
 说    明：如需要停止自动应答，则发送帧数设置为0即可
 注意：如回复ECU后，如ECU不需要再回答该帧，sfSendContinuedFrame中的接收帧数
 应设为0
 *****************************************************************************/
short CCommWithEcu::SetAutoSendContinuedFrame(
		CFilterOrLengthRecognise flrFilterCondition,
		CSendFrame sfSendContinuedFrame)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp =
					"CCommWithEcu::SetAutoSendContinuedFrame(CFilterOrLengthRecognise flrFilterCondition, CSendFrame sfSendContinuedFrame)";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[iSendDataLength++] = 0x02;
	m_pchBuf[iSendDataLength++] = 0x0E;

	if (0 == sfSendContinuedFrame.GetFrameNumber())
	{
		m_pchBuf[iSendDataLength++] = 0; // 过滤条件：无
		m_pchBuf[iSendDataLength++] = 0; // 从ECU接收到的数据帧长度
	}
	else
	{
		m_pchBuf[iSendDataLength++] = flrFilterCondition.GetNum(); // 过滤内容数量
		memcpy(m_pchBuf + iSendDataLength, flrFilterCondition.GetBuffer(),
				flrFilterCondition.GetLenth());
		iSendDataLength += flrFilterCondition.GetLenth();

		// 发送帧
		m_pchBuf[iSendDataLength++] = sfSendContinuedFrame.GetFrameNumber();
		for (int i = 0; i < sfSendContinuedFrame.GetFrameNumber(); i++)
			iSendDataLength += sfSendContinuedFrame.PutCmdDataToBuffer(
					m_pchBuf + iSendDataLength, 100);
	}

	if (m_bAssembledFlag) // 如果是组合发送，直接返回
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.26
		string strTemp =
				"CFilterOrLengthRecognise flrFilterCondition, CSendFrame sfSendContinuedFrame";

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION, strTemp,
					"CCommWithEcu::SetAutoSendContinuedFrame()");
		else
			throw CEcuCommException(ErrCode, strTemp,
					"CCommWithEcu::SetAutoSendContinuedFrame()");*/
	}

	return ErrCode;

}

/********************************************************************
 功    能：	设置BOSCH链路保持
 参数说明：	bool bEnableKeep 进行链路保持否，缺省为false
 返 回 值：	错误代码
 说    明：	无
 ********************************************************************/
short CCommWithEcu::BoschKeepLink(bool bEnableKeep)
{
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::BoschKeepLink(";
			if (bEnableKeep)
				temp += "true)";
			else
				temp += "false)";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif

	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	m_pchBuf[0] = 0x02;
	m_pchBuf[1] = 0x02;

	// 0x00表示终止链路保持，0x01表示开始链路
	if (bEnableKeep)
	{
		m_pchBuf[2] = 0x01;
	}
	else
	{
		m_pchBuf[2] = 0x00;
	}

	iSendDataLength += 3;

	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.26
		string strTemp;
		strTemp = "bool bEnableKeep = ";
		if (bEnableKeep)
			strTemp += "true";
		else
			strTemp += "false";

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION, strTemp,
					"CCommWithEcu::BoschKeepLink ()");
		else
			throw CEcuCommException(ErrCode, strTemp, "BoschKeepLink ()");*/

	}

	return ErrCode;
}
/********************************************************************
 功    能：	线路电平设置命令
 参数说明：	short iHighLowTimes 电平变化次数
 后面参数为依次为高、低电平保持时间：unsigned short iTime, …
 返 回 值：	错误代码
 说    明：	无

 Added by bin for data driven.
 ********************************************************************/
short CCommWithEcu::VoltageHighLowTime(short count, vector<short>& Times)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::VoltageHighLowTime(";
			char t[25];
			//temp += _itoa(count,t,10);
			//temp += ",";
			//hpy modified for ios
			sprintf(t, "%d,", count);
			temp += t;
			for (int i = 0; i < Times.size(); i++)
			{
				//temp += _itoa(Times[i],t,10);
				//temp += ",";
				//hpy modified for ios
				sprintf(t, "%d,", Times[i]);
				temp += t;
			}
			temp += ")";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x02;
	m_pchBuf[1] = 0x03;

	// 数个数 :: = 1个字节，无符号数
	m_pchBuf[2] = count;

	iSendDataLength += 3;

// 	va_list ap;
// 	va_start(ap, iHighLowTimes);

	//2006.6.27
	string strParameter;
	char szParameter[50];
	sprintf(szParameter, "short iHighLowTimes = %d", count);
	strParameter = szParameter;
	//2006.6.27

	bool bVolHigh = true; //电平为高、低交替
	for (int i = 0; i < count; i++)
	{
		if (bVolHigh)
		{
			m_pchBuf[iSendDataLength] = 0xFF;
		}
		else
		{
			m_pchBuf[iSendDataLength] = 0x00;
		}

		short iTimes = Times[i];

		//2006.6.27
		sprintf(szParameter, ",\n  %d", iTimes);
		strParameter += szParameter;
		//2006.6.27

		// 保持时间
		m_pchBuf[iSendDataLength + 1] = (unsigned char) ((iTimes >> 8) & 0xFF);
		m_pchBuf[iSendDataLength + 2] = (unsigned char) (iTimes & 0xFF);

		iSendDataLength += 3;

		bVolHigh = !bVolHigh;
	}

// 	va_end (ap);

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.27

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
					strParameter, "CCommWithEcu::VoltageHighLowTime()");
		else
			throw CEcuCommException(ErrCode, strParameter,
					"CCommWithEcu::VoltageHighLowTime()");*/
	}

	return ErrCode;
}

/********************************************************************
 功    能：	线路电平设置命令
 参数说明：	short iHighLowTimes 电平变化次数
 后面参数为依次为高、低电平保持时间：unsigned short iTime, …
 返 回 值：	错误代码
 说    明：	无
 ********************************************************************/
short CCommWithEcu::VoltageHighLowTime(short iHighLowTimes, ...)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	m_pchBuf[0] = 0x02;
	m_pchBuf[1] = 0x03;

	// 数个数 :: = 1个字节，无符号数
	m_pchBuf[2] = iHighLowTimes;

	iSendDataLength += 3;

	va_list ap;
	va_start(ap, iHighLowTimes);

	//2006.6.27
	string strParameter;
	char szParameter[50];
	sprintf(szParameter, "%d", iHighLowTimes);
	strParameter = szParameter;
	//2006.6.27

	bool bVolHigh = true; //电平为高、低交替
	for (int i = 0; i < iHighLowTimes; i++)
	{
		if (bVolHigh)
		{
			m_pchBuf[iSendDataLength] = 0xFF;
		}
		else
		{
			m_pchBuf[iSendDataLength] = 0x00;
		}

		short iTimes = va_arg (ap, short);

		//2006.6.27
		sprintf(szParameter, ",%d", iTimes);
		strParameter += szParameter;
		//2006.6.27

		// 保持时间
		m_pchBuf[iSendDataLength + 1] = (unsigned char) ((iTimes >> 8) & 0xFF);
		m_pchBuf[iSendDataLength + 2] = (unsigned char) (iTimes & 0xFF);

		iSendDataLength += 3;

		bVolHigh = !bVolHigh;
	}

	va_end(ap);
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::VoltageHighLowTime(";
			temp += strParameter;
			temp += ")";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.27

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
					strParameter, "CCommWithEcu::VoltageHighLowTime()");
		else
			throw CEcuCommException(ErrCode, strParameter,
					"CCommWithEcu::VoltageHighLowTime()");*/
	}

	return ErrCode;
}

/********************************************************************
 功    能：	线路电平设置命令
 参数说明：	short iHighLowTimes 电平变化次数
 后面参数为依次为高、低电平保持时间：unsigned short iTime, …
 返 回 值：	错误代码
 说    明：	无
 ********************************************************************/
short CCommWithEcu::VoltageHighLowTenthTime(short iHighLowTimes, ...)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x0A; //modify by tom sun 20110613

	// 数个数 :: = 1个字节，无符号数
	m_pchBuf[2] = iHighLowTimes;

	iSendDataLength += 3;

	va_list ap;
	va_start(ap, iHighLowTimes);

	//2006.6.27
	string strParameter;
	char szParameter[50];
	sprintf(szParameter, "%d", iHighLowTimes);
	strParameter = szParameter;
	//2006.6.27

	bool bVolHigh = true; //电平为高、低交替
	for (int i = 0; i < iHighLowTimes; i++)
	{
		if (bVolHigh)
		{
			m_pchBuf[iSendDataLength] = 0xFF;
		}
		else
		{
			m_pchBuf[iSendDataLength] = 0x00;
		}

		short iTimes = va_arg (ap, short);

		//2006.6.27
		sprintf(szParameter, ",%d", iTimes);
		strParameter += szParameter;
		//2006.6.27

		// 保持时间
		m_pchBuf[iSendDataLength + 1] = (unsigned char) ((iTimes >> 8) & 0xFF);
		m_pchBuf[iSendDataLength + 2] = (unsigned char) (iTimes & 0xFF);

		iSendDataLength += 3;

		bVolHigh = !bVolHigh;
	}

	va_end(ap);
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::VoltageHighLowTime(";
			temp += strParameter;
			temp += ")";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.27

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
					strParameter, "CCommWithEcu::VoltageHighLowTime()");
		else
			throw CEcuCommException(ErrCode, strParameter,
					"CCommWithEcu::VoltageHighLowTime()");*/
	}

	return ErrCode;
}

/********************************************************************
 功    能：地址码进入系统
 参数说明：unsigned char ucAddressCode  地址码
 unsigned short ui16Parameter  地址码进入选项
 取值范围为以下的组合
 ACE_BPS_AUTO_RECOGNIZE             0x01
 ACE_KW2_REVERSE_TO_ECU             0x02
 ACE_GET_ADDRESS_REVERSE_FROM_ECU   0x04
 ACE_RECEIVE_KW5					 0x08
 ACE_RECEIVE_ONE_FRAME              0x10
 ACE_KW1_REVERSE_TO_ECU			 0x20
 ACE_RECEIVE_N_FRAME				 0x40
 double fBps  发地址码的波特率(bps)
 short i16SendKw2TimeInterval  KW2取反发回的时间间隔(mS)
 返 回 值：ECU  回复帧数据
 说    明：无
 ********************************************************************/
CReceiveFrame CCommWithEcu::AddressCodeEnter(unsigned char ucAddressCode,
		unsigned short ui16Parameter, double fBps,
		short i16ReverseKw2TimeInterval)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;
	double fbps = fBps;

	CReceiveFrame recvFrame; // 返回值

	g_bDemoFlag = 3;
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::AddressCodeEnter(";
			char t[25];
			/*
			 temp += _itoa(ucAddressCode,t,10);
			 temp += ",";
			 temp += _itoa(ui16Parameter,t,10);
			 temp += ",";
			 sprintf(t,"%.2f",fBps);
			 temp += t;
			 temp += ",";
			 temp += _itoa(i16ReverseKw2TimeInterval,t,10);
			 temp += ")";
			 */
			//hpy modified for ios
			sprintf(t, "%d,%d,%.2f,%d)", ucAddressCode, ui16Parameter, fBps,
					i16ReverseKw2TimeInterval);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[iSendDataLength++] = 0x02;
	m_pchBuf[iSendDataLength++] = 0x04;

	// 地址码, 1个字节
	m_pchBuf[iSendDataLength++] = ucAddressCode;

	char expNum = 0;
	while (fBps > 10.0)
	{
		fBps = (double) (fBps / 10.0);
		expNum++;
	}
	while (fBps < 65535.0)
	{
		fBps = (double) (fBps * 10.0);
		expNum--;
	}

	// 最大能表示的有效位数
	unsigned short bps = (unsigned short) (fBps / 10.0);
	expNum++;

	// 波特率，占2个字节
	m_pchBuf[iSendDataLength++] = (unsigned char) (bps >> 8); // 发地址码的波特率
	m_pchBuf[iSendDataLength++] = (unsigned char) (bps & 0xFF);
	// 波特率小数点位置，1个字节
	m_pchBuf[iSendDataLength++] = expNum;

	//地址码选项参数
	m_pchBuf[iSendDataLength++] = ui16Parameter / 256;
	m_pchBuf[iSendDataLength++] = ui16Parameter % 256;

	//回复/KW2时间间隔
	m_pchBuf[iSendDataLength++] = i16ReverseKw2TimeInterval / 256;
	m_pchBuf[iSendDataLength++] = i16ReverseKw2TimeInterval % 256;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		CReceiveFrame recvFrame; // 返回值
		recvFrame.clear();

		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 2;
		m_vAssembleChain.push_back(assChain);

		// 返回空的
		return recvFrame;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, COMMBUFSIZE);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	if (0 == recvLength)
	{ // 2006.6.26
		string strTemp;
		char szTemp[100];

		sprintf(szTemp, "unsigned char ucAddressCode = 0x%02X, ",
				ucAddressCode & 0xFF);
		strTemp += szTemp;

		strTemp += "\n  unsigned short ui16Parameter = ";
		bool bFlag = false;
		if (ui16Parameter & ACE_BPS_AUTO_RECOGNIZE)
		{
			strTemp += "ACE_BPS_AUTO_RECOGNIZE";
			bFlag = true;
		}
		if (ui16Parameter & ACE_KW2_REVERSE_TO_ECU)
		{
			if (bFlag)
				strTemp += "|";
			strTemp += "ACE_KW2_REVERSE_TO_ECU";
			bFlag = true;
		}
		if (ui16Parameter & ACE_GET_ADDRESS_REVERSE_FROM_ECU)
		{
			if (bFlag)
				strTemp += "|";
			strTemp += "ACE_GET_ADDRESS_REVERSE_FROM_ECU";
			bFlag = true;
		}
		if (ui16Parameter & ACE_RECEIVE_KW5)
		{
			if (bFlag)
				strTemp += "|";
			strTemp += "ACE_RECEIVE_KW5";
			bFlag = true;
		}
		if (ui16Parameter & ACE_RECEIVE_ONE_FRAME)
		{
			if (bFlag)
				strTemp += "|";
			strTemp += "ACE_RECEIVE_ONE_FRAME";
			bFlag = true;
		}
		if (ui16Parameter & ACE_CLOSE_TRIGGER_CHANNEL)
		{
			if (bFlag)
				strTemp += "|";
			strTemp += "ACE_CLOSE_TRIGGER_CHANNEL";
			bFlag = true;
		}

		sprintf(szTemp, ", \n  double fBps = %-9.1f bps ", fbps);
		strTemp += szTemp;

		sprintf(szTemp, ", \n  short i16ReverseKw2TimeInterval = %d mS ",
				i16ReverseKw2TimeInterval);
		strTemp += szTemp;

		/*throw CEcuCommException(CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
				strTemp, "CCommWithEcu::AddressCodeEnter()");*/
	}

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;

	// 接收帧数
	short iRecvFrameNum = *pchBuffer;
	pchBuffer++;

	if (0 == iRecvFrameNum)
	{
		return recvFrame;
	}

	vector<CBinary> binVector;
	for (int i = 0; i < iRecvFrameNum; i++)
	{
		short iLength = *pchBuffer; // 接收帧长
		pchBuffer++;

		if (iLength > 0)
		{
			CBinary binData;

			binData.WriteBuffer((char*) pchBuffer, iLength);
			binVector.push_back(binData);

			pchBuffer += iLength;
		}
		else
			break;
	}
	if (binVector.size() > 0)
		recvFrame += binVector;
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "";
			CBinary bin;
			const char* data = NULL;
			char s[1024];
			s[0] = 0;
			memset(s, 0, sizeof(char) * 1024);
			for (int j = 0; j < recvFrame.size(); j++)
			{
				for (int i = 0; i < recvFrame[j].size(); i++)
				{
					bin = recvFrame[j][i];
					data = (char*) bin.GetBuffer();
					for (int k = 0; k < bin.GetSize(); k++)
					{
						sprintf(s + 3 * k, "%.2X ", (unsigned char) data[k]);
					}
					temp = "Ans:";
					temp += s;
					m_runLog->WriteContent(temp.c_str());
				}
			}
		}
	}
	return recvFrame;
}

/********************************************************************
 功    能：BOSCH地址码进入系统
 参数说明：unsigned char ucAddressCode              地址码
 short i16SendKw2TimeInterval	KW2取反发回的时间间隔(mS)
 返 回 值：ECU回复的版本信息
 说    明：无
 ********************************************************************/
CReceiveFrame CCommWithEcu::BoschEnter(unsigned char ucAddressCode,
		short i16ReverseKw2TimeInterval)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	g_bDemoFlag = 3;
	CReceiveFrame recvFrame; // 返回值

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::BoschEnter(";
			char t[25];
			/*
			 temp += _itoa(ucAddressCode,t,16);
			 temp += ",";
			 temp += _itoa(i16ReverseKw2TimeInterval,t,10);
			 temp += ")";
			 */
			//hpy modified for ios
			sprintf(t, "%d,%d)", ucAddressCode, i16ReverseKw2TimeInterval);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[iSendDataLength++] = 0x02;
	m_pchBuf[iSendDataLength++] = 0x09;

	// 地址码
	m_pchBuf[iSendDataLength++] = ucAddressCode;

	//KW2取反发回的时间间隔(mS)
	m_pchBuf[iSendDataLength++] = i16ReverseKw2TimeInterval / 256;
	m_pchBuf[iSendDataLength++] = i16ReverseKw2TimeInterval % 256;

	if (m_bAssembledFlag) // 如果是组合发送，直接返回
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 2;
		m_vAssembleChain.push_back(assChain);

		// 返回空的
		return recvFrame;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, COMMBUFSIZE);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	if (0 == recvLength)
	{ // 2006.6.26
		string strTemp;
		char szTemp[100];

		sprintf(szTemp, "unsigned char ucAddressCode = 0x%02X, ",
				ucAddressCode & 0xFF);
		strTemp += szTemp;

		/*throw CEcuCommException(CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
				strTemp, "CCommWithEcu::BoschEnter()");*/
	}

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;

	short iRecvFrameNum = *pchBuffer;
	pchBuffer++;

	if (0 == iRecvFrameNum)
	{
		return recvFrame;
	}

	CBinary binData;
	vector<CBinary> binVector;

	// 便历接收帧数
	for (int iIndex = 0; iIndex < iRecvFrameNum; iIndex++)
	{
		// 接收帧长
		short iLength = *pchBuffer;

		if ((1 == iRecvFrameNum) && (0 == iLength))
			break;

		pchBuffer++;

		binData.WriteBuffer((char*) pchBuffer, iLength);

		binVector.push_back(binData);
		pchBuffer += iLength;

	}

	if (binVector.size() > 0)
		recvFrame += binVector;

	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "";
			CBinary bin;
			const char* data = NULL;
			char s[1024];
			s[0] = 0;
			memset(s, 0, sizeof(char) * 1024);
			for (int j = 0; j < recvFrame.size(); j++)
			{
				for (int i = 0; i < recvFrame[j].size(); i++)
				{
					bin = recvFrame[j][i];
					data = (char*) bin.GetBuffer();
					for (int k = 0; k < bin.GetSize(); k++)
					{
						sprintf(s + 3 * k, "%.2X ", (unsigned char) data[k]);
					}
					temp = "Ans:";
					temp += s;
					m_runLog->WriteContent(temp.c_str());
				}
			}
		}
	}

	return recvFrame;
}

/********************************************************************
 功    能：	取得下位机状态
 参数说明：	unsigned char ui8StatusWay 状态字，char *pParameter 参数
 返 回 值：	下位机回复的数据
 说    明：	无
 ********************************************************************/
short CCommWithEcu::GetStatus(unsigned char ui8StatusWay,
		unsigned char ucIoInputPort)
{
	assert(m_bAssembledFlag != true);

	m_bRepeating = 0;
	int iSendDataLength = 0;

	CBinary recvBin;

	// 控制命令字
	m_SendCommBuf[0] = 0x02;
	m_SendCommBuf[1] = 0x06;

	// 状态代码
	m_SendCommBuf[2] = ui8StatusWay;

	// 状态参数，占一个字节
	m_SendCommBuf[3] = ucIoInputPort;

	iSendDataLength += 4;

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	if (0 == recvLength)
	{
		string strTemp;

		strTemp = "unsigned char ui8StatusWay = ";
		switch (ui8StatusWay)
		{
		case GS_CHANNEL_SIGNAL:
			strTemp += "GS_CHANNEL_SIGNAL";
			break;
		case GS_CHANNEL_VOLTAGE:
			strTemp += "GS_CHANNEL_VOLTAGE";
			break;
		default:
			strTemp += "UNKNOW";
			break;
		}
		strTemp += ", ";

		strTemp += "\n  unsigned char ucIoInputPort = ";
		switch (ucIoInputPort)
		{
		case DB15_PIN1:
		case DB15_PIN3:
		case DB15_PIN5:
		case DB15_PIN6:
		case DB15_PIN7:
		case DB15_PIN8:
		case DB15_PIN9:
		case DB15_PIN11:
		case DB15_PIN12:
		case DB15_PIN13:
		case DB15_PIN14:
		{
			char szTemp[30];
			sprintf(szTemp, "DB15_PIN%d", ucIoInputPort);
			strTemp += szTemp;
			break;
		}
		case DB15_PIN10_INPUT:
			strTemp += "DB15_PIN10_INPUT";
			break;
		case DB15_PIN0_INPUT_GROUND:
			strTemp += "DB15_PIN0_INPUT_GROUND";
			break;
		case DB15_PIN2_INPUT:
			strTemp += "DB15_PIN2_INPUT";
			break;
			/*
			 case DB15_PIN4_OUTPUT_INVALIDATION:
			 strTemp += "DB15_PIN4_OUTPUT_INVALIDATION";
			 break;
			 */
		case DB15_PIN14_153_6kHz:
			strTemp += "DB15_PIN14_153_6kHz";
			break;
		case DB15_PIN1_HIGH_ELECTRICITY:
			strTemp += "DB15_PIN1_HIGH_ELECTRICITY";
			break;
		case DB15_PIN5_HIGH_ELECTRICITY:
			strTemp += "DB15_PIN5_HIGH_ELECTRICITY";
			break;
		case DB15_VPW:
			strTemp += "DB15_VPW";
			break;
		case DB15_PWM:
			strTemp += "DB15_PWM";
			break;
		case DB15_CAN_PIN6_14:
			strTemp += "DB15_CAN_PIN6_14";
			break;
		case DB15_CAN_PIN3_11:
			strTemp += "DB15_CAN_PIN3_11";
			break;
		case DB15_NO_SWITCH_CHANNEL:
			strTemp += "DB15_NO_SWITCH_CHANNEL";
			break;
		case DB15_PIN4_INPUT_UNCONNECT:
			strTemp += "DB15_PIN4_INPUT_UNCONNECT";
			break;
		case DB15_PIN15_INPUT_WORK_VOLTAGE:
			strTemp += "DB15_PIN15_INPUT_WORK_VOLTAGE";
			break;
		default:
			strTemp += "UNKNOW";
			break;
		}

		/*throw CEcuCommException(CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
				strTemp, "CCommWithEcu::GetStatus()");*/
	}

	unsigned char *chBuffer = m_RecieveCommBuf + MOVESIZE;

	// 状态数据长度
	//short iRecvDataNum = *chBuffer;
	//modified by andy huang 2015.5.18
	unsigned char bLow = *chBuffer;
	chBuffer++;
	unsigned char bHeigh = *chBuffer;
	//short iRetCode = MAKEWORD(bLoW, bHigh);
	short iRetCode = (bHeigh << 8) + bLow;
	return iRetCode;
}

short CCommWithEcu::CFilterOrLengthRecognise::GetLenth()
{
	return m_iLenth;
}

short CCommWithEcu::CFilterOrLengthRecognise::GetNum()
{
	return m_iNum;
}

char *CCommWithEcu::CFilterOrLengthRecognise::GetBuffer()
{
	return m_chBuffer;
}

/********************************************************************
 功    能：读闪光码
 参数说明：W_INT32 iBitMaxInterval 数据位(bit)脉冲间最大间隔时间；
 要求不小于最大实际时间间隔，单位：毫秒
 int iByteMaxInterval 数据值(digit)间最小间隔时间，
 如个位十位之间，十位百位之间的时间间隔；
 要求不小于最大实际时间间隔，单位：毫秒
 int iCodeMaxInterval 闪光码间最大间隔时间
 要求不小于最大实际时间间隔，单位：毫秒
 此参数用于闪光码读取超时
 int iMaxCodeTime 闪烁一个完整的最大的闪光码所占用时间
 要求不小于最大实际使用时间，单位：毫秒
 返 回 值：闪光码内容
 说    明：1、每次调会收到一个闪光码，即诊断程序员第一调本函数时，
 发送参数，之后每次再调本函数时，只是从缓冲区中接收一个
 闪光码包，直至新命令打断。
 2、如收到的闪光码长度为0表示出错/新命令打断退出
 3、上位机接收最大等待时间不应小于如下值：
 CodeMaxInterval+iMaxCodeTime
 ********************************************************************/
CBinary CCommWithEcu::FlashCode(unsigned int uiBitMaxInterval,
		unsigned int uiByteMaxInterval, unsigned int uiCodeMaxInterval,
		unsigned int uiMaxCodeTime, unsigned char uiFlashType)
{
	// 如果读闪光码放到组合发送中，抛出异常
	assert(m_bAssembledFlag != true);

	CBinary recvBin;

	int iSendDataLength = 0;

	g_bDemoFlag = 4;
	if (FLASH_CYCLE != m_bRepeating)
	{
		// 控制命令字
		m_SendCommBuf[iSendDataLength++] = 0x02;
		m_SendCommBuf[iSendDataLength++] = 0x05;

		// 闪光码方式
		m_SendCommBuf[iSendDataLength++] = uiFlashType;

		// BIT间最小间隔时间
		m_SendCommBuf[iSendDataLength++] = (unsigned char) ((uiBitMaxInterval
				>> 8) & 0xFF);
		m_SendCommBuf[iSendDataLength++] = (unsigned char) (uiBitMaxInterval
				& 0xFF);

		// unsigned char间最小间隔时间
		m_SendCommBuf[iSendDataLength++] = (unsigned char) ((uiByteMaxInterval
				>> 8) & 0xFF);
		m_SendCommBuf[iSendDataLength++] = (unsigned char) (uiByteMaxInterval
				& 0xFF);

		// CODE间最小间隔时间
		m_SendCommBuf[iSendDataLength++] = (unsigned char) ((uiCodeMaxInterval
				>> 8) & 0xFF);
		m_SendCommBuf[iSendDataLength++] = (unsigned char) (uiCodeMaxInterval
				& 0xFF);

		// 最长闪光码所用时间
		m_SendCommBuf[iSendDataLength++] = (unsigned char) ((uiMaxCodeTime >> 8)
				& 0xFF);
		m_SendCommBuf[iSendDataLength++] =
				(unsigned char) (uiMaxCodeTime & 0xFF);

		m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

		// 设置闪光码读取循环标志
		m_bRepeating = FLASH_CYCLE;
	}

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);
	if (recvLength <= 1 + 1)
	{
		string strTemp;
		char buffer[50];

		m_bRepeating = N0_CYCLE;

		sprintf(buffer, "uiBitMaxInterval=%d, ", uiBitMaxInterval);
		strTemp += buffer;
		sprintf(buffer, "uiByteMaxInterval=%d, ", uiByteMaxInterval);
		strTemp += buffer;
		sprintf(buffer, "uiCodeMaxInterval=%d, ", uiCodeMaxInterval);
		strTemp += buffer;
		sprintf(buffer, "uiMaxCodeTime=%d, ", uiMaxCodeTime);
		strTemp += buffer;

		/*if (0 == recvLength)
		{
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION, strTemp,
					"CCommWithEcu::FlashCode()");
		}
		else
		{
			short iErrorCode = *(char*) (m_RecieveCommBuf + MOVESIZE);
			throw CEcuCommException(iErrorCode, strTemp,
					"CCommWithEcu::FlashCode()");
		}*/
	}
	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;

	// 闪光码数据长度
	short iFlashLen = *pchBuffer;

	pchBuffer++;

	// 闪光码内容
	recvBin.WriteBuffer((char *) pchBuffer, iFlashLen);
	return recvBin;
}
#ifndef NEW_DRV
// 构造函数
CCommWithEcu::CFilterOrLengthRecognise::CFilterOrLengthRecognise()
{
	Empty();
}

// 类嵌套函数 Empty
void CCommWithEcu::CFilterOrLengthRecognise::Empty()
{
	memset(m_chBuffer, 0, sizeof(m_chBuffer));
	m_iLenth = 0;
	m_iNum = 0;
}

// 类嵌套函数 SetNormalFilterMode
void CCommWithEcu::CFilterOrLengthRecognise::SetNormalFilterMode(
		short iStartPosition, short iFilterSizeLength, char *chFilterContain)
{
	m_iLenth = 0;
	m_iNum = 1;

	m_chBuffer[m_iLenth++] = 0x01;
	m_chBuffer[m_iLenth++] = (char) (unsigned char) (1 + 1 + iFilterSizeLength);
	m_chBuffer[m_iLenth++] = iStartPosition;
	m_chBuffer[m_iLenth++] = iFilterSizeLength;
	memcpy(m_chBuffer + m_iLenth, chFilterContain, iFilterSizeLength);
	m_iLenth += iFilterSizeLength;

}

// 类嵌套函数 AddCanFilterId
void CCommWithEcu::CFilterOrLengthRecognise::AddCanFilterId(
		CBinary binCanFilterId)
{
	switch (binCanFilterId.GetSize())
	{
	case 2:
	case 4:
		if (0x00 == m_chBuffer[0])
		{
			m_chBuffer[0] = 0x02;
			m_chBuffer[1] = 0x00;
			m_iLenth = 2;

			m_iNum = 1;
		}
		if (0x02 == m_chBuffer[0])
		{
			m_chBuffer[m_iLenth++] = binCanFilterId.GetSize();
			binCanFilterId.ReadBuffer(m_chBuffer + m_iLenth);
			m_iLenth += binCanFilterId.GetSize();

			m_chBuffer[1] = m_iLenth - 2;
		}
		break;
	default:
		break;
	}
}

// 类嵌套函数 SetNormalLengthRecogniseMode
void CCommWithEcu::CFilterOrLengthRecognise::SetNormalLengthRecogniseMode(
		short iBytePosition, unsigned char ucMark, short iAddValue)
{
	m_iLenth = 0;
	m_iNum = 1;

	m_chBuffer[m_iLenth++] = (unsigned char) 0x81;
	m_chBuffer[m_iLenth++] = 3;
	m_chBuffer[m_iLenth++] = iBytePosition;
	m_chBuffer[m_iLenth++] = ucMark;
	m_chBuffer[m_iLenth++] = iAddValue;
}

// 类嵌套函数 SetSpecifiedLengthMode
void CCommWithEcu::CFilterOrLengthRecognise::SetSpecifiedLengthMode()
{
	m_iLenth = 0;
	m_iNum = 1;

	m_chBuffer[m_iLenth++] = (unsigned char) 0x82;
	m_chBuffer[m_iLenth++] = 0;
}
#endif
/********************************************************************
 功    能：	设置与ECU通信协议及参数
 参数说明：	short iProtocolWay	   协议标识
 CIdentifyInfo IdentifyInfo
 返 回 值：	错误代码
 说    明：	无
 ********************************************************************/
short CCommWithEcu::ProtocolSet(short iProtocolWay,
		CFilterOrLengthRecognise *pFilterOrLengthRecognise, unsigned char bFlag)
{
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::ProtocolSet(";
			switch (iProtocolWay)
			{
			case PROTOCOL_NORMAL:
				temp += "PROTOCOL_NORMAL";
				break;
			case PROTOCOL_KWP:
				temp += "PROTOCOL_KWP";
				break;
			case PROTOCOL_ISO:
				temp += "PROTOCOL_ISO";
				break;
			case PROTOCOL_VPW:
				temp += "PROTOCOL_VPW";
				break;
			case PROTOCOL_PWM:
				temp += "PROTOCOL_PWM";
				break;
			case PROTOCOL_CANBUS:
				temp += "PROTOCOL_CAN";
				break;
			case PROTOCOL_BOSCH:
				temp += "PROTOCOL_BOSCH";
				break;
			default:
				temp += "UNDEFINE_PROTOCOL";
				break;
			}
			temp += ",";
			if (NULL == pFilterOrLengthRecognise)
				temp += "NULL";
			else
			{
				temp += "NOT NULL";
			}
			temp += ")";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	m_bRepeating = 0;
	int iSendDataLength = 0;
	g_bProtocol = iProtocolWay;
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	//add by scf 2009.04.14 for lsx 
	if (bFlag == 0x00)
	{
		m_pchBuf[0] = 0x02;
		m_pchBuf[1] = 0x07;
	}
	else
	{
		m_pchBuf[0] = 0x02;
		m_pchBuf[1] = 0x20;
	}
	m_pchBuf[2] = (unsigned char) iProtocolWay; // 协议
	m_pchBuf[3] = 0x00; //
	iSendDataLength = 4;

	if (NULL == pFilterOrLengthRecognise)
	{
		switch (iProtocolWay)
		{
		case PROTOCOL_VPW: // VPW协议
		/*
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x01;				// 过滤及识别内容数量
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x01;				// 方式字 NORMAL
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x04;				// CONTAIN LENGTH
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x00;				// 起始位置0
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x02;				// 过滤字节长度
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x48;				// 过滤字节内容
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x6B;
		 */
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x01; // 过滤及识别内容数量
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x01; // 方式字 NORMAL
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x03; // CONTAIN LENGTH
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x00; // 起始位置0
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x01; // 过滤字节长度
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x48; // 过滤字节内容
			break;
		case PROTOCOL_PWM: // PWM协议
		/*
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x01;				// 过滤及识别内容数量
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x01;				// 方式字 NORMAL
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x04;				// CONTAIN LENGTH
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x00;				// 起始位置0
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x02;				// 过滤字节长度
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x41;				// 过滤字节内容
		 m_pchBuf[iSendDataLength++] = (unsigned char)0x6B;
		 */
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x01; // 过滤及识别内容数量
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x01; // 方式字 NORMAL
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x03; // CONTAIN LENGTH
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x00; // 起始位置0
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x01; // 过滤字节长度
			m_pchBuf[iSendDataLength++] = (unsigned char) 0x41; // 过滤字节内容
			break;
		default:
			break;
		}
	}
	else
	{
		m_pchBuf[0x03] = pFilterOrLengthRecognise->GetNum(); // 过滤及识别内容数量

		memcpy(m_pchBuf + iSendDataLength,
				pFilterOrLengthRecognise->GetBuffer(),
				pFilterOrLengthRecognise->GetLenth());

		iSendDataLength += pFilterOrLengthRecognise->GetLenth();
	}

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.27
		string strTemp;
		strTemp = "short iProtocolWay = ";
		switch (iProtocolWay)
		{
		case PROTOCOL_NORMAL:
			strTemp += "PROTOCOL_NORMAL";
			break;
		case PROTOCOL_KWP:
			strTemp += "PROTOCOL_KWP";
			break;
		case PROTOCOL_ISO:
			strTemp += "PROTOCOL_ISO";
			break;
		case PROTOCOL_VPW:
			strTemp += "PROTOCOL_VPW";
			break;
		case PROTOCOL_PWM:
			strTemp += "PROTOCOL_PWM";
			break;
		case PROTOCOL_CANBUS:
			strTemp += "PROTOCOL_PWM";
			break;
		case PROTOCOL_BOSCH:
			strTemp += "PROTOCOL_BOSCH";
			break;
		default:
			strTemp += "UNDEFINE_PROTOCOL";
			break;
		}

		strTemp += ", ";
		strTemp += "\n  CFilterOrLengthRecognise *pFilterOrLengthRecognise = ";
		if (NULL == pFilterOrLengthRecognise)
			strTemp += "NULL";
		else
		{
			strTemp += "NOT NULL";
		}

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION, strTemp,
					"CCommWithEcu::ProtocolSet ()");
		else
			return 0;*/
		//throw CEcuCommException(ErrCode, strTemp, "CCommWithEcu::ProtocolSet ()");
	}

	return ErrCode;
}
void debug(const char *data, unsigned char len)
{
#ifdef _DEBUG
	char s[1024];s[0]=0;
	for(int i=0;i<len;i++)
	sprintf(s+3*i,"%.2X ",(unsigned char)data[i]);
	::OutputDebugString("				");
	::OutputDebugString(s);
	::OutputDebugString("\n");
#endif
}
void debug(CSendFrame& sf, CReceiveFrame& rf)
{
#ifdef _DEBUG
	CBinary bin;

	for(int i=0;i<sf.GetFrameNumber();i++)
	{
		bin=sf[i].m_binSendCmd;
		debug((char*)bin.GetBuffer(),bin.GetSize());
	}
	for(int j=0;j<rf.size();j++)
	{
		for(int i=0;i<rf[j].size();i++)
		{
			bin=rf[j][i];
			debug((char*)bin.GetBuffer(),bin.GetSize());
		}
	}
	char s[1024];s[0]=0;
	sprintf(s,"group=%d",rf.size());
	::OutputDebugString(s);

	if(rf.size())
	{
		::OutputDebugString(" frame=");
		for(j=0;j<rf.size();j++)
		{
			sprintf(s,"%d ",rf[j].size());
			::OutputDebugString(s);
		}
	}
	::OutputDebugString("\n");
#endif
}
void debug(CReceiveFrame& rf)
{
#ifdef _DEBUG

	CBinary bin;
	for(int j=0;j<rf.size();j++)
	{
		for(int i=0;i<rf[j].size();i++)
		{
			bin=rf[j][i];
			debug((char*)bin.GetBuffer(),bin.GetSize());
		}
	}
	char s[1024];s[0]=0;
	sprintf(s,"group=%d",rf.size());
	::OutputDebugString(s);

	if(rf.size())
	{
		::OutputDebugString("frame= \n");
		for(j=0;j<rf.size();j++)
		{
			sprintf(s,"%d ",rf[j].size());
			::OutputDebugString(s);
		}
	}
#endif
}
/********************************************************************
 功    能：	单帧发送数据给ECU接收应答
 参数说明：	CSendFrame SendFrame 发送的帧
 bool bRepeat         是否循环执行
 返 回 值：	接收帧
 说    明：	如接收数据帧校验错则重发发送帧
 ********************************************************************/
CReceiveFrame CCommWithEcu::CumSendReceive(CSendFrame& SendFrame, bool bRepeat)
{
	/////////////////////////////////////////////////////////////////////////
	//和打印互斥
	long int nCount;
#ifdef WIN32
	HANDLE hCanPrint=NULL;
#else
	void * hCanPrint = NULL; //hpy modified for ios
#endif
	unsigned long sendTime = 0;
	unsigned long receTime = 0;
	g_bDemoFlag = 2;
#ifdef WIN32
	if(!m_bCommBlockStart)
	{
		hCanPrint=CreateSemaphore(NULL,1,1,TEXT("CANPRINT"));
		if(hCanPrint!=NULL)
		{
			WaitForSingleObject(hCanPrint,INFINITE);
		}
	}
#endif 
	/////////////////////////////////////////////////////////////////////////
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "";
			char s[1024] =
			{ 0 };
			const char* data = NULL;
			for (int i = 0; i < SendFrame.GetFrameNumber(); i++)
			{
				CBinary bin = SendFrame[i].m_binSendCmd;
				data = (char*) bin.GetBuffer();
				for (int k = 0; k < bin.GetSize(); k++)
				{
					sprintf(s + 3 * k, "%.2x ", (unsigned char) data[k]);
				}
				temp = "Req: ";
				temp += s;
				m_runLog->WriteContent(temp.c_str());
			}
		}
	}
#endif
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	CReceiveFrame recvFrame;

	// 如果非正处于单帧发送循环，组织命令发送, 否则直接接收
	if (SINGLE_CYCLE != m_bRepeating)
	{
		int iSendDataLength = 0; // 发送数据长度

		// 控制命令字
#ifdef NEW_DRV
		m_pchBuf[0] = (m_ctlCmd >> 8)&0x00FF;
		m_pchBuf[1] = m_ctlCmd & 0xFF;
#else
		m_pchBuf[0] = 0x03;
		m_pchBuf[1] = 0x09;
#endif
		if (bRepeat) // 如果是循环发送，则添加循环发送标志
		{
			m_pchBuf[2] = 0xFF; // 重复执行
		}
		else
		{
			m_pchBuf[2] = 0x00; // 不重复执行
		}

		// 单组发送
//		m_pchBuf[3] = 1;
		m_pchBuf[3] = SendFrame.GetFrameNumber(); //2006.10.9

		iSendDataLength += 4;
		m_pchBuf += 4;

		int bufLength = SendFrame.PutCmdDataToBuffer(m_pchBuf, COMMBUFSIZE); //

		iSendDataLength += bufLength;

		// 如果是组合发送，直接返回
		if (m_bAssembledFlag)
		{
			m_pchBuf += bufLength; //iSendDataLength; //modify by yxp

			ASSEMBLECHAIN assChain;
			assChain.inValueAddres = (void*) &SendFrame; // 需要对输入值做分析
			assChain.retType = 2;
			m_vAssembleChain.push_back(assChain);
#ifdef WIN32 
			//关闭互斥
			if(hCanPrint!=NULL)
			{
				ReleaseSemaphore(hCanPrint,1,&nCount);
				CloseHandle(hCanPrint);
			}
#endif 
			return recvFrame;
		}
		if (CRunEnvironment::GetCheckMode())
		{
			//sendTime = GetTickCount();
		}
		m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

		if (bRepeat)
		{
			m_bRepeating = SINGLE_CYCLE;
		}
	}

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);
	if (CRunEnvironment::GetCheckMode())
	{
		//receTime = GetTickCount();
	}

	//2006.6.27 wcx
	if (0 == recvLength)
	{
#ifdef WIN32         
		//关闭互斥
		if(hCanPrint!=NULL)
		{
			ReleaseSemaphore(hCanPrint,1,&nCount);
			CloseHandle(hCanPrint);
		}
#endif 
		return recvFrame;
	}
	//2006.6.27 

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;

	// 接收组数
	short iFrameGroup = *pchBuffer;
	pchBuffer++;

	if (0 == iFrameGroup)
	{
#ifdef WIN32         
		//关闭互斥
		if(hCanPrint!=NULL)
		{
			ReleaseSemaphore(hCanPrint,1,&nCount);
			CloseHandle(hCanPrint);
		}
#endif 
		return recvFrame;
	}

	// 遍历接收组数
	for (int iIndex = 0; iIndex < iFrameGroup; iIndex++)
	{
		// 接收帧数
		short iFrameNum = *pchBuffer;
		pchBuffer++;

		CBinary binData;
		vector<CBinary> binVector;

		// 遍历接收帧数
		for (int iIndex = 0; iIndex < iFrameNum; iIndex++)
		{
			// 接收帧数
			short iFrameLength = *pchBuffer;
			pchBuffer++;

			if (iFrameLength == 0xFF)
			{
				iFrameLength = (*pchBuffer << 8) + (*(pchBuffer + 1));
				pchBuffer += 2;
			}

			//WCX ADD 6/23/2006
			if (0 == iFrameLength)
				break;
			//ADD END

			binData.WriteBuffer((char*) pchBuffer, iFrameLength);

			binVector.push_back(binData);
			pchBuffer += iFrameLength;
		}

		if (binVector.size() > 0) //WCX ADD 6/23/2006
			recvFrame += binVector;
	}
#ifdef WIN32 
	//关闭互斥
	if(hCanPrint!=NULL)
	{
		ReleaseSemaphore(hCanPrint,1,&nCount);
		CloseHandle(hCanPrint);
	}
#endif 
	debug(SendFrame, recvFrame);
	//RunLog
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			// 			string temp = "CCommWithEcu::SendReceive()";
			// 			m_runLog->WriteContent(temp.c_str());
			string temp = "";
			CBinary bin;
			const char* data = NULL;
			char s[1024];
			s[0] = 0;
			char tempStr[50];
			for (int j = 0; j < recvFrame.size(); j++)
			{
				for (int i = 0; i < recvFrame[j].size(); i++)
				{
					bin = recvFrame[j][i];
					data = (char*) bin.GetBuffer();
					for (int k = 0; k < bin.GetSize(); k++)
					{
						sprintf(s + 3 * k, "%.2X ", (unsigned char) data[k]);
					}
					sprintf(tempStr, "%ld Ans:", receTime - sendTime);
					temp += tempStr;
					temp += s;
					m_runLog->WriteContent(temp.c_str());
				}
			}
			memset(s, 0, sizeof(char) * 1024);
			sprintf(s, "group=%d", recvFrame.size());
			temp = s;
			sprintf(s, "  frame=%d", recvFrame.size());
			temp += s;
			m_runLog->WriteContent(temp.c_str());
		}
	}
	return recvFrame;
}
CReceiveFrame CCommWithEcu::CanSendReceive(CSendFrame& SendFrame, bool bRepeat)
{
	/////////////////////////////////////////////////////////////////////////
	//和打印互斥
	long int nCount;
	unsigned long sendTime = 0;
	unsigned long receTime = 0;
#ifdef WIN32
	HANDLE hCanPrint=NULL;
#else
	void * hCanPrint = NULL; //hpy modified for ios
#endif
	g_bDemoFlag = 2;

#ifdef WIN32 
	if(!m_bCommBlockStart)
	{
		hCanPrint=CreateSemaphore(NULL,1,1,TEXT("CANPRINT"));
		if(hCanPrint!=NULL)
		{
			WaitForSingleObject(hCanPrint,INFINITE);
		}
	}
#endif 
	/////////////////////////////////////////////////////////////////////////
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "";
			char s[1024] =
			{ 0 };
			const char* data = NULL;
			for (int i = 0; i < SendFrame.GetFrameNumber(); i++)
			{
				CBinary bin = SendFrame[i].m_binSendCmd;
				data = (char*) bin.GetBuffer();
				for (int k = 0; k < bin.GetSize(); k++)
				{
					sprintf(s + 3 * k, "%.2x ", (unsigned char) data[k]);
				}
				temp = "Req: ";
				temp += s;
				m_runLog->WriteContent(temp.c_str());
			}
		}
	}
#endif
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	CReceiveFrame recvFrame;

	// 如果非正处于单帧发送循环，组织命令发送, 否则直接接收
	if (SINGLE_CYCLE != m_bRepeating)
	{
		int iSendDataLength = 0; // 发送数据长度

		// 控制命令字
#ifdef NEW_DRV
		m_pchBuf[0] = (m_ctlCmd >> 8)&0x00FF;
		m_pchBuf[1] = m_ctlCmd & 0xFF;
#else
		m_pchBuf[0] = 0x03;
		m_pchBuf[1] = 0x08;
#endif
		if (bRepeat) // 如果是循环发送，则添加循环发送标志
		{
			m_pchBuf[2] = 0xFF; // 重复执行
		}
		else
		{
			m_pchBuf[2] = 0x00; // 不重复执行
		}

		// 单组发送
//		m_pchBuf[3] = 1;
		m_pchBuf[3] = SendFrame.GetFrameNumber(); //2006.10.9

		iSendDataLength += 4;
		m_pchBuf += 4;

		int bufLength = SendFrame.PutCmdDataToBuffer(m_pchBuf, COMMBUFSIZE); //

		iSendDataLength += bufLength;

		// 如果是组合发送，直接返回
		if (m_bAssembledFlag)
		{
			m_pchBuf += bufLength; //iSendDataLength; //modify by yxp

			ASSEMBLECHAIN assChain;
			assChain.inValueAddres = (void*) &SendFrame; // 需要对输入值做分析
			assChain.retType = 2;
			m_vAssembleChain.push_back(assChain);
#ifdef WIN32 
			//关闭互斥
			if(hCanPrint!=NULL)
			{
				ReleaseSemaphore(hCanPrint,1,&nCount);
				CloseHandle(hCanPrint);
			}
#endif 
			return recvFrame;
		}
		if (CRunEnvironment::GetCheckMode())
		{
			//sendTime = GetTickCount();
		}
		m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

		if (bRepeat)
		{
			m_bRepeating = SINGLE_CYCLE;
		}
	}

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);
	if (CRunEnvironment::GetCheckMode())
	{
		//receTime = GetTickCount();
	}
	//2006.6.27 wcx
	if (0 == recvLength)
	{
#ifdef WIN32         
		//关闭互斥
		if(hCanPrint!=NULL)
		{
			ReleaseSemaphore(hCanPrint,1,&nCount);
			CloseHandle(hCanPrint);
		}
#endif 
		return recvFrame;
	}
	//2006.6.27 

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;

	// 接收组数
	short iFrameGroup = *pchBuffer;
	pchBuffer++;

	if (0 == iFrameGroup)
	{
#ifdef WIN32         
		//关闭互斥
		if(hCanPrint!=NULL)
		{
			ReleaseSemaphore(hCanPrint,1,&nCount);
			CloseHandle(hCanPrint);
		}
#endif 
		return recvFrame;
	}

	// 遍历接收组数
	for (int iIndex = 0; iIndex < iFrameGroup; iIndex++)
	{
		// 接收帧数
		short iFrameNum = *pchBuffer;
		pchBuffer++;

		CBinary binData;
		vector<CBinary> binVector;

		// 遍历接收帧数
		for (int iIndex = 0; iIndex < iFrameNum; iIndex++)
		{
			// 接收帧数
			short iFrameLength = *pchBuffer;
			pchBuffer++;

			if (iFrameLength == 0xFF)
			{
				iFrameLength = (*pchBuffer << 8) + (*(pchBuffer + 1));
				pchBuffer += 2;
			}

			//WCX ADD 6/23/2006
			if (0 == iFrameLength)
				break;
			//ADD END

			binData.WriteBuffer((char*) pchBuffer, iFrameLength);

			binVector.push_back(binData);
			pchBuffer += iFrameLength;
		}

		if (binVector.size() > 0) //WCX ADD 6/23/2006
			recvFrame += binVector;
	}
#ifdef WIN32 
	//关闭互斥
	if(hCanPrint!=NULL)
	{
		ReleaseSemaphore(hCanPrint,1,&nCount);
		CloseHandle(hCanPrint);
	}
#endif 
	debug(SendFrame, recvFrame);
	//RunLog
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			// 			string temp = "CCommWithEcu::SendReceive()";
			// 			m_runLog->WriteContent(temp.c_str());
			string temp = "";
			CBinary bin;
			const char* data = NULL;
			char s[1024];
			s[0] = 0;
			char tempStr[50];
// 			for(int i=0;i<SendFrame.GetFrameNumber();i++){
// 				bin=SendFrame[i].m_binSendCmd;
// 				data = (char*)bin.GetBuffer();
// 				for(int k=0;k<bin.GetSize();k++){
// 					sprintf(s+3*k,"%.2X ",(unsigned char)data[k]);
// 				}
// 				sprintf(tempStr,"%ld Req:",sendTime);
// 				temp += tempStr;
// 				temp += s;
// 				m_runLog->WriteContent(temp.c_str());
// 			}
			memset(s, 0, sizeof(char) * 1024);
			memset(tempStr, 0, sizeof(char) * 50);
			temp = "";
			for (int j = 0; j < recvFrame.size(); j++)
			{
				for (int i = 0; i < recvFrame[j].size(); i++)
				{
					bin = recvFrame[j][i];
					data = (char*) bin.GetBuffer();
					for (int k = 0; k < bin.GetSize(); k++)
					{
						sprintf(s + 3 * k, "%.2X ", (unsigned char) data[k]);
					}
					sprintf(tempStr, "%ld Ans:", receTime - sendTime);
					temp = tempStr;
					temp += s;
					m_runLog->WriteContent(temp.c_str());
				}
			}
		}
	}
	return recvFrame;
}
CReceiveFrame CCommWithEcu::SendReceive(CSendFrame& SendFrame, bool bRepeat)
{

	/////////////////////////////////////////////////////////////////////////
	//和打印互斥
	long int nCount;
	unsigned long sendTime = 0;
	unsigned long receTime = 0;

	g_bDemoFlag = 2;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	CReceiveFrame recvFrame;

	// 如果非正处于单帧发送循环，组织命令发送, 否则直接接收
	if (SINGLE_CYCLE != m_bRepeating)
	{
		int iSendDataLength = 0; // 发送数据长度

		// 控制命令字
		m_pchBuf[0] = 0x02;
		m_pchBuf[1] = 0x08;
		if (bRepeat) // 如果是循环发送，则添加循环发送标志
		{
			m_pchBuf[2] = 0xFF; // 重复执行
		}
		else
		{
			m_pchBuf[2] = 0x00; // 不重复执行
		}

		// 单组发送
//		m_pchBuf[3] = 1;
		m_pchBuf[3] = SendFrame.GetFrameNumber(); //2006.10.9

		iSendDataLength += 4;
		m_pchBuf += 4;

		int bufLength = SendFrame.PutCmdDataToBuffer(m_pchBuf, COMMBUFSIZE); //

		iSendDataLength += bufLength;

		// 如果是组合发送，直接返回
		if (m_bAssembledFlag)
		{
			m_pchBuf += bufLength; //iSendDataLength;

			ASSEMBLECHAIN assChain;
			assChain.inValueAddres = (void*) &SendFrame; // 需要对输入值做分析
			assChain.retType = 2;
			m_vAssembleChain.push_back(assChain);
			return recvFrame;
		}

		if (CRunEnvironment::GetCheckMode())
		{
			//sendTime = GetTickCount();
		}

		int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

		if (bRepeat)
		{
			m_bRepeating = SINGLE_CYCLE;
		}
	}

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);
	if (CRunEnvironment::GetCheckMode())
	{
		//receTime = GetTickCount();
	}

	if (0 == recvLength)
	{
		return recvFrame;
	}
	//2006.6.27 

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;

	// 接收组数
	short iFrameGroup = *pchBuffer;
	pchBuffer++;

	if (0 == iFrameGroup)
	{
		return recvFrame;
	}

	// 遍历接收组数
	for (int iIndex = 0; iIndex < iFrameGroup; iIndex++)
	{
		// 接收帧数
		short iFrameNum = *pchBuffer;
		pchBuffer++;

		CBinary binData;
		vector<CBinary> binVector;

		// 遍历接收帧数
		for (int iIndex = 0; iIndex < iFrameNum; iIndex++)
		{
			// 接收帧数
			short iFrameLength = *pchBuffer;
			pchBuffer++;

			if (iFrameLength == 0xFF)
			{
				iFrameLength = (*pchBuffer << 8) + (*(pchBuffer + 1));
				pchBuffer += 2;
			}


			if (0 == iFrameLength)
				break;

			binData.WriteBuffer((char*) pchBuffer, iFrameLength);

			binVector.push_back(binData);
			pchBuffer += iFrameLength;
		}

		if (binVector.size() > 0)
			recvFrame += binVector;
	}
	//debug(SendFrame, recvFrame);
	return recvFrame;
}
// bool CCommWithEcu::ContinueReceive(CSendFrame& sendFrm)
// {
// 	CReceiveFrame receFrm =	SendReceive(sendFrm);
// 	return (receFrm.size() > 0 ? true:false);
// }

/********************************************************************
 功    能：	接收ECU应答数据
 参数说明：	CSendFrame SendFrame 发送的帧
 返 回 值：	接收帧
 说    明：
 ********************************************************************/
CReceiveFrame CCommWithEcu::ReceiveOnly(bool bClearBuffer)
{
	CReceiveFrame recvFrame;
#ifdef WIN32 
	/////////////////////////////////////////////////////////////////////////
	//和打印互斥    
	LONG nCount;
	HANDLE hCanPrint=NULL;

	if(!m_bCommBlockStart)
	{
		hCanPrint=CreateSemaphore(NULL,1,1,TEXT("CANPRINT"));
		if(hCanPrint!=NULL)
		{
			WaitForSingleObject(hCanPrint,INFINITE);
		}
	}
	/////////////////////////////////////////////////////////////////////////
#endif 
	g_bDemoFlag = 3;
	memset(m_RecieveCommBuf, 0, 100);

	//2006.12.4
	if (bClearBuffer)
		m_SendRecv->ClearBuffer(); //2006.9.12 解决收多帧时等待时间过长问题]

	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	//2006.6.27 wcx
	if (0 == recvLength)
	{
#ifdef WIN32         
		//关闭互斥
		if(hCanPrint!=NULL)
		{
			ReleaseSemaphore(hCanPrint,1,&nCount);
			CloseHandle(hCanPrint);
		}
#endif 
		return recvFrame;
	}
	//2006.6.27 
	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;

	// 接收帧数
	short iFrameNum = *pchBuffer;
	pchBuffer++;

	CBinary binData;
	vector<CBinary> binVector;

	// 遍历接收帧数
	for (int iIndex = 0; iIndex < iFrameNum; iIndex++)
	{
		short iFrameLength = *pchBuffer;
		pchBuffer++;

		if (iFrameLength == 0xFF)
		{
			iFrameLength = (*pchBuffer << 8) + (*(pchBuffer + 1));
			pchBuffer += 2;
		}

		//2006.6.30 wcx
		if ((1 == iFrameNum) && (0 == iFrameLength))
			break;

		binData.WriteBuffer((char*) pchBuffer, iFrameLength);

		binVector.push_back(binData);
		pchBuffer += iFrameLength;
	}

	if (binVector.size() > 0)
		recvFrame += binVector;
#ifdef WIN320
	//关闭互斥
	if(hCanPrint!=NULL)
	{
		ReleaseSemaphore(hCanPrint,1,&nCount);
		CloseHandle(hCanPrint);
	}
#endif 
	debug(recvFrame);
#ifdef DATA_DRV_EXT

	//RunLog
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			// 			string temp = "CCommWithEcu::SendReceive()";
			// 			m_runLog->WriteContent(temp.c_str());
			string temp = "";
			CBinary bin;
			const char* data = NULL;
			char s[1024];
			s[0] = 0;
			memset(s, 0, sizeof(char) * 1024);
			for (int j = 0; j < recvFrame.size(); j++)
			{
				for (int i = 0; i < recvFrame[j].size(); i++)
				{
					bin = recvFrame[j][i];
					data = (char*) bin.GetBuffer();
					for (int k = 0; k < bin.GetSize(); k++)
					{
						sprintf(s + 3 * k, "%.2X ", (unsigned char) data[k]);
					}
					temp = "Ans:";
					temp += s;
					m_runLog->WriteContent(temp.c_str());
				}
			}
		}
	}
#endif
	return recvFrame;
}

/********************************************************************
 功    能：读快速闪光码
 参数说明：unsigned short ui16Type  快速码类型
 取值范围目前只支持：QF_TOYOTA----丰田闪光码
 bool bRepeat  true--下位机不断读取向上送 false--仅读取一次
 返 回 值：ECU  回复帧数据，
 参数QF_TOYOTA(丰田闪光码)回复格式如下：
 第一个下标下的：
 第一个CBinary内容为ID(正常长度为2个字节，前12bit有效),
 第二个CBinary内容为闪光码内容(正常情况下为12个字节)
 说    明：如果bRepeat为true, 读取闪光码后，要求使用命令打断下位机
 ********************************************************************/
CReceiveFrame CCommWithEcu::QuickFlashCode(unsigned short uiFlashType,
		bool bRepeat)
{
	CReceiveFrame recvFrame; // 返回值
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::QuickFlashCode(";
			char t[25] =
			{ 0 };
			sprintf(t, "%.2x", uiFlashType);
			temp += t;
			temp += ",";
			if (bRepeat)
				temp += "true";
			else
				temp += "false";
			temp += ")";
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	g_bDemoFlag = 3;
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	short iSendDataLength = 0;

	if (QUICK_FLASH_CYCLE != m_bRepeating)
	{
		m_bRepeating = QUICK_FLASH_CYCLE;

		// 控制命令字
		m_pchBuf[iSendDataLength++] = 0x02;
		m_pchBuf[iSendDataLength++] = 0x0B;

		// 地址码, 1个字节
		m_pchBuf[iSendDataLength++] = uiFlashType & 0xFF;
		m_pchBuf[iSendDataLength++] = (bRepeat ? 0x01 : 0x00);

		// 如果是组合发送，直接返回
		if (m_bAssembledFlag)
		{
			CReceiveFrame recvFrame; // 返回值
			m_pchBuf += iSendDataLength;

			ASSEMBLECHAIN assChain;
			assChain.inValueAddres = NULL;
			assChain.retType = 2;
			m_vAssembleChain.push_back(assChain);

			// 返回空的
			return recvFrame;
		}
		m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);
	}

	memset(m_RecieveCommBuf, 0, COMMBUFSIZE);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	if (0 == recvLength)
	{ // 2006.6.26
		string strTemp;

		strTemp = "unsigned short uiFlashType = ";
		if (uiFlashType == QF_TOYOTA)
		{
			strTemp += "QF_TOYOTA";
		}
		else
			strTemp += "unknow";

		if (bRepeat)
			strTemp += ", bRepeat==true";
		else
			strTemp += ", bRepeat==false";

		/*throw CEcuCommException(CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
				strTemp, "CCommWithEcu::QuickFlash()");*/
	}

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;

	// 接收帧数
	short iFrameNum = *pchBuffer;
	pchBuffer++;

	CBinary binData;
	vector<CBinary> binVector;

	// 遍历接收帧数
	for (int iIndex = 0; iIndex < iFrameNum; iIndex++)
	{
		short iFrameLength = *pchBuffer;
		pchBuffer++;

		if (iFrameLength == 0xFF)
		{
			iFrameLength = (*pchBuffer << 8) + (*(pchBuffer + 1));
			pchBuffer += 2;
		}

		//2006.6.30 wcx
		if ((1 == iFrameNum) && (0 == iFrameLength))
			break;

		binData.WriteBuffer((char*) pchBuffer, iFrameLength);

		binVector.push_back(binData);
		pchBuffer += iFrameLength;
	}

	if (binVector.size() > 0)
		recvFrame += binVector;
	else
	{
		// 如果调用一次本函数，返回接收帧数是0时，诊断程序员未发送
		//任何下位机通信命令时，再调用本函数会抛出异常（因为此时下
		//位机未执行任何通信动作）。
		//正确方法：诊断程序员应先发送任意下位机通信命令
		m_bRepeating = N0_CYCLE;
	}
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "";
			CBinary bin;
			const char* data = NULL;
			char s[1024];
			s[0] = 0;
			memset(s, 0, sizeof(char) * 1024);
			for (int j = 0; j < recvFrame.size(); j++)
			{
				for (int i = 0; i < recvFrame[j].size(); i++)
				{
					bin = recvFrame[j][i];
					data = (char*) bin.GetBuffer();
					for (int k = 0; k < bin.GetSize(); k++)
					{
						sprintf(s + 3 * k, "%.2X ", (unsigned char) data[k]);
					}
					temp = "Ans:";
					temp += s;
					m_runLog->WriteContent(temp.c_str());
				}
			}
		}
	}
	return recvFrame;
}

/********************************************************************
 功    能：等待，直到接收电平不变化的时间达到某一给定数据后退出
 参数说明：W_UINT32 uiVoltageKeepTime 电平无变化保持的时间
 unsigned int uiOverTime 等待最大超时时间
 short VoltageLevelStatus 要求的电平,
 高电平：WVT_HIGH_VOLTAGE
 低电平：WVT_LOW_VOLTAGE
 缺省为：WVT_HIGH_VOLTAGE|WVT_LOW_VOLTAGE，即没有电平要求
 返 回 值：错误代码
 说    明：上位机最大等待时间一定要大于最大超时时间－－uiOverTime
 成功否需判断返回值，如下位机无应答则抛出异常
 ********************************************************************/
short CCommWithEcu::WaitVoltageHighLowTime(unsigned int uiVoltageKeepTime,
		unsigned int uiOverTime, unsigned short VoltageLevelStatus)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	m_pchBuf[iSendDataLength++] = 0x02;
	m_pchBuf[iSendDataLength++] = 0x0C;

	m_pchBuf[iSendDataLength++] = uiVoltageKeepTime / 256;
	m_pchBuf[iSendDataLength++] = uiVoltageKeepTime % 256;

	m_pchBuf[iSendDataLength++] = uiOverTime / 256;
	m_pchBuf[iSendDataLength++] = uiOverTime % 256;

	m_pchBuf[iSendDataLength++] = VoltageLevelStatus & 0xFF;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if (0 == recvLength)
	{
		string strParameter;
		char buffer[50];

		sprintf(buffer, "uiVoltageKeepTime=%d", uiVoltageKeepTime);
		strParameter += buffer;
		sprintf(buffer, ", \n uiOverTime=%d", uiOverTime);
		strParameter += buffer;
		if (VoltageLevelStatus == WVT_HIGH_VOLTAGE)
			strParameter += ", \n VoltageLevelStatus==WVT_HIGH_VOLTAGE ";
		else if (VoltageLevelStatus == WVT_LOW_VOLTAGE)
			strParameter += ", \n VoltageLevelStatus==WVT_LOW_VOLTAGE ";
		else
			strParameter +=
					", \n VoltageLevelStatus==WVT_HIGH_VOLTAGE|WVT_LOW_VOLTAGE";

		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
					strParameter, "CCommWithEcu::VoltageHighLowTime()");
		else
			throw CEcuCommException(ErrCode, strParameter,
					"CCommWithEcu::VoltageHighLowTime()");*/
	}

	return ErrCode;

}

//////////////////////////////////////////////////////////////////////////////
//凌义川添加
//////////////////////////////////////////////////////////////////////////////
void CCommWithEcu::CommBlockStart()
{
#if 0
	if(m_hBlockHandle==NULL)
	{
		m_hBlockHandle=(int)CreateSemaphore(NULL,1,1,TEXT("CANPRINT"));
		if(m_hBlockHandle!=NULL)
		{
			WaitForSingleObject((HANDLE)m_hBlockHandle,INFINITE);
		}
	}
	m_bCommBlockStart=1;
#endif 

}

void CCommWithEcu::CommBlockEnd()
{
#if 0
	LONG nCount;

	if(m_hBlockHandle!=NULL)
	{
		ReleaseSemaphore((HANDLE)m_hBlockHandle,1,&nCount);
		CloseHandle((HANDLE)m_hBlockHandle);

		//added by johnnyling 2006-11-29
		m_hBlockHandle=NULL;
	}

	m_bCommBlockStart=0;
#endif 
}

/*************************************************************************************
 功    能：设置高速通信方式
 参数说明：unsigned char pParameter 通信方式
 通信方式 ：：=1个字节,0 低速  1 高速
 返 回 值：错误代码
 说    明：执行成功
 *************************************************************************************/
short CCommWithEcu::SetHighSpeed(unsigned char pParameter)
{
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetHighSpeed(";
			char t[25];
			//temp += _itoa(pParameter,t,16);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d", pParameter);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	m_pchBuf[0] = 0x02;
	m_pchBuf[1] = 0x0D;

	m_pchBuf[2] = pParameter;

	iSendDataLength += 3;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	return ErrCode;
}

/*************************************************************************************
 功    能：设置BMW特殊方式(发命令之前先高低电平触发)通信方式命令字
 参数说明：unsigned char pParameter 通信方式
 通信方式 ：：=1个字节,0 无效  1 有效
 返 回 值：错误代码
 说    明：执行成功
 *************************************************************************************/
short CCommWithEcu::SetBMWSpecial(unsigned char pParamerer)
{
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetBMWSpecial(";
			char t[25];
			//temp += _itoa(pParamerer,t,10);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d", pParamerer);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x02;
	m_pchBuf[1] = 0x0F;

	m_pchBuf[2] = pParamerer;

	iSendDataLength += 3;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	return ErrCode;
}

//CCommWithEcu::SetCanHardMaskFilter(bool bflag)
/*************************************************************************************
 功    能：设置Can 硬件过滤模式
 参数说明：unsigned char pParameter 通信方式
 通信方式 ：：=1个字节,0 无效  1 有效
 返 回 值：错误代码
 说    明：执行成功
 *************************************************************************************/
short CCommWithEcu::SetCanHardMaskFilter(unsigned char pParamerer)
{
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetCanHardMaskFilter(";
			char t[25];
			//temp += _itoa(pParamerer,t,10);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d", pParamerer);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x0F;

	m_pchBuf[2] = pParamerer;

	iSendDataLength += 3;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	return ErrCode;
}

short CCommWithEcu::SetVWCanSpecial(short iEnable)
{
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetVWCanSpecial(";
			char t[25];
			//temp += _itoa(iEnable,t,10);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d", iEnable);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x04;

	m_pchBuf[2] = iEnable;

	iSendDataLength += 3;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	return ErrCode;
}

//add by scf for 24V 2009.12.01

short CCommWithEcu::Set24VSpecial(short iEnable)
{
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::Set24VSpecial(";
			char t[25];
			//temp += _itoa(iEnable,t,10);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d", iEnable);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x0B;

	m_pchBuf[2] = iEnable;

	iSendDataLength += 3;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	return ErrCode;
}

void CCommWithEcu::CFilterOrLengthRecognise::SetCanFilterMode(
		short iFilterPosition, short iFilterMask, short iLengthPosition)
{
	m_iLenth = 0;
	m_iNum = 1;

	m_chBuffer[m_iLenth++] = 0x03;
	m_chBuffer[m_iLenth++] = (char) (unsigned char) (1 + 1 + 2);
	m_chBuffer[m_iLenth++] = iFilterPosition;
	m_chBuffer[m_iLenth++] = 2;
	m_chBuffer[m_iLenth++] = iFilterMask;
	m_chBuffer[m_iLenth++] = iLengthPosition;
}
short CCommWithEcu::SetProtocol(short iProtocolWay)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if ((iProtocolWay < PROTOCOL_NORMAL) || (iProtocolWay > PROTOCOL_BOSCH))
		return EC_PARAMETER;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetProtocol(";
			char t[25];
			//temp += _itoa(iProtocolWay,t,10);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d", iProtocolWay);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x02;
	m_pchBuf[2] = (unsigned char) iProtocolWay; // 协议
	iSendDataLength = 3;
	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.27
		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
					"SetProtocol", "CCommWithEcu::SetProtocol ()");
		else
			return 0;*/
		//throw CEcuCommException(ErrCode, strTemp, "CCommWithEcu::ProtocolSet ()");
	}

	return ErrCode;
}
short CCommWithEcu::ControlKLine(short iEnable)
{
	m_bRepeating = 0;
	int iSendDataLength = 0;

	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::ControlKLine(";
			char t[25];
			//temp += _itoa(iEnable,t,10);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d", iEnable);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x03;
	m_pchBuf[2] = (unsigned char) iEnable; // 协议
	iSendDataLength = 3;
	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;

		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	if ((0 == recvLength) || (ErrCode != 0))
	{ // 2006.6.27
		/*if (0 == recvLength)
			throw CEcuCommException(
					CEcuCommException::ECX_DOWN_BOARD_NO_RESPONSION,
					"ControlKLine", "CCommWithEcu::ControlKLine ()");
		else
			return 0;*/
		//throw CEcuCommException(ErrCode, strTemp, "CCommWithEcu::ProtocolSet ()");
	}

	return ErrCode;
}
short CCommWithEcu::SetBenzSpecial(short iEnable)
{
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetBenzSpecial(";
			char t[25];
			//temp += _itoa(iEnable,t,10);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d)", iEnable);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x06;

	m_pchBuf[2] = iEnable;

	iSendDataLength += 3;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	return ErrCode;
}
short CCommWithEcu::SetHoldonSpecial(short iFilterByte)
{
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode() && (m_runLog != NULL))
	{
		if (m_runLog->GetLogPermit())
		{
			string temp = "CCommWithEcu::SetHoldonSpecial(";
			char t[25];
			//temp += _itoa(iFilterByte,t,10);
			//temp += ")";
			//hpy modified for ios
			sprintf(t, "%d)", iFilterByte);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x07;

	m_pchBuf[2] = iFilterByte;

	iSendDataLength += 3;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	return ErrCode;
}

//CCommWithEcu::SetNegativeHandleMethod
/*************************************************************************************
 功    能：设置消极应答的处理模式
 参数说明：	unsigned char iPosition1    消极应答的字节位置 　　如：3
 unsigned char iNegByte      消极应答的字节	　	　如: 0x7F
 unsigned char iPosition2    消极应答参数位置，　　如:5
 unsigned char iNegParameter 消极应答参数，　　　　如:0x78
 unsigned char iHandleType　　消极应答处理方法
 返 回 值：错误代码
 说    明：执行成功
 *************************************************************************************/
//short CCommWithEcu::SetNegativeHandleMethod(unsigned char iPosition1, unsigned char iNegByte, unsigned char iPosition2, unsigned char iNegParameter, unsigned char iHandleType)//hpy modified 20131010
short CCommWithEcu::SetNegativeHandleMethod(unsigned char iPosition1,
		unsigned char iNegByte, unsigned char iPosition2,
		unsigned char iNegParameter, unsigned char iHandleType,
		unsigned short iMaxCnt)
{
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}
#ifdef DATA_DRV_EXT
	if (CRunEnvironment::GetCheckMode())
	{
		if (m_runLog)
		{
			string temp = "CCommWithEcu::SetNegativeHandleMethod(";
			char t[100];
			/*
			 temp += _itoa(iPosition1,t,10);
			 temp += _itoa(iNegByte,t,10);
			 temp += _itoa(iPosition2,t,10);
			 temp += _itoa(iNegParameter,t,10);
			 temp += _itoa(iHandleType,t,10);
			 temp += ")";
			 */
			//hpy modified for ios
			sprintf(t, "%d,%d,%d,%d,%d)", iPosition1, iNegByte, iPosition2,
					iNegParameter, iHandleType);
			temp += t;
			m_runLog->WriteContent(temp.c_str());
		}
	}
#endif
	// 控制命令字
	m_pchBuf[0] = 0x03;
	m_pchBuf[1] = 0x0C;

	m_pchBuf[2] = iPosition1;
	m_pchBuf[3] = iNegByte;
	m_pchBuf[4] = iPosition2;
	m_pchBuf[5] = iNegParameter;
	m_pchBuf[6] = iHandleType;

	m_pchBuf[7] = (unsigned char) (iMaxCnt >> 8); //hpy added 20131010
	m_pchBuf[8] = (unsigned char) iMaxCnt; //hpy added 20131010

	iSendDataLength += 9; //iSendDataLength += 7;//hpy modified 20131010

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	return ErrCode;
}

short CCommWithEcu::InitAndResetCOMM()
{
	int iSendDataLength = 0;

	// 如果不是组合发送，偏移指针指向头部
	if (!m_bAssembledFlag)
	{
		memset(m_SendCommBuf, 0, 100);
		m_pchBuf = m_SendCommBuf;
	}

	// 控制命令字
	m_pchBuf[0] = 0x01;
	m_pchBuf[1] = 0x05;

	iSendDataLength += 2;

	// 如果是组合发送，直接返回
	if (m_bAssembledFlag)
	{
		m_pchBuf += iSendDataLength;
		ASSEMBLECHAIN assChain;
		assChain.inValueAddres = NULL;
		assChain.retType = 1;
		m_vAssembleChain.push_back(assChain);

		return 0;
	}

	int iRev = m_SendRecv->SendData(m_SendCommBuf, iSendDataLength);

	memset(m_RecieveCommBuf, 0, 100);
	int recvLength = m_SendRecv->ReceiveData(m_RecieveCommBuf, COMMBUFSIZE);

	unsigned char *pchBuffer = m_RecieveCommBuf + MOVESIZE;
	short ErrCode = *((char*) pchBuffer);

	return ErrCode;
}
