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
	class CDsIdSendFrame  // ���������ID�뷢��֡�����±��Ӧ��ϵ
	{				
	public:
		CBinary idDataStream;			// ������ID
		short i16SendFrameOrder;		// ����֡������±�
		CBinary idUnit;					// ��������λ

		//added by johnnyling, avoid searching lib twice
		vector<CBinary> listDsContent;  //��ס��������
	};

protected: 
	CArithmetic m_Arithmetic;						// �㷨��
	bool m_bShowDataStreamSelectWindow;				// �Ƿ���ʾ��������ѡ��
	list<CDsIdSendFrame> m_listDsIdSendFrame;	    // ������ID�뷢��֡��Ӧ��ϵ
	CSendFrame m_SendFrame;			        	// ����֡����
	CReceiveFrame m_ReceiveFrame;		        	// ����֡���飬�±��뷢��֡��Ӧ

public:
	CBaseDataStream(void);
	virtual ~CBaseDataStream(void);

public:
	// �����Ƿ���ʾ������ѡ����
	void EnableShowMultiSelected (bool bEnable);

	// ��ɶ�����������ʾ
	virtual short ReadDataStream (vector < CBinary> *paidDataStream) = 0;

protected:
	// ����Ա����m_SendFrame��m_lstDsIdSearchSendFrame
	virtual void DsIdToSendFrame () = 0; 

	// �㷨�����ܴ�������⴦����Ҫ��ϳ���Ա���ر��������
	//virtual string ExceptionArithmetic (CBinary idDataStream, CReceiveFrame*pReceiveFrame) = 0;
	virtual string ExceptionArithmetic (CBinary idDataStream, CReceiveFrame*pReceiveFrame,vector<string> szParam) = 0;
};

#endif	//__BASEDATASTREAM_H_
