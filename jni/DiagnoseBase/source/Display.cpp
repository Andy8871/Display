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

//����ID���ı��ⷵ���ַ���
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

/* ���̰߳�ȫ��������ģʽ */
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
��    �ܣ�1����������ʾ��������ӣ�
		  2����CDisplayDiagnoseCommunication��Ա������ַ����������Ա����
����˵������
�� �� ֵ���ɹ�-false ʧ��-true 
˵    ����
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
			sprintf(cMessage,"License���ʧ�ܣ����뾭������ϵ!%d",nRet);
		}
		else if(strLanguage == "TW")
		{
			sprintf(cMessage,"License�z�yʧ����Ո�c���N���S!%d",nRet);
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
			MessageBox("�汾�ѹ��ڣ����뾭������ϵ��", "Error");
		}
		else if(strLanguage == "TW")
		{
			MessageBox("�汾�ѹ��ڣ�Ո�c���N���S��", "Error");
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
				MessageBox("��ʾ����汾�и��£����������°汾������", "NOTE");
			}
			else if(strLanguage == "TW")
			{
				MessageBox("��ʾ����汾�и��£����������°汾������", "NOTE");
			}
			else
			{
				MessageBox("DISPLAY program have new version,please upgrade it!", "NOTE");	
			}
#else
			if(strLanguage == "CN")
			{
				MessageBox("��ʾ����汾�и��£����������°汾������", "NOTE");
			}
			else if(strLanguage == "TW")
			{
				MessageBox("��ʾ����汾�и��£����������°汾������", "NOTE");
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
��    �ܣ���������ʾ�����ͨ������
����˵������
�� �� ֵ����
˵    ��������m_SendAndReceive.DestroyConnect();
**************************************************************************************/
void CDisplay::Destroy()
{
	char chFuction = (char)DT_SYSTEM_EXIT;

	// ��书�ܱ�־�͵��Ա�־
	m_SendAndReceive.Init(chFuction);

	char chFlag = 0x00;
	m_SendAndReceive.Add(chFlag);

	// ������Ϣ
	m_SendAndReceive.SendDataToDisplayWaitResponsion();


	// ��ȡ�յ�������
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
��    �ܣ���ʾ�Ի���
����˵����char     *pContain �Ի�������ָ�룬
	      char     *pTitle   �Ի�������ı���
          unsigned short nFlag     �Ի�������
�� �� ֵ���û�������ֵ 
˵    ������
**************************************************************************************/
unsigned short CDisplay::MessageBox(const char *pContain, const char *pTitle, unsigned short nFlag)
{
	assert(NULL != pContain);
	assert(NULL != pTitle);

	if(NULL == pContain || NULL == pTitle)
	{
		return adsIDOK;  // ��������ָ�룬ֱ�ӷ���
	}

	// ��շ��ͻ�����
//	m_SendAndReceive.Clear();

	char chFuction = DT_MESSAGE_BOX;

	// ��书�ܱ�־�͵��Ա�־
	m_SendAndReceive.Init(chFuction);
	
	// ��Ϣ����ʾ���� :: = ����ť��־��+�������ı��ַ�����+�������ı��ַ�����
	m_SendAndReceive.Add(nFlag);                           // ��Ŧ��־
	m_SendAndReceive.Add(pContain, strlen(pContain)+1);      // ������ʾ
	m_SendAndReceive.Add(pTitle,   strlen(pTitle)+1);        // �����ı�

	// ������Ϣ
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���

	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// ��5���ֽڱ�ʾ�û�������ֵ
	unsigned short nUserKey = pchContain[4];

 	return nUserKey;
}

#endif

/***************************************************************************************
��    �ܣ���ʾ�Ի���
����˵����char     *pContain �Ի�������ָ�룬
	      char     *pTitle   �Ի�������ı���
          unsigned short nFlag     �Ի�������
		  char *pView     �Ի�����ʾģʽ
�� �� ֵ���û�������ֵ 
˵    ������
**************************************************************************************/
unsigned short CDisplay::MessageBox(const char *pContain, const char *pTitle, unsigned short nFlag, unsigned short nView)
{
	assert(NULL != pContain);
	assert(NULL != pTitle);

	if(NULL == pContain || NULL == pTitle)
	{
		return adsIDOK;  // ��������ָ�룬ֱ�ӷ���
	}

	// ��շ��ͻ�����
//	m_SendAndReceive.Clear();

	char chFuction = DT_MESSAGE_BOX;

	// ��书�ܱ�־�͵��Ա�־
	m_SendAndReceive.Init(chFuction);
	
	// ��Ϣ����ʾ���� :: = ����ť��־��+�������ı��ַ�����+�������ı��ַ�����
	m_SendAndReceive.Add(nFlag);                           // ��Ŧ��־
	m_SendAndReceive.Add(pContain, strlen(pContain)+1);      // ������ʾ
	m_SendAndReceive.Add(pTitle,   strlen(pTitle)+1);        // �����ı�

	m_SendAndReceive.Add(nView);        // �����ı�
	// ������Ϣ
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������
	char *pchContain = m_SendAndReceive.GetBuffer();

	assert(NULL != pchContain);

	for (int i = 0; i < m_SendAndReceive.GetBufferLen(); ++i)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "CDisplay", "GetBuffer -->0x%02x", pchContain[i]);
	}
	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���

	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "CDisplay", "iLenth < 5");
		assert(iLenth < 5);
	}	

	// ��5���ֽڱ�ʾ�û�������ֵ
	unsigned short nUserKey = pchContain[4];

 	return nUserKey;
}


/***************************************************************************************
��    �ܣ���ʾ�Ի���
����˵����string strContain �Ի���������ʾ��
		  string strTitle   �Ի�������ı���
          unsigned short nFlag    �Ի�������
�� �� ֵ���û�������ֵ 
˵    ������
**************************************************************************************/
unsigned short CDisplay::MessageBox(string strContain, string strTitle, unsigned short nFlag, unsigned short nView)
{
	// �������غ���
	return MessageBox(strContain.c_str(), strTitle.c_str(), nFlag);

} 

/***************************************************************************************
��    �ܣ���ʾ�Ի���
����˵����CBinary idContain �Ի�������ID��
          CBinary idTitle   �Ի�������ı�ID��
          unsigned short nFlag     �Ի�������
�� �� ֵ���û�������ֵ 
˵    ������
**************************************************************************************/
unsigned short CDisplay::MessageBox (CBinary idContain, CBinary idTitle, unsigned short nFlag, unsigned short nView)
{
	// ����IDת�����ַ���
	string strContain = adsGetTextString(idContain);
	if(strContain.size()<=0)
		strContain = CDisplay::IdToString(idContain);

	string strTitle   = adsGetTextString(idTitle);
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// �������غ���
	return MessageBox(strContain, strTitle, nFlag);
}

/***************************************************************************************
��    �ܣ���ʾϵͳ��Ϣ��
����˵����          
          unsigned short nMsgID    ��Ϣ����ID
          unsigned short nFlag     ��ť��־
�� �� ֵ���û�������ֵ 
˵    ������
**************************************************************************************/
unsigned short CDisplay::SysMessageBox(unsigned short nMsgID, unsigned short nFlag)
{
	// ��շ��ͻ�����
//	m_SendAndReceive.Clear();

	char chFuction = DT_SYS_MESSAGE_BOX;

	// ��书�ܱ�־�͵��Ա�־
	m_SendAndReceive.Init(chFuction);
	
	// ��Ϣ����ʾ���� :: = ����ť��־��+�������ı��ַ�����+�������ı��ַ�����
	m_SendAndReceive.Add(nFlag);                           // ��Ŧ��־
	m_SendAndReceive.Add(nMsgID);                          // ����ID	

	// ������Ϣ
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���

	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// ��5���ֽڱ�ʾ�û�������ֵ
	unsigned short nUserKey = pchContain[4];

 	return nUserKey;
}

/***************************************************************************************
��    �ܣ������ַ�������
����˵����string strPrompt        ������ʾ�ı���
          string strTitle         �����ı���
          string strDefaultString ȱʡ�ַ���
          string strFormat        ��ʽ����ȱʡΪ�մ���  
          string strMinString     ��С�ַ���
          string strMaxString     ����ַ���
�� �� ֵ���û�������ֵ�������ַ����� 
˵    ����1����ʽ˵����"9"��ʾ"0"~"9"֮�䣻"X"��ʾ"0"~"F"֮�䣻
                       "A"��ʾ"A"~"Z"֮�䣻"S"��ʾȫ���ַ�����
					   	"V" ��ĸ+����
					   "D"��ʾ���ڣ���ʽ��080121��
          2��ʮ���ơ�ʮ�������ִ���С�Ƚϰ��Ҷ��뷽ʽ����
**************************************************************************************/
CDisplay::CInputValue CDisplay::Input(string strPrompt, 
                             string strTitle, 
                             string strDefaultString, 
                             string strFormat, 
                             string strMinString, 
                             string strMaxString)
{
	// ��շ��ͻ�����
//	m_SendAndReceive.Clear();

	char chFuction = DT_INPUT;    

	// ��书�ܱ�־�͵��Ա�־
	m_SendAndReceive.Init(chFuction);

	// �������ʾ���� ::=�������ı��ַ�����+�������ı��ַ�����+��ȱʡֵ��+�������ʽ�ַ�����+����Сֵ��+�����ֵ��
	m_SendAndReceive.Add(strPrompt);                       // ������ʾ�ı�
	m_SendAndReceive.Add(strTitle);                        // �����ı�
	m_SendAndReceive.Add(strDefaultString);                // ȱʡ�ַ���
	m_SendAndReceive.Add(strFormat);                       // ��ʽ��
	m_SendAndReceive.Add(strMinString);                    // ��С�ַ���
	m_SendAndReceive.Add(strMaxString);                    // ����ַ���

	// ������Ϣ
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���

	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 6)
	{
		assert(iLenth < 6);
	}	

	// ��5���ֽڱ�ʾ�û�������ֵ
	short iUserKey = pchContain[4];

	// �û�������ַ�������\0��β
    char *pchUserInput = pchContain + 5;
	assert(NULL != pchUserInput);

	// ת����string����
	string  strUserInput(pchUserInput);  
		
	CInputValue inputValue(iUserKey, strUserInput);

 	return inputValue;
}

/***************************************************************************************
��    �ܣ������ַ�������
����˵����CBinary idPrompt       ������ʾ�ı�ID��
		  CBinary idTitle        �����ı�ID��
          string strDefaultString ȱʡ���볤��ֵ��
          string strFormat        ��ʽ����ȱʡΪ�մ���
          string strMinString     ��С�ַ���
          string strMaxString     ����ַ���
�� �� ֵ���û�������ֵ������ֵ 
˵    ����1����ʽ˵����"9"��ʾ"0"~"9"֮�䣻"X"��ʾ"0"~"F"֮�䣻
                       "A"��ʾ"A"~"Z"֮�䣻"S"��ʾȫ���ַ�����
					   "V" ��ĸ+����
					   	"D"��ʾ���ڣ���ʽ��080121��
          2��ʮ���ơ�ʮ�������ִ���С�Ƚϰ��Ҷ��뷽ʽ����
**************************************************************************************/
CDisplay::CInputValue  CDisplay::Input(CBinary idPrompt, 
	                          CBinary idTitle, 
                              string strDefaultString, 							 
                              string strFormat, 
                              string strMinString, 
                              string strMaxString)
{
	// ����IDת�����ַ���
	string strPrompt = adsGetTextString(idPrompt);
	if(strPrompt.size()<=0)
		strPrompt = CDisplay::IdToString(idPrompt);

	string strTitle  = adsGetTextString(idTitle);
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// �������غ���
	return Input(strPrompt, strTitle, strDefaultString, strFormat, strMinString, strMaxString);
}

/***************************************************************************************
��    �ܣ���ʾ������
����˵����string strPrompt  ��������ʾ�ı���
          string strTitle   �����ı�ID��
          unsigned short uiPercen ��������ʾ�ٷֱȣ�1-100��
�� �� ֵ���û�������ֵ����С��0��Ϊ�������
˵    ������ 
**************************************************************************************/
unsigned short CDisplay::ProgressBar(string strPrompt, string strTitle, unsigned short uiPercen)
{
	// ��շ��ͻ�����
//	m_SendAndReceive.Clear();

	char chFuction = DT_PROGRESS_BAR;    
	char chPercen = (char)uiPercen;  // ȡ��λ�ֽ�

	// ��书�ܱ�־�͵��Ա�־
	m_SendAndReceive.Init(chFuction);

	// ����������ʾ���� :: = ��������ֵ��+�������ı��ַ�����+�������ı��ַ�����
	m_SendAndReceive.Add(chPercen);                        // ������ֵ
	m_SendAndReceive.Add(strPrompt);                       // ������ʾ�ı�
	m_SendAndReceive.Add(strTitle);                        // �����ı�

	// ������Ϣ
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���

	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// ��5���ֽڱ�ʾ�û�������ֵ
	short iUserKey = pchContain[4];

 	return iUserKey;
}

/***************************************************************************************
��    �ܣ���ʾ������
����˵����CBinary idPrompt��������ʾ�ı�ID��
          CBinary idTitle �����ı�ID��
          unsigned short uiPercen ��������ʾ�ٷֱȣ�1-100��
�� �� ֵ���û�������ֵ����С��0��Ϊ������� 
˵    ������
**************************************************************************************/
unsigned short CDisplay::ProgressBar(CBinary idPrompt, CBinary idTitle, unsigned short uiPercen)
{
	// ����IDת�����ַ���
	string strPrompt = adsGetTextString(idPrompt);
	if(strPrompt.size()<=0)
		strPrompt = CDisplay::IdToString(idPrompt);

	string strTitle  = adsGetTextString(idTitle);
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// �������غ���
	return ProgressBar(strPrompt, strTitle, uiPercen);
}


/***************************************************************************************
��    �ܣ���ʾͼƬ
����˵����string strPictureFileName ͼƬ�ļ�����
          string strTitle ͼƬ����⣬
          unsigned short nFlag ��ť���ͣ�ȱʡΪadsMB_OK
          vector< XYPOINT > *point ��˸��iX, iY���������ָ��
�� �� ֵ���û�������ֵ����С��0��Ϊ������� 
˵    ������
**************************************************************************************/
unsigned short CDisplay::Picture(string strPictureFileName, 
						   string strTitle, 
						   unsigned short nFlag,
						   vector< XYPOINT > *point)
{
	// ��շ��ͻ�����
//	m_SendAndReceive.Clear();

	char chFuction = DT_PICTURE;    

	// ��书�ܱ�־�͵��Ա�־
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

	// ͼƬ����ʾ���� :: =����ť��־��+��ͼ���ļ�����+�������ı��ַ�����+����˸��������+ [ [����˸�����꡵] + �� ] 
	m_SendAndReceive.Add(nFlag);                           // ��Ŧ��־
	m_SendAndReceive.Add(strPictureFileName);              // ͼ���ļ���
	m_SendAndReceive.Add(strTitle);                        // �����ı��ַ���

	char chFlashNumber = iFlashNumber;
	m_SendAndReceive.Add(chFlashNumber);                   // ��˸������

	for(int iIndex = 0; iIndex < iFlashNumber; iIndex++)
	{
		m_SendAndReceive.Add((*point)[iIndex].iX);               // ��˸��X����
		m_SendAndReceive.Add((*point)[iIndex].iY);               // ��˸��Y����
	}

	// ������Ϣ
	m_SendAndReceive.SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������
	char *pchContain = m_SendAndReceive.GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���

	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// ��5���ֽڱ�ʾ�û�������ֵ
	short iUserKey = pchContain[4];

 	return iUserKey;
}



/***************************************************************************************
��    �ܣ���ʾͼƬ
����˵����CBinary idPictureFileName ͼƬ�ļ���ID��
          CBinary idTitle ͼƬ����⣬
          unsigned short nFlag ��ť���ͣ�ȱʡΪadsMB_OK
          vector< XYPOINT > *point ��˸��iX, iY���������ָ��
�� �� ֵ���û�������ֵ����С��0��Ϊ������� 
˵    ������
**************************************************************************************/
unsigned short CDisplay::Picture(CBinary idPictureFileName, 
						   CBinary idTitle,						   
					       unsigned short nFlag,
						   vector< XYPOINT > *point)
{
	// ����IDת�����ַ���
	string strPictureFileName = adsGetTextString(idPictureFileName);
	if(strPictureFileName.size()<=0)
		strPictureFileName = CDisplay::IdToString(idPictureFileName);

	string strTitle  = adsGetTextString(idTitle);
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// �������غ���
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
ʵʱ��ȡ��ʾ�������Ϣ
*/

unsigned short CDisplay::GetDisplayMessage()
{
	unsigned char *pMsg;
	unsigned char DispCount;
	unsigned char DispValid;
	unsigned char Msg;
	//m_SendAndReceive
    //step 1 : ��ù����ڴ��ض���Ԫ��ַ
    char *pchContain = m_SendAndReceive.GetBuffer();
    pMsg=(unsigned char *)&pchContain[0x10000-4];

	//step 2 : �ж�[3]=? 0x55, [0]=?[1]�Ƿ���ͬ
    DispValid = pMsg[3];
    DispCount = pMsg[0];
	if(DispValid==0x55)
	{
		if(DispCount!=pMsg[1])
		{
	        //step 3 : ����ͬ ȡ[2] Msg  and [1]=[0]//��Ҫʵ��;
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
