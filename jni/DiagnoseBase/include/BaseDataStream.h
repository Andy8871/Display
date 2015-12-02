#ifndef __BASEDATASTREAM_H_
#define __BASEDATASTREAM_H_

#include "adsStd.h"
#include <vector>
#include <list>
using namespace std;
#include "Binary.h"
#include "Arithmetic.h"
#include "SendFrame.h"
#include "ReceiveFrame.h"


class CBaseDataStream
{
protected:
	class CDsIdSendFrame  // 存放数据流ID与发送帧数组下标对应关系
	{				
	public:
		CBinary idDataStream;			// 数据流ID
		short i16SendFrameOrder;		// 发送帧数组的下标
		CBinary idUnit;					// 数据流单位

		//added by johnnyling, avoid searching lib twice
		vector<CBinary> listDsContent;  //记住查表的内容
	};

protected: 
	CArithmetic m_Arithmetic;						// 算法类
	bool m_bShowDataStreamSelectWindow;				// 是否显示数据流多选窗
	list<CDsIdSendFrame> m_listDsIdSendFrame;	    // 数据流ID与发送帧对应关系
	CSendFrame m_SendFrame;			        	// 发送帧数组
	CReceiveFrame m_ReceiveFrame;		        	// 接收帧数组，下标与发送帧对应

public:
	CBaseDataStream(void);
	virtual ~CBaseDataStream(void);

public:
	// 设置是否显示数据流选择项
	void EnableShowMultiSelected (bool bEnable);

	// 完成读数据流并显示
	virtual short ReadDataStream (vector < CBinary> *paidDataStream) = 0;

protected:
	// 添充成员变量m_SendFrame和m_lstDsIdSearchSendFrame
	virtual void DsIdToSendFrame () = 0; 

	// 算法程序不能处理的例外处理，需要诊断程序员重载本函数完成
	//virtual string ExceptionArithmetic (CBinary idDataStream, CReceiveFrame*pReceiveFrame) = 0;
	virtual string ExceptionArithmetic (CBinary idDataStream, CReceiveFrame*pReceiveFrame,vector<string> szParam) = 0;
};

#endif	//__BASEDATASTREAM_H_
