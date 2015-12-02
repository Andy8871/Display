#ifndef	__ACTIVETESTSHOW_H_
#define	__ACTIVETESTSHOW_H_

#include <string>
using namespace std;
#include "adsStd.h"
#include "basedisplay.h"
#include "Binary.h"


class CActiveTestShow :	public CBaseDisplay
{
public:
	CActiveTestShow();

	enum 
	{
		DT_ACTIVE_TEST          = 30
    };

	// BS��BUTTON STATUS
	enum 
	{
		BS_MASK          = 0x01,	// ��ť״̬����λ����ʾ��ťѡ��״̬
		BS_LOCK          = 0x02     // ��ť״̬����λ����ʾ��ť����״̬
    };

public:
	enum 
	{
		//BDF_ BUTTON SHOW FORMAT
		BDF_NOMAL = 0,
		BDF_VW = 1
	};

	unsigned char AcceptMsg ();

	void Init (const char *pTitle = NULL);	
	void Init (string strTitle);
	void Init (CBinary idTitle);

	short AddButton (CBinary idButtonText, char byStatus);
	//add by scf 2008\2\29
	bool SetFlag (unsigned char byFlag);
	short AddMsg(string strMsg);
	short AddHeaderText(string strHeaderMsg);
	//end add
	short Add (CBinary idDataStream, string strDataStreamValue);
	short Add (CBinary idDataStream, string strDataStreamValue, CBinary idUnit);
	short Add (CBinary idDataStream, string strValue1, string strValue2);

	//hpy added 20131203
	short Add (string strDataStreamName, string strDataStreamValue, string strDataStreamUnit);

	short Show ();
	short Show (short &iSelNum);
private:
	short m_iTitleLenth;        // �������Ա��ⳤ��
	short m_iButtonNum;		  //��ť����	
	short m_iHeaderNum;		  //��ͷ����	
	short m_iAllHeaderTextLen;  //�����ı��ܳ���;
	short m_iDataStreamFormat;  // ��������ʽ
	bool m_bAddDataStream;        // �Ƿ��Ѿ��������������
	string	m_strMsg;
};

#endif	//__ACTIVETESTSHOW_H_