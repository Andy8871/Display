
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
	//���Can������Ҫ����Ϣ��
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
	// ���ƽ���֡
	void operator = (CReceiveFrame ReceiveFrame );

	// �����·���֡����
	void operator += ( CBinaryGroup& ReceiveGroup );

	// ȡ��ĳһ����֡��Ӧ��֡
	CBinaryGroup& operator[] (const short iFrameOrder);

	// ȡ�÷���֡������
	short GetSendFrameNumber ();

	//short SetSendFrameNumber (short iSendFrameNumber);

	// ȡ�õ�һ����֡��Ӧ��֡
	CBinary GetFirstAnswerFrame();

	// ȡ��ĳһ����֡��Ӧ��֡
	CBinaryGroup& GetOneFrameAnswer (const short iFrameOrder);

	// ȡ�ý���֡�������С
	short size();

	// ��ս���֡����
	void clear();
	void SetCanParseInfo(CmdParseInfo& info){m_CanParseInfo = info;};
	CmdParseInfo& GetCanParseInfo(void){return m_CanParseInfo;};
private:
	//added by bin.for cmd buffer used in DTC functions.
	CmdParseInfo m_CanParseInfo;
};

#endif	//__RECEIVEFRAME_H_

