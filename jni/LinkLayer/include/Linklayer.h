#include "BaseComm.h"
void WriteLog(const char* pBuff);
int CheckCommPort(void);
CComm *  CreatInstance( int iLinkMode = 0 );
void  DestroyInstance(void);



//extern "C"
//{
//	LINK_API DWORD	Init( void * pComm);
//	LINK_API DWORD	Send(unsigned char *pTxBuffer, int iLength);
//	LINK_API DWORD	Receive(unsigned char *pRxBuffer, int iLength);
//	LINK_API DWORD	CheckLink(void);
//	LINK_API DWORD	ClearBuffer(void);
//	LINK_API DWORD	ResetVDM(void);
//}
