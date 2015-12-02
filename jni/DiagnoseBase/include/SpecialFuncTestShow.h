#ifndef	__SPECIALFUNCSHOW_H_
#define	__SPECIALFUNCSHOW_H_

#include <string>
using namespace std;
#include "adsStd.h"
#include "basedisplay.h"
#include "Binary.h"


class CSpecialFuncTestShow :	public CBaseDisplay
{
public:
	CSpecialFuncTestShow();

	enum 
	{
		DT_SPECIAL_TEST          = 110
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

	void Init (const char *pTitle = NULL);	
	void Init (string strTitle);
	void Init (CBinary idTitle);

	//hpy added 20131203
	short Add (string strDataStreamName, string strDataStreamValue, string strDataStreamUnit);

	short AddButton (CBinary idButtonText, char byStatus);

	short Add (CBinary idDataStream, string strDataStreamValue);
	short Add (CBinary idDataStream, string strDataStreamValue, CBinary idUnit);
	short Add (string strMsg);
	short AddPrerequisites (const char* strMsg);
	short AddProcessCtrl (short nMin,short nMax,short nSteps);
	bool SetFlag(unsigned char iFlag);
	short Show ();

private:
	short m_iTitleLenth;        // �������Ա��ⳤ��
	short m_iButtonNum;
	short m_iDataStreamFormat;  // ��������ʽ
	bool m_bAddDataStream;        // �Ƿ��Ѿ��������������

};

#endif	//__SpecialFuncShow_H_