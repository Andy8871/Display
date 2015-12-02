#ifndef __STDDATASTREAM_H_
#define __STDDATASTREAM_H_

#include "basedatastream.h"
#define MAXLINE 50
class CStdDataStream :	public CBaseDataStream
{
public:
	CStdDataStream(void);
	virtual ~CStdDataStream(void);

public:
	// 完成读数据流并显示
	virtual short ReadDataStream (vector<CBinary> *paidDataStream);


	virtual void DsIdToSendFrame (void);

	virtual void OnBeforeSendDsCmd () ;
};

#endif // __STDDATASTREAM_H_