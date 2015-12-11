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
	// ��ȡ�����ļ���������λ��ͨ�Ŷ˿�
	bool OpenPort(void);

	// ��˿��Ѵ���رգ������޶���
	void ClosePort(void);

	// ���������е����ݴ��������
	int	SendData(unsigned char* pSendBuffer, int iSendLength);

	// ����һ�����ݣ����ظ��ջص�֡
	int	ReceiveData(unsigned char* pReceiveBuffer, int iBufferLength);

	// ���ó�ʱʱ��
	void SetTimeout(int msTimeout);
	bool ClearBuffer();

private:
	CComm* m_pCommPort;//CCommPort* m_pCommPort;
	int		m_iTimeout;	        // ��ʱʱ�䣬��λ����
	unsigned char		m_uiPacketID;		// ���Ͱ�ID
};

#endif	//_SENDRECEIVEPACKET_H_
