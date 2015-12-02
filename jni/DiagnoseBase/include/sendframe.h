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
接收帧数量 ：：= 1个字节，无符号数，0x00只发送不接收，0xFF自动识别，0xFE回答无数帧
0xFF自动识别:此时，下位机会一直侦测线上的数据，直到线上没有数据，超时退出。 比如ECU只回一帧数据，但是下位机在接收完这一帧之后，再试图接一帧，结果ECU线上没有数据，等到一定时间后，超时退出。
0xFE回答无数帧：这个时候ECU会回答无穷帧。下位机一直在接收，直到上位机发一个命令，要下位机发一帧数据出去，下位机转入发送状态
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

	// 设置缺省接收帧帧数
	short	SetDefaultReceiveFrameNumber (short iDefaultReceiveFrame = 1);

	// 加入命令库ID对应的发送帧
	bool AddFromCmdLib(CBinary idCmd);

	// 追加发送帧
	short	AddSendFrame(CSendFrame& SendFrame);

	// 清空发送帧
	void Clear(void);

	// 取得发送帧数量
	short	GetFrameNumber(void);

	// 得到发送帧的返回帧数
	short GetAllReceiveFrameNumber(void);

	// 得到发送帧中单帧的返回帧数
	short GetOneReceiveFrameNumber(int iOrder);

	// 将发送帧命令内容按“[长度＋内容]...”的格式输出到指定的缓冲区,返回填充长度
	int PutCmdDataToBuffer(unsigned char* dataBuf, int bufLength);
};

#endif	// __SENDFRAME_H_
