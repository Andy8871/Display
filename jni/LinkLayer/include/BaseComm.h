#ifndef	__BASE_COMM_H__
#define	__BASE_COMM_H__
#define MAX_COMM_PACK_LENGTH	260
#define DATA_BLOCK_SIZE			256

#define OPEN_MODE_NORMAL		0x00
#define OPEN_MODE_INIT			0x01
#define OPEN_MODE_REFRESH		0x02
//通信抽象层
class CComm
{
public:
	CComm(void) { };
	virtual ~CComm(void) { };

public:
	virtual	bool	Init ( int iOpenMode) = 0;
	virtual int		Send (unsigned char* pBuffer, int iLength) = 0;
	virtual int		Receive (unsigned char *pBuffer, int iLength) = 0;
	virtual void	Close (void) = 0;
	virtual bool	TestLink(char * pCmdData, int iLength) = 0;
	virtual bool	ResetVDM(char * pData, int uiSize) = 0;
	virtual int 	ClearBuffer() = 0;
protected:
	//XOR校验
	unsigned char XorCheckSum( unsigned char * ucBuf, int uiLen)
	{
		unsigned char ucTmp = 0;
		for (int i=0; i<uiLen; i++)
		{
			ucTmp ^= ucBuf[i];
		}
		return ucTmp;
	}
};
#endif	//__BASE_COMM_H__
