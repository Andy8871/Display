#ifndef __DISPLAYDIAGNOSECOMMUNICATION_H_ 
#define __DISPLAYDIAGNOSECOMMUNICATION_H_ 


#include <string>
#include <jni.h>
using namespace std;
#include "adsStd.h"
#include "Binary.h"
#include "CommBaseClass.h"
#ifdef WIN32
#include "windows.h"
#endif

// 定义跟显示进程通讯的消息ID
#define ID_Message 34543 //#define ID_Message 30000  
#define ID_Send_Message 40000  

class CCommunicationWithDisplay
{
public:
	CCommunicationWithDisplay();
	~CCommunicationWithDisplay();

protected:
	char *m_pCommBuffer;            // 共享内存缓冲 = 2字节长度 + 数据包内容	
	char *m_pSendBufferSaved;		// 保存上一次发送数据缓存
	char *m_pReceiveBufferSaved;	// 保存上一次接收数据缓存
	unsigned short m_iDataLengthBuffer;       // 数据包内容长度
#ifdef WIN32
	HANDLE hSemaphore;

#endif
	wchar_t m_pwszDisplayBuffer[200];    // 窗口标题



public:
	bool CreateConnect(JNIEnv* env);
	void DestroyConnect();

	void SendDataToDisplayWaitResponsion();
	bool SendDataToDisplayImmediateResponsion(void* p);
	char *GetBuffer();
	int GetBufferLen(){
		return m_iDataLengthBuffer;
	}
	void Clear();	

    enum 
	{
		DT_DIAGNOSE_RUN_SUCCESS = 0xFF
    };

	void Init(char chFuction);

	short Add(char cChar);
	short Add(const char *pContain, short i16Length);    

	short Add(string strContain);

	short Add(CBinary binData);
	short Add(CBinary *pbinData);

	short Add(short iData);
	short Add(unsigned short nData);

	void SetFlag(unsigned char bFlag);
	unsigned char GetFlag();
	void IncreaseNum(short iIndex);    // 将表示数量的那两个字节"+1"
	void Modify(short iData, short iIndex);
	void Modify(char chData, short iIndex);

private:

#ifdef  UNICODE
	void CharToTChar(char szSourceBuf[], int iSourceBufLen, TCHAR wszDestBuf[], int iDestBufLen);
#endif
	CCommBase* m_pComm;

};

#endif // __DISPLAYDIAGNOSECOMMUNICATION_H_ 
