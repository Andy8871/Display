// EncrypClass.h : interface of the CEncrypClass class
//
/////////////////////////////////////////////////////////////////////////////
#include "adsStd.h"

#define W 32
#define R 16

class CEncrypClass
{
public: // create from serialization only
	CEncrypClass();

public:
	unsigned char* m_pFileData;
	unsigned int m_nDocLength;
	DWORD m_dwS[2 * R + 4];
	int m_nKey;
private:

	DWORD OffsetAmount(DWORD dwVar);
	DWORD LeftRotate(DWORD dwVar, DWORD dwOffset);
	DWORD RightRotate(DWORD dwVar, DWORD dwOffset);
public:
	void KeyGen(DWORD dwKey);
	void DecodeBuffer(BYTE *m_pFileData,long lBufferLen);
	void EncodeBuffer(BYTE *m_pFileData,long lBufferLen);
};