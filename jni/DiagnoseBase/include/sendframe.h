#ifndef __SENDFRAME_H_
#define __SENDFRAME_H_

#include <vector>
using namespace std;
#include "adsStd.h"
#include "Binary.h"


class CSendFrame
{
public:
//RFN_ RECEIVE FRAME NUMBER
/*
����֡���� ����= 1���ֽڣ��޷�������0x00ֻ���Ͳ����գ�0xFF�Զ�ʶ��0xFE�ش�����֡
0xFF�Զ�ʶ��:��ʱ����λ����һֱ������ϵ����ݣ�ֱ������û�����ݣ���ʱ�˳��� ����ECUֻ��һ֡���ݣ�������λ���ڽ�������һ֮֡������ͼ��һ֡�����ECU����û�����ݣ��ȵ�һ��ʱ��󣬳�ʱ�˳���
0xFE�ش�����֡�����ʱ��ECU��ش�����֡����λ��һֱ�ڽ��գ�ֱ����λ����һ�����Ҫ��λ����һ֡���ݳ�ȥ����λ��ת�뷢��״̬
*/

	enum {
		RFN_AUTO_DISTINGUISH = 0xFF,
		RFN_COUNTLESS = 0xFE
	};

public:
	class COneSendFrame 
	{
	public:
		CBinary m_binSendCmd;
		short m_iReceiveFrameNumber;
	};

protected:
	static int m_iDefaultReceiveFrameNumber;
	vector<COneSendFrame> m_SendFrame;

public:
	CSendFrame(void);
	~CSendFrame(void);

public:
	void operator = (CSendFrame SendFrame);

	void operator = (vector<CBinary>& binCmd);
	void operator += (vector<CBinary>& binCmd);

	void operator = (const CBinary binData);
	void operator += (CBinary binData);

	void operator += (CSendFrame& SendFrame);
	void operator += (COneSendFrame& oneSendFrame);
	CSendFrame::COneSendFrame operator [] (short nIndex);

	// ����ȱʡ����֡֡��
	short	SetDefaultReceiveFrameNumber (short iDefaultReceiveFrame = 1);

	// ���������ID��Ӧ�ķ���֡
	bool AddFromCmdLib(CBinary idCmd);

	// ׷�ӷ���֡
	short	AddSendFrame(CSendFrame& SendFrame);

	// ��շ���֡
	void Clear(void);

	// ȡ�÷���֡����
	short	GetFrameNumber(void);

	// �õ�����֡�ķ���֡��
	short GetAllReceiveFrameNumber(void);

	// �õ�����֡�е�֡�ķ���֡��
	short GetOneReceiveFrameNumber(int iOrder);

	// ������֡�������ݰ���[���ȣ�����]...���ĸ�ʽ�����ָ���Ļ�����,������䳤��
	int PutCmdDataToBuffer(unsigned char* dataBuf, int bufLength);
};

#endif	// __SENDFRAME_H_
