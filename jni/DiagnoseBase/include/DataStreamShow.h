#ifndef	__DATASTREAMSHOW_H_
#define	__DATASTREAMSHOW_H_

#include <string>
using namespace std;
#include "basedisplay.h"
#include "adsStd.h"
#include "Binary.h"

#define	DST_NORMAL	0
#define DST_VW		1 //added by johnnyling

class CDataStreamShow :	public CBaseDisplay
{
public:
	CDataStreamShow();
	~CDataStreamShow();

	enum 
	{
		DT_DATA_STREAM          = 20
    };

public:
	enum 
	{
		//BDF_ BUTTON SHOW FORMAT
		BDF_NOMAL = 0,
		BDF_VW = 1
	};

	typedef struct MSG_DATA
	{
		unsigned char CmdId;
		short iIndex;
		unsigned char	chID[6];
		char	chValue[30];
		char	chUnit[20];
	}MSG_DATA, *PMSGDATA;

private:
	short m_iDataStreamFormat;	// 数据流格式
	short m_iDataStreamType;		// 数据流类型,added by johnnyling

public:

	unsigned char AcceptMsg ();

	void Init (CBinary idTitle, string strStdValueLibName = "", unsigned short uiType = DST_NORMAL);

	void Init(CBinary idTitle, CBinary idStdValueLibName, unsigned short uiType = DST_NORMAL);
	//void Init(const char* pTitle = NULL, const char* pStdValueLibName = NULL, unsigned short uiType = DST_NORMAL);
	//void Init(string strTitle=NULL, string strStdValueLibName = NULL, unsigned short uiType = DST_NORMAL);		
	void Init(string strTitle="", string strStdValueLibName = "", unsigned short uiType = DST_NORMAL);		
	void Init(unsigned short uiTopLine,unsigned short uiAllCount,string strTitle="", string strStdValueLibName = "", unsigned short uiType = DST_NORMAL);		

	unsigned short Add(CBinary idDataStream, string strDataStreamValue,unsigned short nIndex=-1,bool bUpdateFlag=false);
	unsigned short Add(CBinary idDataStream, string strDataStreamValue, CBinary idUnit,unsigned short nIndex=-1,bool bUpdateFlag=false);
	
	//hpy added 20131203
	unsigned short Add (string strDataStreamName, string strDataStreamValue, string strDataStreamUnit);

	bool SetFlag(unsigned char iFlag);
	unsigned short Show (bool bFlag=false);
	unsigned short Show (unsigned short &iTop,unsigned short &iNum,bool bFlag=false);
};

#endif	//__DATASTREAMSHOW_H_
