#include "RunEnvironment.h"
#include "Information.h"
#include "CommWithDisplay.h"
#include "assert.h"
#include "LogCat.h"

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
	if (NULL == m_pCommBuffer)
	{
		LOG_DEBUG(TAG, "NULL == m_pCommBuffer");
		return false;
	}

	m_pComm = CCommBase::GetInstance();
	if (NULL == m_pComm)
	{
		LOG_DEBUG(TAG, "NULL == m_pCommBuffer");
		return false;
	}


	if (!m_pComm->Connect(env))
	{
		return false;
	}
	// 清空发送缓冲区
	Clear();

	char chFuction = (char) DT_DIAGNOSE_RUN_SUCCESS;
	char chDebugFlag = (char) 0x00;

	Add(chFuction); // 功能代码
	Add(chDebugFlag); // 调试标志

	// 读取诊断程序名称
	string strEquipmentName = CInformation::GetEquipmentName();

	// 读取诊断程序版本号
	double fVersion = CInformation::GetVersion();
	char pchVersion[100];
	sprintf(pchVersion, "%.2f", fVersion);

	// 显示内容 :: =〈诊断程序名称〉+〈诊断程序版本号〉
	Add(strEquipmentName); // 诊断程序名称
	Add(pchVersion, strlen(pchVersion) + 1); // 诊断程序版本号

	//add by scf 加入诊断接口的版本号

	//#ifdef LIB_VERSION
	double fLibVersion = fVersion;
	char pchLibVersion[100];
	sprintf(pchLibVersion, "%.2f", fLibVersion);
	Add(pchLibVersion, strlen(pchLibVersion) + 1); // 诊断接口程序版本号
	//#endif
	//end add

	// 发送消息
	SendDataToDisplayWaitResponsion();
	if (0 == m_iDataLengthBuffer)
		return false;

	int nOffset = 0;

	char* szDiagnosePath = m_pCommBuffer;
	nOffset = strlen(szDiagnosePath) + 1;
	CRunEnvironment::SetDiagnosisDirectory(szDiagnosePath);

	char* szLanguage = m_pCommBuffer + nOffset;
	nOffset += strlen(szLanguage) + 1;
	CRunEnvironment::SetLanguage(szLanguage);

	char bDemo = *(m_pCommBuffer + nOffset);
	nOffset += 1;
	CRunEnvironment::SetDemoMode(bDemo);
	return true;
}

/*-----------------------------------------------------------------------------
 功    能：销毁为了与显示程序通信所申请所有资源；
 参数说明：无
 返 回 值：无
 说    明：无
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::DestroyConnect()
{
	m_pComm->Disconnect();
	if(m_pCommBuffer)
	{
		delete []m_pCommBuffer;
		m_pCommBuffer = NULL;
	}
}

/*-----------------------------------------------------------------------------
 功    能：通知显示程序数据已准备好，显示程序取走数据后将回复结果放回缓冲区
 参数说明：无
 返 回 值：无
 说    明：在显示程序将数据取走并应答后，该函数才返回
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::SendDataToDisplayWaitResponsion()
{
	// 数据包长度(包括两个字节长度+数据内容长度)
	short m_iLenth = m_iDataLengthBuffer + 2;

	m_pCommBuffer[0] = (char) (m_iLenth / 256);
	m_pCommBuffer[1] = (char) (m_iLenth % 256);
	int nLen = 0;

	char* szRes = m_pComm->WriteForWait(m_pCommBuffer, m_iLenth, nLen);
	if (szRes != NULL)
	{
		Clear();
		m_iDataLengthBuffer = nLen;
		memcpy(m_pCommBuffer, szRes, nLen);
	}
}

/*-----------------------------------------------------------------------------
 功    能：发送数据到显示程序，立即返回
 参数说明：无
 返 回 值：无
 说    明：
 -----------------------------------------------------------------------------*/
bool CCommunicationWithDisplay::SendDataToDisplayImmediateResponsion(void* p)
{
	return m_pComm->WriteForImmediate(m_pCommBuffer, m_iDataLengthBuffer);
}

/*-----------------------------------------------------------------------------
 功    能：返回数据缓冲区指针
 参数说明：无
 返 回 值：无
 说    明：无
 -----------------------------------------------------------------------------*/
char *CCommunicationWithDisplay::GetBuffer()
{
	return m_pCommBuffer;
}

/*-----------------------------------------------------------------------------
 功    能：清空发送缓冲区
 参数说明：无
 返 回 值：无
 说    明：将发送缓冲区数据长度置0
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::Clear()
{
	m_iDataLengthBuffer = 0;
	memset(m_pCommBuffer, 0, 0xFFFF);
}

/*-----------------------------------------------------------------------------
 功    能：填充功能代码和调试标志
 参数说明：无
 返 回 值：无
 说    明：无
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::Init(char chFuction)
{
	// 清空发送缓冲区
	Clear();

	m_pCommBuffer[2] = chFuction; // 功能代码
	m_pCommBuffer[3] = 0x00; // 调试标志,不需要填充

	m_iDataLengthBuffer = 2;
}

/*-----------------------------------------------------------------------------
 功    能：添加一个字节到发送缓冲区，并修正缓冲区数据长度
 参数说明：添加的字节
 返 回 值：错误代码
 说    明：无
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(char cChar)
{
	m_pCommBuffer[m_iDataLengthBuffer + 2] = cChar;
	m_iDataLengthBuffer += 1;

	return true;
}

/*-----------------------------------------------------------------------------
 功    能：添加数据到发送缓冲区，并修正缓冲区数据长度
 参数说明：pContain 添加的数据地址指针；iAddLength 添加的数据长度。
 返 回 值：错误代码
 说    明：无
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(const char *pContain, short i16Length)
{
	// 将要添加的数据复制到缓冲区的末尾
	memcpy(m_pCommBuffer + m_iDataLengthBuffer + 2, pContain, i16Length);
	// 修正数据报长度
	m_iDataLengthBuffer += i16Length;
	return true;
}

/*-----------------------------------------------------------------------------
 功    能：添加数据到发送缓冲区，并修正缓冲区数据长度
 参数说明：strContain 添加的数据
 返 回 值：错误代码
 说    明：无
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(string strContain)
{
	return Add(strContain.c_str(), strContain.length() + 1);

}

/*-----------------------------------------------------------------------------
 功    能：添加数据到发送缓冲区，并修正缓冲区数据长度
 参数说明：CBinary binData 添加的数据类
 返 回 值：错误代码
 说    明：无
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(CBinary binData)
{
	char *pchContain = (char*) binData.GetBuffer();
	short i16Length = binData.GetSize();

	return Add(pchContain, i16Length);
}

/*-----------------------------------------------------------------------------
 功    能：添加数据到发送缓冲区，并修正缓冲区数据长度
 参数说明：CBinary* pbinData 添加的数据类指针
 返 回 值：错误代码
 说    明：无
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(CBinary* pbinData)
{
	char *pchContain = (char *) pbinData->GetBuffer();
	short i16Length = pbinData->GetSize();

	return Add(pchContain, i16Length);
}

/*-----------------------------------------------------------------------------
 功    能：添加2个字节数据到发送缓冲区，并修正缓冲区数据长度
 参数说明：unsigned short iData
 返 回 值：错误代码
 说    明：无
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(short iData)
{
	short iHigh = iData / 256; // 取高字节
	short iLow = iData % 256; // 取低字节

	char chHigh = (char) iHigh;
	char chLow = (char) iLow;

	m_pCommBuffer[m_iDataLengthBuffer + 2] = chHigh;
	m_pCommBuffer[m_iDataLengthBuffer + 3] = chLow;

	// 修正缓冲区长度
	m_iDataLengthBuffer += 2;

	return true;
}

/*-----------------------------------------------------------------------------
 功    能：添加2个字节数据到发送缓冲区，并修正缓冲区数据长度
 参数说明：unsigned short nData
 返 回 值：错误代码
 说    明：无
 -----------------------------------------------------------------------------*/
short CCommunicationWithDisplay::Add(unsigned short nData)
{
	short iHigh = nData / 256; // 取高字节
	short iLow = nData % 256; // 取低字节

	char chHigh = (char) iHigh;
	char chLow = (char) iLow;

	m_pCommBuffer[m_iDataLengthBuffer + 2] = chHigh;
	m_pCommBuffer[m_iDataLengthBuffer + 3] = chLow;

	// 修正缓冲区长度
	m_iDataLengthBuffer += 2;

	return true;
}

/*-----------------------------------------------------------------------------
 功    能：修正缓冲区数据长度
 参数说明：unsigned short iIndex 数据长度所在位置序号（从1开始数起）
 返 回 值：无
 说    明：仅供有 ”菜单数量“ 的调用
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::IncreaseNum(short iIndex)
{
	unsigned char iHigh = (unsigned char) m_pCommBuffer[iIndex]; // 数据长度的高字节部分
	unsigned char iLow = (unsigned char) m_pCommBuffer[iIndex + 1]; // 数据长度的低字节部分

	int iLenth = 256 * iHigh + iLow + 1; // 将表示的长度加1

	iHigh = iLenth / 256;
	iLow = iLenth % 256;

	m_pCommBuffer[iIndex] = iHigh;
	m_pCommBuffer[iIndex + 1] = iLow;

}

/*-----------------------------------------------------------------------------
 功    能：修正缓冲区数据
 参数说明：无
 返 回 值：无
 说    明：无
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
 功    能：修正缓冲区数据
 参数说明：无
 返 回 值：无
 说    明：无
 -----------------------------------------------------------------------------*/
void CCommunicationWithDisplay::SetFlag(unsigned char bFlag)
{
	m_pCommBuffer[3] = bFlag;
}

/*-----------------------------------------------------------------------------
 功    能：获取缓冲区数据
 参数说明：无
 返 回 值：无
 说    明：无
 -----------------------------------------------------------------------------*/
unsigned char CCommunicationWithDisplay::GetFlag()
{
	return m_pCommBuffer[3];
}

/*-----------------------------------------------------------------------------
 功    能：修正缓冲区数据
 参数说明：无
 返 回 值：无
 说    明：无
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

