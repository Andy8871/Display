#include "RunEnvironment.h"
#include "Information.h"
#include "CommWithDisplay.h"
#include "assert.h"
#include <android/log.h>
#ifdef WIN32
#include <tchar.h>
#endif
#include "ProFile.h"

static const char* TAG = "CCommunicationWithDisplay";

CCommunicationWithDisplay::CCommunicationWithDisplay()
{
	m_pCommBuffer = NULL;
	m_iDataLengthBuffer = 0;

	m_pComm = NULL;
}

CCommunicationWithDisplay::~CCommunicationWithDisplay()
{
	if (NULL != m_pComm)
	{
		delete m_pComm;
		m_pComm = NULL;
	}
	if (NULL != m_pCommBuffer)
	{
		delete[] m_pCommBuffer;
		m_pCommBuffer = NULL;
	}
}

bool CCommunicationWithDisplay::CreateConnect(JNIEnv* env)
{
	m_pCommBuffer = new char[0xFFFF];
	assert(NULL != m_pCommBuffer);
	__android_log_write(ANDROID_LOG_DEBUG, TAG,
			"new CCommBase()");
	m_pComm = new CCommBase();
	if (NULL == m_pComm)
		return false;

	if (!m_pComm->Connect(env))
		return false;
	// ��շ��ͻ�����
	Clear();

	char chFuction = (char) DT_DIAGNOSE_RUN_SUCCESS;
	char chDebugFlag = (char) 0x00;

	Add(chFuction); // ���ܴ���
	Add(chDebugFlag); // ���Ա�־

	// ��ȡ��ϳ�������
	string strEquipmentName = CInformation::GetEquipmentName();

	// ��ȡ��ϳ���汾��
	double fVersion = CInformation::GetVersion();
	char pchVersion[100];
	sprintf(pchVersion, "%.2f", fVersion);

	// ��ʾ���� :: =����ϳ������ơ�+����ϳ���汾�š�
	Add(strEquipmentName); // ��ϳ�������
	Add(pchVersion, strlen(pchVersion) + 1); // ��ϳ���汾��

	//add by scf ������Ͻӿڵİ汾��

	//#ifdef LIB_VERSION
	double fLibVersion = fVersion;
	char pchLibVersion[100];
	sprintf(pchLibVersion, "%.2f", fLibVersion);
	Add(pchLibVersion, strlen(pchLibVersion) + 1); // ��Ͻӿڳ���汾��
	//#endif
	//end add

	// ������Ϣ
	SendDataToDisplayWaitResponsion();
	if (0 == m_iDataLengthBuffer)
		return false;

	int nOffset = 0;

	char* szDiagnosePath = m_pCommBuffer;
	nOffset = strlen(szDiagnosePath) + 1;
	CRunEnvironment::SetDiagnosisDirectory(szDiagnosePath);
	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "SetDiagnosisDirectory>>>>>%s", szDiagnosePath);

	char* szLanguage = m_pCommBuffer + nOffset;
	nOffset += strlen(szLanguage) + 1;
	CRunEnvironment::SetLanguage(szLanguage);
	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "SetDiagnosisDirectory>>>>>%s", szLanguage);

	CRunEnvironment::SetDemoMode(true);
	return true;
}

/*-----------------------------------------------------------------------------
 ��    �ܣ�����Ϊ������ʾ����ͨ��������������Դ��
 ����˵������
 �� �� ֵ����
 ˵    ������
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::DestroyConnect()
{
	m_pComm->Disconnect();
	/*if(m_pCommBuffer)
	 m_pCommBuffer = NULL;*/
}

/*-----------------------------------------------------------------------------
 ��    �ܣ�֪ͨ��ʾ����������׼���ã���ʾ����ȡ�����ݺ󽫻ظ�����Żػ�����
 ����˵������
 �� �� ֵ����
 ˵    ��������ʾ��������ȡ�߲�Ӧ��󣬸ú����ŷ���
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::SendDataToDisplayWaitResponsion()
{
	// ���ݰ�����(���������ֽڳ���+�������ݳ���)
	short m_iLenth = m_iDataLengthBuffer + 2;

	m_pCommBuffer[0] = (char) (m_iLenth / 256);
	m_pCommBuffer[1] = (char) (m_iLenth % 256);
	char buff[8] = {0};
	string strCommData;
	for (int i = 0; i < m_iLenth; ++i)
	{
		sprintf(buff, "0x%02x ", m_pCommBuffer[i]);
		strCommData += buff;
		/*__android_log_print(ANDROID_LOG_DEBUG, TAG,
				"Send data to UI 0x%02x", m_pCommBuffer[i]);*/
	}
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "Send data to UI:%s", strCommData.c_str());
	int nLen = 0;
	char* szRes = m_pComm->WriteForWait(m_pCommBuffer, m_iLenth, nLen);
	/*__android_log_print(ANDROID_LOG_DEBUG, TAG,
					"recv data from UI length ");*/
	if (szRes != NULL)
	{
		Clear();
		m_iDataLengthBuffer = nLen;
		memcpy(m_pCommBuffer, szRes, nLen);
	}
}

/*-----------------------------------------------------------------------------
 ��    �ܣ��������ݵ���ʾ������������
 ����˵������
 �� �� ֵ����
 ˵    ����
 -----------------------------------------------------------------------------*/
bool CCommunicationWithDisplay::SendDataToDisplayImmediateResponsion(void* p)
{
	return m_pComm->WriteForImmediate(m_pCommBuffer, m_iDataLengthBuffer);
}

/*-----------------------------------------------------------------------------
 ��    �ܣ��������ݻ�����ָ��
 ����˵������
 �� �� ֵ����
 ˵    ������
 -----------------------------------------------------------------------------*/
char *CCommunicationWithDisplay::GetBuffer()
{
	return m_pCommBuffer;
}

/*-----------------------------------------------------------------------------
 ��    �ܣ���շ��ͻ�����
 ����˵������
 �� �� ֵ����
 ˵    ���������ͻ��������ݳ�����0
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::Clear()
{
	m_iDataLengthBuffer = 0;
	memset(m_pCommBuffer, 0, 0xFFFF);
}

/*-----------------------------------------------------------------------------
 ��    �ܣ���书�ܴ���͵��Ա�־
 ����˵������
 �� �� ֵ����
 ˵    ������
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::Init(char chFuction)
{
	// ��շ��ͻ�����
	Clear();

	m_pCommBuffer[2] = chFuction; // ���ܴ���
	m_pCommBuffer[3] = 0x00; // ���Ա�־,����Ҫ���

	m_iDataLengthBuffer = 2;
}

/*-----------------------------------------------------------------------------
 ��    �ܣ����һ���ֽڵ����ͻ����������������������ݳ���
 ����˵������ӵ��ֽ�
 �� �� ֵ���������
 ˵    ������
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(char cChar)
{
	m_pCommBuffer[m_iDataLengthBuffer + 2] = cChar;
	m_iDataLengthBuffer += 1;

	return true;
}

/*-----------------------------------------------------------------------------
 ��    �ܣ�������ݵ����ͻ����������������������ݳ���
 ����˵����pContain ��ӵ����ݵ�ַָ�룻iAddLength ��ӵ����ݳ��ȡ�
 �� �� ֵ���������
 ˵    ������
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(const char *pContain, short i16Length)
{
	// ��Ҫ��ӵ����ݸ��Ƶ���������ĩβ
	memcpy(m_pCommBuffer + m_iDataLengthBuffer + 2, pContain, i16Length);
	/*__android_log_print(ANDROID_LOG_DEBUG, TAG,
			"m_pCommBuffer %s", m_pCommBuffer + m_iDataLengthBuffer + 2);*/

	// �������ݱ�����
	m_iDataLengthBuffer += i16Length;

	/*__android_log_print(ANDROID_LOG_DEBUG, TAG,
			"m_iDataLengthBuffer %d", m_iDataLengthBuffer);*/

	return true;
}

/*-----------------------------------------------------------------------------
 ��    �ܣ�������ݵ����ͻ����������������������ݳ���
 ����˵����strContain ��ӵ�����
 �� �� ֵ���������
 ˵    ������
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(string strContain)
{
	return Add(strContain.c_str(), strContain.length() + 1);

}

/*-----------------------------------------------------------------------------
 ��    �ܣ�������ݵ����ͻ����������������������ݳ���
 ����˵����CBinary binData ��ӵ�������
 �� �� ֵ���������
 ˵    ������
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(CBinary binData)
{
	char *pchContain = (char*) binData.GetBuffer();
	short i16Length = binData.GetSize();

	return Add(pchContain, i16Length);
}

/*-----------------------------------------------------------------------------
 ��    �ܣ�������ݵ����ͻ����������������������ݳ���
 ����˵����CBinary* pbinData ��ӵ�������ָ��
 �� �� ֵ���������
 ˵    ������
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(CBinary* pbinData)
{
	char *pchContain = (char *) pbinData->GetBuffer();
	short i16Length = pbinData->GetSize();

	return Add(pchContain, i16Length);
}

/*-----------------------------------------------------------------------------
 ��    �ܣ����2���ֽ����ݵ����ͻ����������������������ݳ���
 ����˵����unsigned short iData
 �� �� ֵ���������
 ˵    ������
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(short iData)
{
	short iHigh = iData / 256; // ȡ���ֽ�
	short iLow = iData % 256; // ȡ���ֽ�

	char chHigh = (char) iHigh;
	char chLow = (char) iLow;

	m_pCommBuffer[m_iDataLengthBuffer + 2] = chHigh;
	m_pCommBuffer[m_iDataLengthBuffer + 3] = chLow;

	// ��������������
	m_iDataLengthBuffer += 2;

	return true;
}

/*-----------------------------------------------------------------------------
 ��    �ܣ����2���ֽ����ݵ����ͻ����������������������ݳ���
 ����˵����unsigned short nData
 �� �� ֵ���������
 ˵    ������
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(unsigned short nData)
{
	short iHigh = nData / 256; // ȡ���ֽ�
	short iLow = nData % 256; // ȡ���ֽ�

	char chHigh = (char) iHigh;
	char chLow = (char) iLow;

	m_pCommBuffer[m_iDataLengthBuffer + 2] = chHigh;
	m_pCommBuffer[m_iDataLengthBuffer + 3] = chLow;

	// ��������������
	m_iDataLengthBuffer += 2;

	/*__android_log_print(ANDROID_LOG_DEBUG, TAG,
			"m_pCommBuffer %s", m_pCommBuffer);*/

	return true;
}

/*-----------------------------------------------------------------------------
 ��    �ܣ��������������ݳ���
 ����˵����unsigned short iIndex ���ݳ�������λ����ţ���1��ʼ����
 �� �� ֵ����
 ˵    ���������� ���˵������� �ĵ���
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::IncreaseNum(short iIndex)
{
	unsigned char iHigh = (unsigned char) m_pCommBuffer[iIndex]; // ���ݳ��ȵĸ��ֽڲ���
	unsigned char iLow = (unsigned char) m_pCommBuffer[iIndex + 1]; // ���ݳ��ȵĵ��ֽڲ���

	int iLenth = 256 * iHigh + iLow + 1; // ����ʾ�ĳ��ȼ�1

	iHigh = iLenth / 256;
	iLow = iLenth % 256;

	m_pCommBuffer[iIndex] = iHigh;
	m_pCommBuffer[iIndex + 1] = iLow;

}

/*-----------------------------------------------------------------------------
 ��    �ܣ���������������
 ����˵������
 �� �� ֵ����
 ˵    ������
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::Modify(short iData, short iIndex)
{
	int iHigh = iData / 256;
	int iLow = iData % 256;

	if (-1 == iData)
	{
		m_pCommBuffer[iIndex] = (char) 0xff;
		m_pCommBuffer[iIndex + 1] = (char) 0xff;
	}
	else
	{
		m_pCommBuffer[iIndex] = (char) iHigh;
		m_pCommBuffer[iIndex + 1] = (char) iLow;
	}
}

/*-----------------------------------------------------------------------------
 ��    �ܣ���������������
 ����˵������
 �� �� ֵ����
 ˵    ������
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::SetFlag(unsigned char bFlag)
{
	m_pCommBuffer[3] = bFlag;
}

/*-----------------------------------------------------------------------------
 ��    �ܣ���ȡ����������
 ����˵������
 �� �� ֵ����
 ˵    ������
 -----------------------------------------------------------------------------*/
unsigned char CCommunicationWithDisplay::GetFlag()
{
	return m_pCommBuffer[3];
}

/*-----------------------------------------------------------------------------
 ��    �ܣ���������������
 ����˵������
 �� �� ֵ����
 ˵    ������
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::Modify(char chData, short iIndex)
{
	m_pCommBuffer[iIndex] = chData;
}

#ifdef  UNICODE
void CCommunicationWithDisplay::CharToTChar(char szSourceBuf[], int iSourceBufLen, TCHAR wszDestBuf[], int iDestBufLen)
{

	memset(wszDestBuf, 0, iDestBufLen);

	TCHAR S[256];
	LCTYPE iType=LOCALE_SNATIVELANGNAME;
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, iType, S, sizeof(S) );

	MultiByteToWideChar(GetACP(), //g_uiCodePage,
			0,
			szSourceBuf,
			-1,
			wszDestBuf,
			iDestBufLen
	);
}
#endif

