/***************************************************************************//**
\file		StShow.h
\brief		declaration of the CStShow class.
\author		James Zhang
*******************************************************************************/
#ifndef	STSHOW_H
#define	STSHOW_H

#include <string>
using namespace std;
#include "adsStd.h"
#include "basedisplay.h"
#include "Binary.h"


/***************************************************************************//**
\brief	most like CActiveTestShow, manager interactions with "Display.exe" for
		our script-style Fucntional Test | Special Test
*******************************************************************************/
class CStShow :	public CBaseDisplay
{
public:
	CStShow();

	enum 
	{
		DT_ST          = 120
    };

	// BS：BUTTON STATUS
	enum 
	{
		BS_MASK          = 0x01,	// 按钮状态掩码位，表示按钮选中状态
		BS_LOCK          = 0x02     // 按钮状态锁定位，表示按钮锁定状态
		,BS_ENABLED		 = 0x04		// 按钮状态激活位，表示按钮激活状态
    };

public:
	enum 
	{
		//BDF_ BUTTON SHOW FORMAT
		BDF_NOMAL = 0,
		BDF_VW = 1
	};

// 	unsigned char AcceptMsg ();

	void Init(unsigned short usWinNo,string& sTitle,string& sTopLftStc
		,string& sTopRgtEdt,string& sBtmLftEdt,string& sBtmRgtEdt
		,unsigned short uiTopLine=-1,unsigned short uiAllCount=-1);

	short AddButton(string& sBtnTxt, char byStatus);

	//add by scf 2008\2\29
// 	bool SetFlag (unsigned char byFlag);
// 	short AddMsg(string strMsg);
// 	short AddHeaderText(string strHeaderMsg);
	//end add

	short Add(string& sDsName,string& sDsValue,string& sDsUnit);
	
	short Show ();
// 	short Show (short &iSelNum);
	short Show (short &iTop,short &iNum,bool bFlag);
private:
	short m_iTitleLenth;        // 动作测试标题+首行文本+...+...的总长度
	short m_iBtnLength;			// 按钮字串总长度
	short m_iButtonNum;		  //按钮数量	
// 	short m_iHeaderNum;		  //标头数量	
// 	short m_iAllHeaderTextLen;  //标题文本总长度;
// 	short m_iDataStreamFormat;  // 数据流格式
	bool m_bAddDataStream;        // 是否已经添加数据流内容
// 	string	m_strMsg;
	unsigned short m_uiTopLine/*=-1*/;		// DS start index on current page 
	unsigned short m_uiAllCount/*=-1*/;		// DS total counts
};

#endif	//STSHOW_H