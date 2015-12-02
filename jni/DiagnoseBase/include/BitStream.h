/***************************************************************************//**
\file		BitStream.h
\brief		declaration of the CBitStream class.
\author		James Zhang
*******************************************************************************/
#if !defined(AFX_BITSTREAM_H__1E82854B_18A7_4BFB_9731_EA3F62E7DB8F__INCLUDED_)
#define AFX_BITSTREAM_H__1E82854B_18A7_4BFB_9731_EA3F62E7DB8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "adsStd.h"

#define MAX_BS_USED_BUF_IN_BYTES	256		//	1024

typedef enum tpEmBitValue
{
	BitValue0 = 0
	,BitValue1 = 1
} emBitValue;

/***************************************************************************//**
\brief	manager the buffer by bits
*******************************************************************************/
class CBitStream  
{
public:
	CBitStream();
	virtual ~CBitStream();

/***************************************************************************//**
\brief		fill an individual bit in internal managed buffer from high-bit to
			low-bit, from low-byte to high-byte 
\param[in]	emBV	bit value "0" or bit value "1" 
\return		bool	true if function succeeded, false otherwise
*******************************************************************************/
	bool FillBit(emBitValue emBV);

/***************************************************************************//**
\brief		find out how many bits filled
\return		unsigned long	count of bits filled
\warning	
*******************************************************************************/
	unsigned long GetFilledBits();

/***************************************************************************//**
\brief		fetch all filled bits to specified buffer (clean up internal managed 
			buffer at the same time)
\param[out]	pbtData		buffer to accommodate fetched data
\param[in]	dwBytes		size of "pbtData" buffer in bytes
\param[out]	dwBytes		length of fetched data in bytes
\return		bool	true if function succeeded, false otherwise
\warning	function will return false immediately if the size of "pbtData" is
			not large enough to accommodate fetched data
*******************************************************************************/
	bool FetchData(OUT unsigned char* pbtData,IN OUT unsigned long& dwBytes);

private:
	void Clear();

private:
	unsigned char	m_btBuf[MAX_BS_USED_BUF_IN_BYTES];
	unsigned long	m_dwUsedBytes;
	unsigned long	m_dwUsedBits;
};

#endif // !defined(AFX_BITSTREAM_H__1E82854B_18A7_4BFB_9731_EA3F62E7DB8F__INCLUDED_)
