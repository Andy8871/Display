
#ifndef	__RECEIVEFRAME_H_
#define	__RECEIVEFRAME_H_

#include "adsStd.h"
#include <vector>
using namespace std;
#include "Binary.h"
#include "sendframe.h"
#ifndef CBinaryGroup
#define CBinaryGroup vector<CBinary>
#endif
typedef struct  
{
	//针对Can解析需要的信息。
	CSendFrame sendFrame;
	unsigned char cmdSid;
}CmdParseInfo;


class CReceiveFrame
{
public:
	CReceiveFrame(void);
	~CReceiveFrame(void);

protected:
	vector< CBinaryGroup > m_ReceiveFrame;

public:
	// 复制接收帧
	void operator = (CReceiveFrame ReceiveFrame );

	// 加入新发送帧数据
	void operator += ( CBinaryGroup& ReceiveGroup );

	// 取得某一发送帧的应答帧
	CBinaryGroup& operator[] (const short iFrameOrder);

	// 取得发送帧的数量
	short GetSendFrameNumber ();

	//short SetSendFrameNumber (short iSendFrameNumber);

	// 取得第一发送帧的应答帧
	CBinary GetFirstAnswerFrame();

	// 取得某一发送帧的应答帧
	CBinaryGroup& GetOneFrameAnswer (const short iFrameOrder);

	// 取得接收帧的数组大小
	short size();

	// 清空接收帧数据
	void clear();
	void SetCanParseInfo(CmdParseInfo& info){m_CanParseInfo = info;};
	CmdParseInfo& GetCanParseInfo(void){return m_CanParseInfo;};
private:
	//added by bin.for cmd buffer used in DTC functions.
	CmdParseInfo m_CanParseInfo;
};

#endif	//__RECEIVEFRAME_H_

