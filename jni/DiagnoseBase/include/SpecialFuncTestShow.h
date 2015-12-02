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

	// BS：BUTTON STATUS
	enum 
	{
		BS_MASK          = 0x01,	// 按钮状态掩码位，表示按钮选中状态
		BS_LOCK          = 0x02     // 按钮状态锁定位，表示按钮锁定状态
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
	short m_iTitleLenth;        // 动作测试标题长度
	short m_iButtonNum;
	short m_iDataStreamFormat;  // 数据流格式
	bool m_bAddDataStream;        // 是否已经添加数据流内容

};

#endif	//__SpecialFuncShow_H_