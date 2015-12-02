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

	// BS��BUTTON STATUS
	enum 
	{
		BS_MASK          = 0x01,	// ��ť״̬����λ����ʾ��ťѡ��״̬
		BS_LOCK          = 0x02     // ��ť״̬����λ����ʾ��ť����״̬
		,BS_ENABLED		 = 0x04		// ��ť״̬����λ����ʾ��ť����״̬
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
	short m_iTitleLenth;        // �������Ա���+�����ı�+...+...���ܳ���
	short m_iBtnLength;			// ��ť�ִ��ܳ���
	short m_iButtonNum;		  //��ť����	
// 	short m_iHeaderNum;		  //��ͷ����	
// 	short m_iAllHeaderTextLen;  //�����ı��ܳ���;
// 	short m_iDataStreamFormat;  // ��������ʽ
	bool m_bAddDataStream;        // �Ƿ��Ѿ��������������
// 	string	m_strMsg;
	unsigned short m_uiTopLine/*=-1*/;		// DS start index on current page 
	unsigned short m_uiAllCount/*=-1*/;		// DS total counts
};

#endif	//STSHOW_H