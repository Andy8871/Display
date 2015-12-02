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

// �������ʾ����ͨѶ����ϢID
#define ID_Message 34543 //#define ID_Message 30000  
#define ID_Send_Message 40000  

class CCommunicationWithDisplay
{
public:
	CCommunicationWithDisplay();
	~CCommunicationWithDisplay();

protected:
	char *m_pCommBuffer;            // �����ڴ滺�� = 2�ֽڳ��� + ���ݰ�����	

	unsigned short m_iDataLengthBuffer;       // ���ݰ����ݳ���
#ifdef WIN32
	HANDLE hSemaphore;

#endif
	wchar_t m_pwszDisplayBuffer[200];    // ���ڱ���



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
	void IncreaseNum(short iIndex);    // ����ʾ�������������ֽ�"+1"
	void Modify(short iData, short iIndex);
	void Modify(char chData, short iIndex);

private:

#ifdef  UNICODE
	void CharToTChar(char szSourceBuf[], int iSourceBufLen, TCHAR wszDestBuf[], int iDestBufLen);
#endif
	CCommBase* m_pComm;

};

#endif // __DISPLAYDIAGNOSECOMMUNICATION_H_ 
