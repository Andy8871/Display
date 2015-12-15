#ifndef	__DISPLAY_H_
#define	__DISPLAY_H_

#include <jni.h>
#include <string>
using namespace std;
#include "adsStd.h"
#include "Binary.h"
#include <vector>


#include "MenuShow.h"
#include "TroubleCodeShow.h"
#include "MultiSelectShow.h"
#include "VehicleInfo.h"
#include "ShortTestShow.h"
#include "ActiveTestShow.h"
#ifdef DATA_DRV_EXT
#include "StShow.h"
#endif	// #ifdef DATA_DRV_EXT
#include "DataStreamShow.h"
#include "SpecialFuncTestShow.h"


//#define TASKIDTYPE 2

#define STRING_ONLY		0x80


//用户按键键值
#define	adsIDNOCLICK	0x00
#define	adsIDOK			0x01
#define	adsIDCANCEL		0x02
#define adsIDYES		0x03
#define	adsIDNO			0x04
#define adsIDRETRY		0x05
#define	adsIDIGNORE		0x06
#define	adsIDABORT		0x07
#define	adsIDPREV		0x08
#define	adsIDNEXT		0x09
#define	adsIDFINISH		0x0A
#define	adsIDBACK		0x0B
#define	adsIDCHANNEL	0x0C //AUDI 
#define	adsIDSHORTTESTENTER	0x0D //SHORTTESTENTER
//按钮标志
#define	adsMB_NoButton			0x00
#define	adsMB_OK				0x01
#define	adsMB_Cancel			0x02
#define	adsMB_OKCancel			0x03
#define adsMB_YesNo       		0x04
#define adsMB_YesNoCancel		0x05
#define	adsMB_RetryCancel		0x06
#define	adsMB_AbortRetryIgnore	0x07
#define	adsMB_IgnoreAbort		0x08
#define	adsMB_OK_Return			0x09
#define	adsMB_NextCancel		0x16
#define	adsMB_PrevNextCancel	0x17
#define	adsMB_PrevFinish		0x19
#define	adsMB_Print				0x40
#define	adsMB_Help				0x80

//#define	adsMB_OK_Print			0x49 //带打印按钮
#define	adsMB_OK_Print			0x41 //by modified for adsMB_OK_Print bug

#define V30_BLACK_PRODUCT		1
#define V30_COLOR_PRODUCT		2
#define DK80_WIDE_PRODUCT		3
#define PCMAX_PC_PRODUCT		4
#define PCVCI_PC_PRODUCT		5
#define PCLINK_PC_PRODUCT		6
#define EUROPCMAX_PC_PRODUCT	7
class CDisplay;

CDisplay* getDisplayObject(void);

class CDisplay
{
public:
	static CDisplay* GetInstance();
private:
	static CDisplay* m_pInstance;

	CDisplay():Menu(m_Menu),
		       TroubleCode(m_TroubleCode),
		       DataStream(m_DataStream),
			   ActiveTest(m_ActiveTest),
			   MultiSelect(m_MultiSelect),
		       VehicleInfo(m_VehicleInfo),
		       ShortTest(m_ShortTest),
			   SpecialFuncTest(m_SpecialFuncTest)
	{
	}

	~CDisplay();
	/* 仅仅只用于在系统退出时自动调用其析构函数来释放CDisplay指针对象 */
	class Garbo
	{
		~Garbo()
		{
			if (NULL != CDisplay::m_pInstance)
				delete CDisplay::m_pInstance;
		}
	};

	static Garbo m_Garbo;
public:
	class CInputValue 
    {
	public:
		CInputValue();
		CInputValue(short iKey, string strInput);

	public:
		int GetInteger32();
		short GetUserKey();
		string GetString();
		string GetInput();

	private:
		short m_iUserKey;     // 用户按键键值
		string m_strUserInput;  // 用户输入字符串
	};

public:
    enum 
	{
		//DT_ Display Type
		DT_MENU                 = 0,
		DT_TROUBLE_CODE         = 10,
		DT_DATA_STREAM          = 20,
		DT_VW_DATA_STREAM       = 21,
		DT_ACTIVE_TEST          = 30,
#ifdef DATA_DRV_EXT
		DT_ST = 120,
#endif	// #ifdef DATA_DRV_EXT
		DT_MUTI_SELECT          = 40,
		DT_VEHICLE_INFORMATION  = 50,
		DT_MESSAGE_BOX          = 60,
		DT_SYS_MESSAGE_BOX      = 61,
		DT_INPUT                = 70,
		DT_PROGRESS_BAR         = 80,
		DT_PICTURE              = 90,
		DT_SHORT_TEST           = 100,
		DT_SPECIAL_TEST			= 110,
		DT_LOADSTRING			= 0xfd,
		DT_SYSTEM_EXIT          = 0xfe,
		DT_DIAGNOSE_RUN_SUCCESS = 0xFF,
		DT_ECUID				= 130		//added by bin
    };

	typedef struct tagXYPOINT
	{
		short iX;
		short iY;
	}XYPOINT, *PXYPOINT;



public:
    CMenuShow			&Menu      ;     
	CTroubleCodeShow	&TroubleCode;
	CDataStreamShow 	&DataStream ;
	CActiveTestShow 	&ActiveTest ;
/*
#ifdef DATA_DRV_EXT
	CStShow				&St;
#endif	// #ifdef DATA_DRV_EXT
*/
	CMultiSelectShow	&MultiSelect;
	CVehicleInfo    	&VehicleInfo;
	CShortTestShow  	&ShortTest  ;
	CSpecialFuncTestShow 	&SpecialFuncTest ;

protected:
	static bool m_bIsDebug;

	CCommunicationWithDisplay m_SendAndReceive;
	CMenuShow                     m_Menu; 
	CTroubleCodeShow              m_TroubleCode; 
	CDataStreamShow               m_DataStream; 
	CActiveTestShow               m_ActiveTest;
/*
#ifdef DATA_DRV_EXT
	CStShow							m_St;
#endif	// #ifdef DATA_DRV_EXT
*/
	CMultiSelectShow              m_MultiSelect; 
	CVehicleInfo                  m_VehicleInfo;
	CShortTestShow                m_ShortTest; 
	CSpecialFuncTestShow           m_SpecialFuncTest; 


public:
	static string IdToString(CBinary id);
	static bool Debug();
	static bool Debug(bool bIsDebug);
	bool Init(JNIEnv* env);
	void Destroy();

	/**************************************************************************************
	//用户按键键值
	adsIDNOCLICK	
	adsIDOK			
	adsIDCANCEL		
	adsIDYES		
	adsIDNO			
	adsIDRETRY		
	adsIDIGNORE		
	adsIDABORT		
	adsIDPREV		
	adsIDNEXT		
	adsIDFINISH		
	adsIDBACK		
	adsIDCHANNEL	
	adsIDSHORTTESTENTER	

	//按钮标志
	adsMB_NoButton 
	adsMB_OK				
	adsMB_Cancel			
	adsMB_OKCancel			
	adsMB_YesNo       		
	adsMB_YesNoCancel		
	adsMB_RetryCancel		
	adsMB_AbortRetryIgnore	
	adsMB_IgnoreAbort		
	adsMB_PrevNextCancel	
	adsMB_NextCancel		
	adsMB_PrevFinish		
	adsMB_Print				
	adsMB_Help				
	**************************************************************************************/
	unsigned short MessageBox(const char *pContain, const char *pTitle, unsigned short nFlag = adsMB_OK, unsigned short nView = 0);
	unsigned short MessageBox(string strContain,  string strTitle,  unsigned short nFlag = adsMB_OK, unsigned short nView = 0);	
	unsigned short MessageBox(CBinary idContain,  CBinary idTitle,  unsigned short nFlag = adsMB_OK, unsigned short nView = 0);

	unsigned short SysMessageBox(unsigned short nMsgID, unsigned short nFlag = adsMB_OK);

	CInputValue Input(string strPrompt, string strTitle, string strDefaultString, string strFormat = "" , string strMinString = "", string strMaxString = "");
	CInputValue Input(CBinary idPrompt, CBinary idTitle, string strDefaultString, string strFormat = "" , string strMinString = "", string strMaxString = "");

	unsigned short ProgressBar(string strPrompt, string strTitle, unsigned short uiPercen = 0);
	unsigned short ProgressBar(CBinary idPrompt, CBinary idTitle, unsigned short uiPercen = 0);

	unsigned short Picture(string strPictureFileName, string strTitle, unsigned short nFlag = adsMB_OK, vector< XYPOINT > *point = NULL);
	unsigned short Picture(CBinary idPictureFileName, CBinary idTitle, unsigned short nFlag = adsMB_OK, vector< XYPOINT > *point = NULL);
	char *LoadString(unsigned short strID);
	unsigned short GetDisplayMessage();
	bool GetSendAndReceiveHandle(CCommunicationWithDisplay** ppSendReceive);
	//add by bin 2011.2.16
	void SetECUIDinfo(string info);
};


#endif	// !defined _DISPLAY_
