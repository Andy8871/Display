/***************************************************************************//**
\file		BitStream.cpp
\brief		implementation of the CBitStream class
\author		James Zhang
*******************************************************************************/
#include "BitStream.h"
#include "DataCommon.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitStream::CBitStream()
{
	Clear();
}

CBitStream::~CBitStream()
{

}

bool CBitStream::FillBit(emBitValue emBV)
{
	/** to avoid the case: buffer used up... */
	AuxIfFRetF0(m_dwUsedBytes<MAX_BS_USED_BUF_IN_BYTES);

	if(emBV==BitValue1)
	{
		m_btBuf[m_dwUsedBytes] |= (0x01<<(7-m_dwUsedBits));
	}

	m_dwUsedBits++;
	if(m_dwUsedBits==BITSINBYTE)
	{
		m_dwUsedBytes++;
		m_dwUsedBits = 0;
	}

	return true;
}

unsigned long CBitStream::GetFilledBits()
{
	return (m_dwUsedBytes*BITSINBYTE+m_dwUsedBits);	
}

bool CBitStream::FetchData(OUT unsigned char* pbtData,IN OUT unsigned long& dwBytes)
{
	AuxIfFRetF0(pbtData);
	const unsigned long cdw_used_buf_len = m_dwUsedBytes+(m_dwUsedBits?1:0);
	AuxIfFRetF0(dwBytes>=cdw_used_buf_len);

	memcpy(pbtData,m_btBuf,cdw_used_buf_len);
	dwBytes = cdw_used_buf_len;

	Clear();
	return true;	
}

void CBitStream::Clear()
{
	ZeroStruct(m_btBuf);
	m_dwUsedBytes = 0;
	m_dwUsedBits = 0;
}

