#include <android/log.h>

#include "Display.h"
#include "Database.h"
#include "assert.h"
//#include "Net.h"
//#include "ResStr.h"
#include "Information.h"
//#include "SecurityBaseClass.h"
//#pragma comment (lib,"D:\\v30\\adsStd\\w32lib\\SecurityBaseClass.lib")
#ifdef _UNICODE

#include "winuser.h"
#include "pwinuser.h"
#include "pwinreg.h"
#include "pkfuncs.h"
#include "Winbase.h"

#define IOCTL_FMD_INIT				CTL_CODE(FILE_DEVICE_HAL, 103, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FMD_GETINFO        	CTL_CODE(FILE_DEVICE_HAL, 104, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FMD_ERASECHIP	        CTL_CODE(FILE_DEVICE_HAL, 105, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FMD_READSECTOR        CTL_CODE(FILE_DEVICE_HAL, 106, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FMD_WRITESECTOR       CTL_CODE(FILE_DEVICE_HAL, 107, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FMD_DEINIT       		CTL_CODE(FILE_DEVICE_HAL, 108, METHOD_BUFFERED, FILE_ANY_ACCESS)

//20080524 add gep
#define IOCTL_FMD_ERASEBLOCK        CTL_CODE(FILE_DEVICE_HAL, 113, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FMD_ISBADBLOCK        CTL_CODE(FILE_DEVICE_HAL, 114, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FMD_MARKBADBLOCK      CTL_CODE(FILE_DEVICE_HAL, 115, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif

//根据ID从文本库返回字符串
extern string adsGetTextString (CBinary binTextID);

extern void sleep(clock_t wait);
void adsSleep(int iMilliseconds)
{
	sleep(iMilliseconds);
}

/*CDisplay * g_pDisplay = NULL;

CDisplay* getDisplayObject(void)
{
	__android_log_write(ANDROID_LOG_DEBUG, "CDisplay", "getDisplayObject");
	g_pDisplay = new CDisplay();
	return g_pDisplay;
}*/


CDisplay* CDisplay::m_pInstance = NULL;

/* 多线程安全创建单例模式 */
CDisplay* CDisplay::GetInstance()
{
	pthread_mutex_t mutex_t = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&mutex_t, NULL);
	if (NULL == m_pInstance)
	{
		pthread_mutex_lock(&mutex_t);
		if (NULL == m_pInstance)
			m_pInstance = new CDisplay();
		pthread_mutex_unlock(&mutex_t);
	}
	pthread_mutex_destroy(&mutex_t);
	return m_pInstance;
}

bool CDisplay::m_bIsDebug = false;


CDisplay::CInputValue::CInputValue()
{
	m_iUserKey = adsMB_Cancel;
	m_strUserInput = "";
}
CDisplay::CInputValue::CInputValue(short iKey, string strInput)
{
	m_iUserKey = iKey;
	m_strUserInput = strInput;
}

short CDisplay::CInputValue::GetUserKey()
{
	return m_iUserKey;
}

string  CDisplay::CInputValue::GetInput()
{
	return m_strUserInput;
}

string  CDisplay::CInputValue::GetString()
{
	return m_strUserInput;
}

int CDisplay::CInputValue::GetInteger32()
{
	return atoi(m_strUserInput.c_str());
}

CDisplay::~CDisplay()
{
	//m_SendAndReceive.DestroyConnect();
}

/***************************************************************************************
功    能：1、建立与显示程序的连接；
		  2、将CDisplayDiagnoseCommunication成员变量地址传给各个成员变量
参数说明：无
返 回 值：成功-false 失败-true 
说    明：
**************************************************************************************/
#if 1//#if 0	//hpy modified from 0 to 1
bool CDisplay::Init(JNIEnv* env)
{
	m_Menu.SetCommunicationPointer(&m_SendAndReceive); 
	m_TroubleCode.SetCommunicationPointer(&m_SendAndReceive); 
	m_DataStream.SetCommunicationPointer(&m_SendAndReceive); 
	m_ActiveTest.SetCommunicationPointer(&m_SendAndReceive); 
	m_MultiSelect.SetCommunicationPointer(&m_SendAndReceive); 
	m_VehicleInfo.SetCommunicationPointer(&m_SendAndReceive); 
	m_ShortTest.SetCommunicationPointer(&m_SendAndReceive); 
	m_SpecialFuncTest.SetCommunicationPointer(&m_SendAndReceive);
/*
	#ifdef DATA_DRV_EXT
		m_St.SetCommunicationPointer(&m_SendAndReceive);  
	#endif
*/

	bool b=m_SendAndReceive.CreateConnect(env);

//	NetInit();//add by yxp 2007.4.2
	return b;
}
#else
unsigned char CDisplay::Init()
{
	m_Menu.SetCommunicationPointer(&m_SendAndReceive); 
	m_TroubleCode.SetCommunicationPointer(&m_SendAndReceive); 
	m_DataStream.SetCommunicationPointer(&m_SendAndReceive); 
	m_ActiveTest.SetCommunicationPointer(&m_SendAndReceive); 
#ifdef DATA_DRV_EXT
	m_St.SetCommunicationPointer(&m_SendAndReceive); 
// 	m_ScriptParse.SetCommunicationPointer(&m_SendAndReceive); 
#endif	// #ifdef DATA_DRV_EXT
	m_MultiSelect.SetCommunicationPointer(&m_SendAndReceive); 
	m_VehicleInfo.SetCommunicationPointer(&m_SendAndReceive); 
	m_ShortTest.SetCommunicationPointer(&m_SendAndReceive); 
	m_SpecialFuncTest.SetCommunicationPointer(&m_SendAndReceive);
	bool b=m_SendAndReceive.CreateConnect();
	/////////////check license///////////////


	string strLanguage  = CRunEnvironment::GetLanguage();
	string strSerialNumber = CRunEnvironment::GetSerialNumber();

	string strVehicleName = CInformation::GetVehiclesSystemName();
	double fVersion = CInformation::GetVersion();
//	CWarrant warrant;
	int nRet;
	CSecurityBaseClass m_Safe;
	string strFilename;
	int nCount;
	char cMessage[256];
#ifdef _UNICODE
	if(CRunEnvironment::GetDemoMode())
		nRet =0;
	else
//		nRet = warrant.Lisence(strSerialNumber.c_str(),strVehicleName.c_str(),strLanguage.c_str(),fVersion);
	{
		strFilename = CRunEnvironment::GetDiagnosisDirectory();
		char chEnd = *(strFilename.end()-1);
		if(chEnd != '\\' && chEnd != '/')
		{
			strFilename += '\\';
		}
		//strFilename += "license.dat";
		const char *pchFileName = NULL;
 		pchFileName = strFilename.data();
		//nRet = m_Safe.CheckLisence((const char*)strSerialNumber.c_str(),(const char*)pchFileName,(const char*)strVehicleName.c_str(),(const char*)strLanguage.c_str(),fVersion,0);
		m_Safe.Initialize(pchFileName);
		nRet = m_Safe.CheckLisence((const char*)strSerialNumber.c_str(),(const char*)strVehicleName.c_str(),
			(const char*)strLanguage.c_str(),fVersion,0,"","",nCount);
	}
#else
	
	if(CRunEnvironment::GetDemoMode())
		nRet =0;
	else
		//nRet = warrant.Lisence(strSerialNumber.c_str(),strVehicleName.c_str(),strLanguage.c_str(),fVersion);
	{	

		char chStart = *(strSerialNumber.begin());
		unsigned char cphProduct = CRunEnvironment::GetProductType();
		if((chStart == 'V') && ((cphProduct == PCMAX_PC_PRODUCT) || (cphProduct == PCVCI_PC_PRODUCT) || (cphProduct == EUROPCMAX_PC_PRODUCT)) )
		{
			sprintf(cMessage,"License check failed(CHECK_VCI_ERROR), Please contact local distributor!");
			MessageBox(cMessage, "Error");
			Destroy();
			return 0xFF;
		}
		else
		{		
			strFilename = CRunEnvironment::GetDiagnosisDirectory();
			char chEnd = *(strFilename.end()-1);
			if(chEnd != '\\' && chEnd != '/')
			{
				strFilename += '\\';
			}
			//strFilename += "license.dat";
			const char *pchFileName = NULL;
 			pchFileName = strFilename.data();
			//nRet = m_Safe.CheckLisence((const char*)strSerialNumber.c_str(),(const char*)pchFileName,(const char*)strVehicleName.c_str(),(const char*)strLanguage.c_str(),fVersion,0);
			m_Safe.Initialize(pchFileName);
			nRet = m_Safe.CheckLisence((const char*)strSerialNumber.c_str(),(const char*)strVehicleName.c_str(),
				(const char*)strLanguage.c_str(),fVersion,0,"","",nCount);
		}
	}
#endif
	
#if defined(_WIN32) & defined(_DEBUG)
	nRet = CHECK_SUCCESS;	// disable license-check for debugging purpose
#else	//	#if defined(_WIN32) & defined(_DEBUG)

	//nRet = CHECK_SUCCESS;	// disable license-check//aab for debug

	if( (nRet&CHECK_LANGUAGE_ERROR) == CHECK_LANGUAGE_ERROR  \
		|| (nRet&CHECK_SERIALNUMBER_ERROR) == CHECK_SERIALNUMBER_ERROR \
		|| (nRet&CHECK_VECHICLECODE_ERROR) == CHECK_VECHICLECODE_ERROR  \
		|| (nRet&CHECK_VERSION_ERROR) == CHECK_VERSION_ERROR \
		|| (nRet&CHECK_OTHER_ERROR) == CHECK_OTHER_ERROR)
	{
	
		if(strLanguage == "CN")
		{
			sprintf(cMessage,"License检测失败，请与经销商联系!%d",nRet);
		}
		else if(strLanguage == "TW")
		{
			sprintf(cMessage,"License檢測失敗，請與經銷商聯係!%d",nRet);
		}
		else
		{
			sprintf(cMessage,"License check failed,Please contact local distributor!%d",nRet);

		}
		MessageBox(cMessage, "Error");
		Destroy();
		return 0xFF;
		//////////////end check license//////////
	}
	else if(nRet == 6 || nRet == 7)
	{
		if(strLanguage == "CN")
		{
			MessageBox("版本已过期，请与经销商联系！", "Error");
		}
		else if(strLanguage == "TW")
		{
			MessageBox("版本已过期，請與經銷商聯係！", "Error");
		}
		else
		{
			MessageBox("The version have expired,Please contact local distributor!", "Error");	
		}
		Destroy();
		return 0xFF;
	}
	else if(nRet == 0)
	{
		string strDisplayVersion = CRunEnvironment::GetDisplayVersion();

#ifdef _UNICODE
		if(atof(strDisplayVersion.c_str()) <= 3.18)
#else
		if(atof(strDisplayVersion.c_str()) < 1.38) //1.34
#endif
		{
#ifdef _UNICODE
			if(strLanguage == "CN")
			{
				MessageBox("显示程序版本有更新，请下载最新版本并升级", "NOTE");
			}
			else if(strLanguage == "TW")
			{
				MessageBox("显示程序版本有更新，请下载最新版本并升级", "NOTE");
			}
			else
			{
				MessageBox("DISPLAY program have new version,please upgrade it!", "NOTE");	
			}
#else
			if(strLanguage == "CN")
			{
				MessageBox("显示程序版本有更新，请下载最新版本并升级", "NOTE");
			}
			else if(strLanguage == "TW")
			{
				MessageBox("显示程序版本有更新，请下载最新版本并升级", "NOTE");
			}
			else
			{
				MessageBox("DISPLAY program have new version,please download and update it!", "NOTE");	
			}
#endif
			Destroy();
			return 0xFF;
		}
	}
#endif	//	#if defined(_WIN32) & defined(_DEBUG)

	NetInit();//add by yxp 2007.4.2
	return b;

}
#endif

/***************************************************************************************
功    能：销毁与显示程序的通信连接
参数说明：无
返 回 值：无
说    明：调用m_SendAndReceive.DestroyConnect();
**************************************************************************************/
void CDisplay::Destroy()
{
	char chFuction = (char)DT_SYSTEM_EXIT;

	// 填充功能标志和调试标志
	m_SendAndReceive.Init(chFuction);

	char chFlag = 0x00;
	m_SendAndReceive.Add(chFlag);

	// 发送消息
	m_SendAndReceive.SendDataToDisplayWaitResponsion();


	// 获取收到的数据
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	m_SendAndReceive.DestroyConnect();
//	NetExit();// add by yxp 2007.4.2
}
//added by bin for showing ECUID
void CDisplay::SetECUIDinfo(string info){
	char chFuction = (char)DT_ECUID;
	m_SendAndReceive.Init(chFuction);
	
	m_SendAndReceive.Add(info);
	m_SendAndReceive.SendDataToDisplayWaitResponsion();
	return;
}


char *CDisplay::LoadString(unsigned short strID)
{
#ifdef DATA_DRV_EXT
	char chFuction = (char)DT_LOADSTRING;
#else	//	#ifdef DATA_DRV_EXT
	char chFuction = DT_LOADSTRING;
#endif	//	#ifdef DATA_DRV_EXT

	m_SendAndReceive.Init(chFuction);
	
	m_SendAndReceive.Add(strID);
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	char *pchContain = m_SendAndReceive.GetBuffer();
	return pchContain+4;
}
#if 0
/***************************************************************************************
功    能：显示对话框
参数说明：char     *pContain 对话框内容指针，
	      char     *pTitle   对话框标题文本，
          unsigned short nFlag     对话框类型
返 回 值：用户按键键值 
说    明：无
**************************************************************************************/
unsigned short CDisplay::MessageBox(const char *pContain, const char *pTitle, unsigned short nFlag)
{
	assert(NULL != pContain);
	assert(NULL != pTitle);

	if(NULL == pContain || NULL == pTitle)
	{
		return adsIDOK;  // 如果传入空指针，直接返回
	}

	// 清空发送缓冲区
//	m_SendAndReceive.Clear();

	char chFuction = DT_MESSAGE_BOX;

	// 填充功能标志和调试标志
	m_SendAndReceive.Init(chFuction);
	
	// 消息框显示内容 :: = 〈按钮标志〉+〈内容文本字符串〉+〈标题文本字符串〉
	m_SendAndReceive.Add(nFlag);                           // 按纽标志
	m_SendAndReceive.Add(pContain, strlen(pContain)+1);      // 内容提示
	m_SendAndReceive.Add(pTitle,   strlen(pTitle)+1);        // 标题文本

	// 发送消息
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分

	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// 第5个字节表示用户按键键值
	unsigned short nUserKey = pchContain[4];

 	return nUserKey;
}

#endif

/***************************************************************************************
功    能：显示对话框
参数说明：char     *pContain 对话框内容指针，
	      char     *pTitle   对话框标题文本，
          unsigned short nFlag     对话框类型
		  char *pView     对话框显示模式
返 回 值：用户按键键值 
说    明：无
**************************************************************************************/
unsigned short CDisplay::MessageBox(const char *pContain, const char *pTitle, unsigned short nFlag, unsigned short nView)
{
	assert(NULL != pContain);
	assert(NULL != pTitle);

	if(NULL == pContain || NULL == pTitle)
	{
		return adsIDOK;  // 如果传入空指针，直接返回
	}

	// 清空发送缓冲区
//	m_SendAndReceive.Clear();

	char chFuction = DT_MESSAGE_BOX;

	// 填充功能标志和调试标志
	m_SendAndReceive.Init(chFuction);
	
	// 消息框显示内容 :: = 〈按钮标志〉+〈内容文本字符串〉+〈标题文本字符串〉
	m_SendAndReceive.Add(nFlag);                           // 按纽标志
	m_SendAndReceive.Add(pContain, strlen(pContain)+1);      // 内容提示
	m_SendAndReceive.Add(pTitle,   strlen(pTitle)+1);        // 标题文本

	m_SendAndReceive.Add(nView);        // 标题文本
	// 发送消息
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_SendAndReceive.GetBuffer();

	assert(NULL != pchContain);

	for (int i = 0; i < m_SendAndReceive.GetBufferLen(); ++i)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "CDisplay", "GetBuffer -->0x%02x", pchContain[i]);
	}
	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分

	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "CDisplay", "iLenth < 5");
		assert(iLenth < 5);
	}	

	// 第5个字节表示用户按键键值
	unsigned short nUserKey = pchContain[4];

 	return nUserKey;
}


/***************************************************************************************
功    能：显示对话框
参数说明：string strContain 对话框内容提示，
		  string strTitle   对话框标题文本，
          unsigned short nFlag    对话框类型
返 回 值：用户按键键值 
说    明：无
**************************************************************************************/
unsigned short CDisplay::MessageBox(string strContain, string strTitle, unsigned short nFlag, unsigned short nView)
{
	// 调用重载函数
	return MessageBox(strContain.c_str(), strTitle.c_str(), nFlag);

} 

/***************************************************************************************
功    能：显示对话框
参数说明：CBinary idContain 对话框内容ID，
          CBinary idTitle   对话框标题文本ID，
          unsigned short nFlag     对话框类型
返 回 值：用户按键键值 
说    明：无
**************************************************************************************/
unsigned short CDisplay::MessageBox (CBinary idContain, CBinary idTitle, unsigned short nFlag, unsigned short nView)
{
	// 根据ID转换成字符串
	string strContain = adsGetTextString(idContain);
	if(strContain.size()<=0)
		strContain = CDisplay::IdToString(idContain);

	string strTitle   = adsGetTextString(idTitle);
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// 调用重载函数
	return MessageBox(strContain, strTitle, nFlag);
}

/***************************************************************************************
功    能：显示系统消息框
参数说明：          
          unsigned short nMsgID    消息内容ID
          unsigned short nFlag     按钮标志
返 回 值：用户按键键值 
说    明：无
**************************************************************************************/
unsigned short CDisplay::SysMessageBox(unsigned short nMsgID, unsigned short nFlag)
{
	// 清空发送缓冲区
//	m_SendAndReceive.Clear();

	char chFuction = DT_SYS_MESSAGE_BOX;

	// 填充功能标志和调试标志
	m_SendAndReceive.Init(chFuction);
	
	// 消息框显示内容 :: = 〈按钮标志〉+〈内容文本字符串〉+〈标题文本字符串〉
	m_SendAndReceive.Add(nFlag);                           // 按纽标志
	m_SendAndReceive.Add(nMsgID);                          // 内容ID	

	// 发送消息
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分

	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// 第5个字节表示用户按键键值
	unsigned short nUserKey = pchContain[4];

 	return nUserKey;
}

/***************************************************************************************
功    能：输入字符串数据
参数说明：string strPrompt        输入提示文本，
          string strTitle         标题文本，
          string strDefaultString 缺省字符串
          string strFormat        格式串，缺省为空串。  
          string strMinString     最小字符串
          string strMaxString     最大字符串
返 回 值：用户按键键值及输入字符串类 
说    明：1、格式说明："9"表示"0"~"9"之间；"X"表示"0"~"F"之间；
                       "A"表示"A"~"Z"之间；"S"表示全部字符集。
					   	"V" 字母+数字
					   "D"表示日期，格式“080121”
          2、十进制、十六进制字串大小比较按右对齐方式进行
**************************************************************************************/
CDisplay::CInputValue CDisplay::Input(string strPrompt, 
                             string strTitle, 
                             string strDefaultString, 
                             string strFormat, 
                             string strMinString, 
                             string strMaxString)
{
	// 清空发送缓冲区
//	m_SendAndReceive.Clear();

	char chFuction = DT_INPUT;    

	// 填充功能标志和调试标志
	m_SendAndReceive.Init(chFuction);

	// 输入框显示内容 ::=〈内容文本字符串〉+〈标题文本字符串〉+〈缺省值〉+〈输入格式字符串〉+〈最小值〉+〈最大值〉
	m_SendAndReceive.Add(strPrompt);                       // 输入提示文本
	m_SendAndReceive.Add(strTitle);                        // 标题文本
	m_SendAndReceive.Add(strDefaultString);                // 缺省字符串
	m_SendAndReceive.Add(strFormat);                       // 格式串
	m_SendAndReceive.Add(strMinString);                    // 最小字符串
	m_SendAndReceive.Add(strMaxString);                    // 最大字符串

	// 发送消息
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分

	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 6)
	{
		assert(iLenth < 6);
	}	

	// 第5个字节表示用户按键键值
	short iUserKey = pchContain[4];

	// 用户输入的字符串，以\0结尾
    char *pchUserInput = pchContain + 5;
	assert(NULL != pchUserInput);

	// 转换成string类型
	string  strUserInput(pchUserInput);  
		
	CInputValue inputValue(iUserKey, strUserInput);

 	return inputValue;
}

/***************************************************************************************
功    能：输入字符串数据
参数说明：CBinary idPrompt       输入提示文本ID，
		  CBinary idTitle        标题文本ID，
          string strDefaultString 缺省输入长度值，
          string strFormat        格式串，缺省为空串。
          string strMinString     最小字符串
          string strMaxString     最大字符串
返 回 值：用户按键键值及输入值 
说    明：1、格式说明："9"表示"0"~"9"之间；"X"表示"0"~"F"之间；
                       "A"表示"A"~"Z"之间；"S"表示全部字符集。
					   "V" 字母+数字
					   	"D"表示日期，格式“080121”
          2、十进制、十六进制字串大小比较按右对齐方式进行
**************************************************************************************/
CDisplay::CInputValue  CDisplay::Input(CBinary idPrompt, 
	                          CBinary idTitle, 
                              string strDefaultString, 							 
                              string strFormat, 
                              string strMinString, 
                              string strMaxString)
{
	// 根据ID转换成字符串
	string strPrompt = adsGetTextString(idPrompt);
	if(strPrompt.size()<=0)
		strPrompt = CDisplay::IdToString(idPrompt);

	string strTitle  = adsGetTextString(idTitle);
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// 调用重载函数
	return Input(strPrompt, strTitle, strDefaultString, strFormat, strMinString, strMaxString);
}

/***************************************************************************************
功    能：显示进度条
参数说明：string strPrompt  进度条提示文本，
          string strTitle   标题文本ID，
          unsigned short uiPercen 进度条显示百分比（1-100）
返 回 值：用户按键键值，如小于0则为错误代码
说    明：无 
**************************************************************************************/
unsigned short CDisplay::ProgressBar(string strPrompt, string strTitle, unsigned short uiPercen)
{
	// 清空发送缓冲区
//	m_SendAndReceive.Clear();

	char chFuction = DT_PROGRESS_BAR;    
	char chPercen = (char)uiPercen;  // 取低位字节

	// 填充功能标志和调试标志
	m_SendAndReceive.Init(chFuction);

	// 进度条框显示内容 :: = 〈进度数值〉+〈内容文本字符串〉+〈标题文本字符串〉
	m_SendAndReceive.Add(chPercen);                        // 进度数值
	m_SendAndReceive.Add(strPrompt);                       // 输入提示文本
	m_SendAndReceive.Add(strTitle);                        // 标题文本

	// 发送消息
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分

	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// 第5个字节表示用户按键键值
	short iUserKey = pchContain[4];

 	return iUserKey;
}

/***************************************************************************************
功    能：显示进度条
参数说明：CBinary idPrompt进度条提示文本ID，
          CBinary idTitle 标题文本ID，
          unsigned short uiPercen 进度条显示百分比（1-100）
返 回 值：用户按键键值，如小于0则为错误代码 
说    明：无
**************************************************************************************/
unsigned short CDisplay::ProgressBar(CBinary idPrompt, CBinary idTitle, unsigned short uiPercen)
{
	// 根据ID转换成字符串
	string strPrompt = adsGetTextString(idPrompt);
	if(strPrompt.size()<=0)
		strPrompt = CDisplay::IdToString(idPrompt);

	string strTitle  = adsGetTextString(idTitle);
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// 调用重载函数
	return ProgressBar(strPrompt, strTitle, uiPercen);
}


/***************************************************************************************
功    能：显示图片
参数说明：string strPictureFileName 图片文件名，
          string strTitle 图片框标题，
          unsigned short nFlag 按钮类型，缺省为adsMB_OK
          vector< XYPOINT > *point 闪烁点iX, iY坐标的容器指针
返 回 值：用户按键键值，如小于0则为错误代码 
说    明：无
**************************************************************************************/
unsigned short CDisplay::Picture(string strPictureFileName, 
						   string strTitle, 
						   unsigned short nFlag,
						   vector< XYPOINT > *point)
{
	// 清空发送缓冲区
//	m_SendAndReceive.Clear();

	char chFuction = DT_PICTURE;    

	// 填充功能标志和调试标志
	m_SendAndReceive.Init(chFuction);

	short iFlashNumber;
	if(NULL == point)
	{
		iFlashNumber = 0;
	}
	else
	{
		iFlashNumber = point->size();
	}

	// 图片框显示内容 :: =〈按钮标志〉+〈图像文件名〉+〈标题文本字符串〉+〈闪烁点数量〉+ [ [〈闪烁点坐标〉] + … ] 
	m_SendAndReceive.Add(nFlag);                           // 按纽标志
	m_SendAndReceive.Add(strPictureFileName);              // 图像文件名
	m_SendAndReceive.Add(strTitle);                        // 标题文本字符串

	char chFlashNumber = iFlashNumber;
	m_SendAndReceive.Add(chFlashNumber);                   // 闪烁点数量

	for(int iIndex = 0; iIndex < iFlashNumber; iIndex++)
	{
		m_SendAndReceive.Add((*point)[iIndex].iX);               // 闪烁点X坐标
		m_SendAndReceive.Add((*point)[iIndex].iY);               // 闪烁点Y坐标
	}

	// 发送消息
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分

	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// 第5个字节表示用户按键键值
	short iUserKey = pchContain[4];

 	return iUserKey;
}



/***************************************************************************************
功    能：显示图片
参数说明：CBinary idPictureFileName 图片文件名ID，
          CBinary idTitle 图片框标题，
          unsigned short nFlag 按钮类型，缺省为adsMB_OK
          vector< XYPOINT > *point 闪烁点iX, iY坐标的容器指针
返 回 值：用户按键键值，如小于0则为错误代码 
说    明：无
**************************************************************************************/
unsigned short CDisplay::Picture(CBinary idPictureFileName, 
						   CBinary idTitle,						   
					       unsigned short nFlag,
						   vector< XYPOINT > *point)
{
	// 根据ID转换成字符串
	string strPictureFileName = adsGetTextString(idPictureFileName);
	if(strPictureFileName.size()<=0)
		strPictureFileName = CDisplay::IdToString(idPictureFileName);

	string strTitle  = adsGetTextString(idTitle);
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// 调用重载函数
	return Picture(strPictureFileName, strTitle, nFlag, point);
}



bool CDisplay::Debug(bool bIsDebug)
{
	bool bOldDebug = m_bIsDebug;
	m_bIsDebug = bIsDebug;
	return bOldDebug;
}

bool CDisplay::Debug()
{
	return 	m_bIsDebug;
}

string CDisplay::IdToString(CBinary id)
{
	string strId;
	for(int i=0; i<id.GetSize(); i++){
		char szTemp[30];
		sprintf(szTemp, "[0x%02X]", id[i]);
		strId += szTemp;
	}
	strId = "ID:"+strId;

	return strId;
}
/*
实时获取显示界面的消息
*/

unsigned short CDisplay::GetDisplayMessage()
{
	unsigned char *pMsg;
	unsigned char DispCount;
	unsigned char DispValid;
	unsigned char Msg;
	//m_SendAndReceive
    //step 1 : 获得共享内存特定单元地址
    char *pchContain = m_SendAndReceive.GetBuffer();
    pMsg=(unsigned char *)&pchContain[0x10000-4];

	//step 2 : 判断[3]=? 0x55, [0]=?[1]是否相同
    DispValid = pMsg[3];
    DispCount = pMsg[0];
	if(DispValid==0x55)
	{
		if(DispCount!=pMsg[1])
		{
	        //step 3 : 不相同 取[2] Msg  and [1]=[0]//需要实现;
            Msg=pMsg[2];
			pMsg[1]=DispCount;
			return Msg;
		}
	}
    return 0;
}

bool CDisplay::GetSendAndReceiveHandle(CCommunicationWithDisplay **ppSendReceive)
{
	if(!ppSendReceive)
		return false;
		
	*ppSendReceive = &m_SendAndReceive;
	return true;
}
