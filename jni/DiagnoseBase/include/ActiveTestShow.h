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
	short m_iTitleLenth;        // 动作测试标题长度
	short m_iButtonNum;		  //按钮数量	
	short m_iHeaderNum;		  //标头数量	
	short m_iAllHeaderTextLen;  //标题文本总长度;
	short m_iDataStreamFormat;  // 数据流格式
	bool m_bAddDataStream;        // 是否已经添加数据流内容
	string	m_strMsg;
};

#endif	//__ACTIVETESTSHOW_H_