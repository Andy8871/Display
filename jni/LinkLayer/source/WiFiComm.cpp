// WiFiComm.cpp: implementation of the CWiFiComm class.
//
//////////////////////////////////////////////////////////////////////
#include "WiFiComm.h"
#include <sys/socket.h>
#include <errno.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#ifndef _DEBUG
#define PRINTBUF
#else
#define PRINTBUF(pbuf, iLen, status) 
#endif // _DEBUG
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

string CWiFiComm::m_szServerIP = "10.10.100.254:8899";
//int CWiFiComm::m_nCount = 0;
const char* TAG = "CWiFiComm";

/**
 * connection socket 
 */
//SOCKET m_sock = INVALID_SOCKET;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void WriteLog(const char* pBuff)
{
	__android_log_write(ANDROID_LOG_DEBUG, TAG, pBuff);
}

CWiFiComm::CWiFiComm()
{
	m_socketfd = 0;

}

CWiFiComm::~CWiFiComm()
{
}

bool CWiFiComm::Init(int iOpenMode)
{
	WriteLog("CWiFiComm::Init");
	if (ConnectVDM())
	{
		return true;
	}

	return false;
}

int CWiFiComm::Send(unsigned char* pBuffer, int iLength)
{
	int iLenS = 0;

	if (m_socketfd != 0)
	{
		/*unsigned char pData[] =
		{ 0xaa, 0x55, 0x00, 0x02, 0xff, 0xfd, 0x02, 0x00, 0x01, 0x00 };
		memset(pBuffer, 0, iLength);
		memcpy(pBuffer, pData, 10);
		iLength = 10;*/
		iLenS = send(m_socketfd, (const char *) pBuffer, iLength, 0);
		if (iLength != iLenS)
		{
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "Send data failed");
		}
		else
		{
			string strCmd;
			for (int i = 0; i < iLength; ++i)
			{
				char pBuff[8];
				memset(pBuff, 0, 8);
				sprintf(pBuff, "0x%02x ", pBuffer[i]);
				strCmd += pBuff;
			}
			//char* pBuff = new char[iSendLength + 8];

			__android_log_print(ANDROID_LOG_DEBUG, TAG, "Send:%s",
					strCmd.c_str());
		}

	}

	if (m_socketfd != 0)
	{
		return iLenS;
	}
	return errno;
}

int CWiFiComm::Receive(unsigned char *pBuffer, int iLength)
{
	int iLenR = 0;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;
	/*setsockopt(m_socketfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
			sizeof(struct timeval));*/
	iLenR = recv(m_socketfd, (char *) pBuffer, iLength, 0);
	if (iLenR < 0)
	{
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "Receive data failed %s", strerror(errno));
		//return iLenR;
	}
	else
	{
		string strCmd;
		for (int i = 0; i < iLength; ++i)
		{
			char pBuff[8];
			memset(pBuff, 0, 8);
			sprintf(pBuff, "0x%02x ", pBuffer[i]);
			strCmd += pBuff;
		}
		//char* pBuff = new char[iSendLength + 8];

		__android_log_print(ANDROID_LOG_DEBUG, TAG, "Receive:%s",
				strCmd.c_str());
	}
	// check if there are data to recv
	/*fd_set readfds;
	 timeval timeout;
	 timeout.tv_sec = 0;
	 timeout.tv_usec = 1000;
	 FD_ZERO(&readfds);
	 FD_SET(m_socketfd, &readfds);
	 int ret = select(m_socketfd + 1, &readfds, 0, 0, &timeout);
	 if (ret < 0)
	 {
	 __android_log_print(ANDROID_LOG_DEBUG, TAG, "select return < 0 %s", strerror(errno));
	 }
	 else if (ret == 0)
	 {
	 __android_log_print(ANDROID_LOG_DEBUG, TAG, "select return == 0 %s", strerror(errno));
	 }
	 else if (ret > 0)
	 {
	 __android_log_print(ANDROID_LOG_DEBUG, TAG, "select return > 0 %s", strerror(errno));
	 if (FD_ISSET(m_socketfd, &readfds))
	 {
	 iLenR = recv(m_socketfd, (char *) pBuffer, iLength, 0);
	 if (iLenR < 0)
	 {
	 __android_log_print(ANDROID_LOG_DEBUG, TAG, "Receive data failed");
	 return iLenR;
	 }
	 else
	 {
	 string strCmd;
	 for (int i = 0; i < iLength; ++i)
	 {
	 char pBuff[8];
	 memset(pBuff, 0, 8);
	 sprintf(pBuff, "0x%02x ", pBuffer[i]);
	 strCmd += pBuff;
	 }
	 //char* pBuff = new char[iSendLength + 8];

	 __android_log_print(ANDROID_LOG_DEBUG, TAG, "Receive:%s", strCmd.c_str());
	 }

	 }
	 __android_log_print(ANDROID_LOG_DEBUG, TAG, "FD_ISSET failed");
	 }*/

	if (m_socketfd != 0)
	{
		return iLenR;
	}
	return errno;
}

void CWiFiComm::Close(void)
{
	DisconnectVDM();
}

int CWiFiComm::ClearBuffer()
{
	return false;
}

//测试VDM通信
bool CWiFiComm::TestLink(char * pCmdData, int iLength)
{
	return false;
}

//modified by andy huang 2015.5.22
//刷新VDM固件
bool CWiFiComm::ResetVDM(char * pData, int uiSize)
{
	int iRecvLen = 0;
	unsigned char SendPack[MAX_COMM_PACK_LENGTH] =
	{ 0 };
	unsigned char RecvPack[MAX_COMM_PACK_LENGTH] =
	{ 0 };

	WriteLog("CSerialComm::ResetVDM:Begin");

	if (!Refresh())
	{
		WriteLog("CSerialComm::ResetVDM:OpenRefresh failed");
		return false;
	}

	//Each packet is either accepted (ACK answer) or discarded (NACK answer):
	// ACK = 0x79
	// NACK = 0x1F
	SendPack[0] = 0x7F; //handshake with CPU

	//if ( Send(SendPack,1) == 1 )
	//{
	//	iRecvLen = Receive(RecvPack,1);
	//	if ( 0x79 != RecvPack[0] )
	//	{
	//		WriteLog("First time handshake failed!");
	//		return false;
	//	}
	//}
	//hpy modified 20140829
	for (int i = 0; i < 5; i++)
	{
		if (Send(SendPack, 1) == 1)
		{
			sleep(50);
			iRecvLen = Receive(RecvPack, 1);
			if (0x79 != RecvPack[0])
			{
				WriteLog("First time handshake failed!");
				sleep(300);
				continue;;
			}
			else
			{
				break;
			}
		}
	}

	//Readout Unprotect command
	SendPack[0] = 0x92;
	SendPack[1] = 0x6D;
	if (Send(SendPack, 2) == 2)
	{
		sleep(50);
		iRecvLen = Receive(RecvPack, 2);
		if ((0x79 == RecvPack[0]) && (0x79 == RecvPack[1]))
		{
			sleep(300);
		}
		else
		{
			WriteLog("Readout unprotect command failed!");
		}
	}

	//Each packet is either accepted (ACK answer) or discarded (NACK answer):
	// ACK = 0x79
	// NACK = 0x1F
	SendPack[0] = 0x7F; //handshake with CPU
	if (Send(SendPack, 1) == 1)
	{
		iRecvLen = Receive(RecvPack, 1);
		if (0x79 != RecvPack[0])
		{
			WriteLog("Second time handshake failed!");
			return false;
		}
	}

	//Erase all flash memory pages first
	SendPack[0] = 0x43; //erase cmd
	SendPack[1] = 0xBC;
	if (Send(SendPack, 2) == 2)
	{
		iRecvLen = Receive(RecvPack, 1);
		if (0x79 != RecvPack[0])
		{
			WriteLog("Erase memory cmd failed!");
			return false;
		}
	}

	SendPack[0] = 0xFF; //all pages cmd
	SendPack[1] = 0x00;
	if (Send(SendPack, 2) == 2)
	{
		iRecvLen = Receive(RecvPack, MAX_COMM_PACK_LENGTH);
		if (0x79 != RecvPack[0])
		{
			WriteLog("Erase memory size failed!");
			return false;
		}
	}

	int iTotalBlocks = 0;
	if (uiSize % DATA_BLOCK_SIZE == 0)
	{
		iTotalBlocks = uiSize / DATA_BLOCK_SIZE;
	}
	else
	{
		iTotalBlocks = uiSize / DATA_BLOCK_SIZE + 1;
	}

	//Write Memory
	for (int i = 0; i < iTotalBlocks; i++)
	{
		SendPack[0] = 0x31; //write cmd
		SendPack[1] = 0xCE;
		if (Send(SendPack, 2) == 2)
		{
			iRecvLen = Receive(RecvPack, 1);
			if (0x79 != RecvPack[0])
			{
				WriteLog("Write memory cmd failed!");
				return false;
			}
		}

		SendPack[0] = 0x08;
		SendPack[1] = 0x00 + (char) (i >> 8);
		SendPack[2] = 0x00 + (char) i;
		SendPack[3] = 0x00;
		SendPack[4] = XorCheckSum(SendPack, 4);
		if (Send(SendPack, 5) == 5)
		{
			iRecvLen = Receive(RecvPack, 1);
			if (0x79 != RecvPack[0])
			{
				WriteLog("Write memory address failed!");
				return false;
			}
		}

		if (i == (iTotalBlocks - 1))
		{
			char ucLastBlockSize = uiSize - i * DATA_BLOCK_SIZE;
			SendPack[0] = ucLastBlockSize - 1;

			memcpy(SendPack + 1, pData + i * DATA_BLOCK_SIZE, ucLastBlockSize);

			SendPack[ucLastBlockSize + 1] = XorCheckSum(SendPack,
					ucLastBlockSize + 1);

			if ((Send(SendPack, ucLastBlockSize + 2)) == (ucLastBlockSize + 2))
			{
				iRecvLen = Receive(RecvPack, MAX_COMM_PACK_LENGTH);
				if (0x79 != RecvPack[0])
				{
					WriteLog("Error6!");
					return false;
				}
			}
		}
		else
		{
			SendPack[0] = 0xFF;

			memcpy(SendPack + 1, pData + i * DATA_BLOCK_SIZE, DATA_BLOCK_SIZE);

			SendPack[DATA_BLOCK_SIZE + 1] = XorCheckSum(SendPack,
					DATA_BLOCK_SIZE + 1);

			if (Send(SendPack, DATA_BLOCK_SIZE + 2) == (DATA_BLOCK_SIZE + 2))
			{
				iRecvLen = Receive(RecvPack, MAX_COMM_PACK_LENGTH);
				if (0x79 != RecvPack[0])
				{
					WriteLog("Error4!");
					return false;
				}
			}
		}
	}

	//Readout Protect command
	SendPack[0] = 0x82;
	SendPack[1] = 0x7D;
	if (Send(SendPack, 2) == 2)
	{
		iRecvLen = Receive(RecvPack, 2);
		if ((0x79 != RecvPack[0]) && (0x79 != RecvPack[1]))
		{
			WriteLog("Readout Protect command failed!");
			return false;
		}
	}

	//	Reset();

	Close();

	return true;
	//return false;
}

int CWiFiComm::ConnectVDM()
{
	int icnt = 3;
	do
	{
		int n;
		if ((m_socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "Failed to socket %s",
					strerror(errno));
			icnt--;
			continue;
		}
		char tempip[19];
		strcpy(tempip, m_szServerIP.c_str());

		struct sockaddr_in addr;
		memset((struct sockaddr_in *) &addr, 0, sizeof(struct sockaddr_in));
		addr.sin_family = AF_INET;

		char* cp = NULL;
		if (NULL != (cp = strchr(tempip, ':')))
		{
			*cp++ = 0;
			addr.sin_port = htons( atoi( cp ) );
		}

		n = inet_addr(tempip);
		if (n != -1)
			memcpy(&addr.sin_addr, &n, sizeof(n));
		else
		{
			icnt--;
			continue;
		}

		inet_aton(tempip, &addr.sin_addr);

		if (connect(m_socketfd, (struct sockaddr*) &addr,
				sizeof(struct sockaddr)) < 0)
		{
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "Failed to connect %s",
					strerror(errno));
			icnt--;
			continue;
		}
		else
		{
			return 1;
		}
	} while (icnt > 0);

	return 0;
}

long CWiFiComm::DisconnectVDM() const
{
	if (m_socketfd != 0)
	{
		close(m_socketfd);
		//m_socketfd = 0;
	}
	return 0;
}

long CWiFiComm::SetIPAddress(const char *pIPAddress)
{
	if (NULL == pIPAddress)
	{
		return -1;
	}

	m_szServerIP = pIPAddress;

	return 0;
}

long CWiFiComm::GetIPAddress(char *pIPAddress)
{
	if (NULL == pIPAddress || m_szServerIP.empty())
	{
		return -1;
	}

	strncpy(pIPAddress, m_szServerIP.c_str(), m_szServerIP.size());

	return 0;
}

/************************************************************************/
/*   connect to specific wifi                                           */
/************************************************************************/

//modified by andy huang 2015.5.22
//刷新VDM固件
bool CWiFiComm::Reset(void)
{
	//发送Wifi控制命令,通过Wifi模块GPIO口模拟DTR,RTS信号
	unsigned char szReset[8];
	memcpy(szReset, "RESET", 5);
	if (Send(szReset, 5) != 5)
		return false;
	else
		return true;
}

//add by andy huang 2015.5.22
bool CWiFiComm::Refresh(void)
{
	unsigned char szRefresh[8];
	memcpy(szRefresh, "REFRESH", 7);
	if (Send(szRefresh, 7) != 7)
		return false;
	return true;
}

bool CWiFiComm::ResetWifi(void)
{
	unsigned char szRefresh[16];
	memcpy(szRefresh, "***RESET***", 11);
	if (Send(szRefresh, 11) != 11)
		return false;
	return true;
}
