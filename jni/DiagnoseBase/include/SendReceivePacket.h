#ifndef	_SENDRECEIVEPACKET_H_
#define	_SENDRECEIVEPACKET_H_

#include "adsStd.h"
#include "Binary.h"
#include "SendFrame.h"
#include "ReceiveFrame.h"
#include "../../LinkLayer/include/BaseComm.h"
#define HEAD_FLAG1  (0xAA)
#define HEAD_FLAG2  (0x55)
class CSendReceivePacket
{
public:
	CSendReceivePacket(void);
	~CSendReceivePacket(void);

public:
	short GetErrorCode();
	// 读取配置文件，打开与下位机通信端口
	bool OpenPort(void);

	// 如端口已打开则关闭，否则无动作
	void ClosePort(void);

	// 将缓冲区中的数据打包并发送
	int	SendData(unsigned char* pSendBuffer, int iSendLength);

	// 接收一包数据，并回复收回到帧
	int	ReceiveData(unsigned char* pReceiveBuffer, int iBufferLength);

	// 设置超时时间
	void SetTimeout(int msTimeout);
	bool ClearBuffer();

private:
	CComm* m_pCommPort;//CCommPort* m_pCommPort;
	int		m_iTimeout;	        // 超时时间，单位毫秒
	unsigned char		m_uiPacketID;		// 发送包ID
};

#endif	//_SENDRECEIVEPACKET_H_
