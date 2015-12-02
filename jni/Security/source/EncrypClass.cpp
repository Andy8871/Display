// EncrypClass.cpp : implementation of the CEncrypClass class
//
#include <math.h>
#include "EncrypClass.h"


CEncrypClass::CEncrypClass()
{
	m_nDocLength = 0;
	m_pFileData = NULL;
	m_nKey = 1;
}


// encrypt the file
void CEncrypClass::EncodeBuffer(BYTE *m_pFileData,long lBufferLen)
{
	DWORD* pdwTemp;

	for(W_UINT i = 0; i < lBufferLen; i += 16)
	{
		pdwTemp = (DWORD*)&m_pFileData[i];

		pdwTemp[0] = (pdwTemp[0] - m_dwS[2 * R + 2]);
		pdwTemp[2] = (pdwTemp[2] - m_dwS[2 * R + 3]);

		for(int j = R; j >= 1; j--)
		{
			DWORD temp = pdwTemp[3];
			pdwTemp[3] = pdwTemp[2];
			pdwTemp[2] = pdwTemp[1];
			pdwTemp[1] = pdwTemp[0];
			pdwTemp[0] = temp;

			DWORD t = LeftRotate((pdwTemp[1] * (2 * pdwTemp[1] + 1)),
								OffsetAmount((DWORD)(log((double)W)/log(2.0))));
			DWORD u = LeftRotate((pdwTemp[3] * (2 * pdwTemp[3] + 1)),
								OffsetAmount((DWORD)(log((double)W)/log(2.0))));
			pdwTemp[0] = (RightRotate((pdwTemp[0] - m_dwS[2 * j]), OffsetAmount(u))) ^ t;
			pdwTemp[2] = (RightRotate((pdwTemp[2] - m_dwS[2 * j + 1]), OffsetAmount(t))) ^ u;
		}

		pdwTemp[1] = (pdwTemp[1] - m_dwS[0]);
		pdwTemp[3] = (pdwTemp[3] - m_dwS[1]);
	}
	pdwTemp = NULL;
}

// decrypt the file
void CEncrypClass::DecodeBuffer(BYTE *m_pFileData,long lBufferLen)
{
	DWORD* pdwTemp;

	for(W_UINT i = 0; i < lBufferLen; i += 16)
	{
		pdwTemp = (DWORD*)&m_pFileData[i];

		pdwTemp[1] = (pdwTemp[1] + m_dwS[0]);
		pdwTemp[3] = (pdwTemp[3] + m_dwS[1]);

		for(int j = 1; j <= R; j++)
		{
			DWORD t = LeftRotate((pdwTemp[1] * (2 * pdwTemp[1] + 1)),
								OffsetAmount((DWORD)(log((double)W)/log(2.0))));
			DWORD u = LeftRotate((pdwTemp[3] * (2 * pdwTemp[3] + 1)),
								OffsetAmount((DWORD)(log((double)W)/log(2.0))));
			pdwTemp[0] = (LeftRotate(pdwTemp[0] ^ t, OffsetAmount(u)) + m_dwS[2 * j]);
			pdwTemp[2] = (LeftRotate(pdwTemp[2] ^ u, OffsetAmount(t)) + m_dwS[2 * j + 1]);

			DWORD temp = pdwTemp[0];
			pdwTemp[0] = pdwTemp[1];
			pdwTemp[1] = pdwTemp[2];
			pdwTemp[2] = pdwTemp[3];
			pdwTemp[3] = temp;
		}

		pdwTemp[0] = (pdwTemp[0] + m_dwS[2 * R + 2]);
		pdwTemp[2] = (pdwTemp[2] + m_dwS[2 * R + 3]);
	}
	pdwTemp = NULL;
}

void CEncrypClass::KeyGen(DWORD dwKey)
{
	DWORD P32 = 0xB7E15163;
	DWORD Q32 = 0x9E3779B9;
	DWORD i, A, B;
	DWORD dwByteOne, dwByteTwo, dwByteThree, dwByteFour;

	dwByteOne = dwKey >> 24;
	dwByteTwo = dwKey >> 8;
	dwByteTwo = dwByteTwo & 0x0010;
	dwByteThree = dwKey << 8;
	dwByteThree = dwByteThree & 0x0100;
	dwByteFour = dwKey << 24;

	dwKey = dwByteOne | dwByteTwo | dwByteThree | dwByteFour;

	m_dwS[0] = P32;

	for(i = 1; i < 2 * R + 4; i++)
		m_dwS[i] = m_dwS[i - 1] + Q32;

	i = A = B = 0;

	int v = 3 * max(1, 2 * R + 4);

	for(int s = 1; s <= v; s++)
	{
		A = m_dwS[i] = LeftRotate(m_dwS[i] + A + B, OffsetAmount(3));
		B = dwKey = LeftRotate(dwKey + A + B, OffsetAmount(A + B));

		i = (i + 1) % (2 * R + 4);
	}
}

DWORD CEncrypClass::OffsetAmount(DWORD dwVar)
{
	int nLgw = (int)(log((double)W)/log(2.0));

	dwVar = dwVar << (W - nLgw);
	dwVar = dwVar >> (W - nLgw);
	return dwVar;
}

DWORD CEncrypClass::LeftRotate(DWORD dwVar, DWORD dwOffset)
{
	DWORD temp1, temp2;

	temp1 = dwVar >> (W - dwOffset);
	temp2 = dwVar << dwOffset;
	temp2 = temp2 | temp1;

	return temp2;
}

DWORD CEncrypClass::RightRotate(DWORD dwVar, DWORD dwOffset)
{
	DWORD temp1, temp2;

	temp1 = dwVar << (W - dwOffset);
	temp2 = dwVar >> dwOffset;
	temp2 = temp2 | temp1;

	return temp2;
}