#include "CrcClass.h"
//#include <fstream.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

//***********************************************
CCrcClass::CCrcClass() : m_pdwCrc32Table(NULL)
{
}

//***********************************************
CCrcClass::~CCrcClass()
{
	Free();
}

//***********************************************
void CCrcClass::Init(void)
{
	// This is the official polynomial used by CRC32 in PKZip.
	// Often times the polynomial shown reversed as 0x04C11DB7.
  //unsigned long dwPolynomial = 0xEDB88320;
	unsigned long dwPolynomial = 0xEDB48340;
	
	int i, j;

	Free();
	m_pdwCrc32Table = new unsigned long[256];

	unsigned long dwCrc;
	for(i = 0; i < 256; i++)
	{
		dwCrc = i;
		for(j = 8; j > 0; j--)
		{
			if(dwCrc & 1)
				dwCrc = (dwCrc >> 1) ^ dwPolynomial;
			else
				dwCrc >>= 1;
		}
		m_pdwCrc32Table[i] = dwCrc;
	}
}

//***********************************************
void CCrcClass::Free(void)
{
	delete m_pdwCrc32Table;
	m_pdwCrc32Table = NULL;
}

//***********************************************
inline void CCrcClass::CalcCrc32(const unsigned char cByte, unsigned long &dwCrc32) const
{
	dwCrc32 = ((dwCrc32) >> 8) ^ m_pdwCrc32Table[(cByte) ^ ((dwCrc32) & 0x000000FF)];
}



//***********************************************
unsigned long CCrcClass::FileCrc(const char* szFilename,long lReverseOffset, unsigned long &dwCrc32) const
{
	FILE *m_pfText = NULL;
	 dwCrc32 = 0xFFFFFFFF;
	try
	{
		m_pfText = fopen(szFilename, "rb");
		if(!m_pfText)
		{
			return 0;
		}
		fseek(m_pfText,0,SEEK_SET);
		fseek(m_pfText,0,SEEK_END);
		long lFileSize = ftell(m_pfText);// 得到文件长度
		long nStart = 0;
		if(m_pfText)
		{
			
			fseek(m_pfText,0,SEEK_SET);

			//int nCount = fread(mac,1,1,m_pfText);				
			//int nCount = fread(mac,1,1024,m_pfText);				
			if(lReverseOffset == 0)
			{	
				char mac[1024];
				while(1)
				{	
					int nCount = fread(mac,1,1024,m_pfText);
					for(int i=0;i<nCount;i++)
					{
						CalcCrc32(mac[i], dwCrc32);
					}
					if(nCount<1024)break;

				}
			}
			else
			{
				/*
				while(nCount)
				{
					CalcCrc32(mac[0], dwCrc32);	
					nStart++;
					if( lFileSize-lReverseOffset == nStart)
						break;
					nCount = fread(mac,1,1,m_pfText);

				}*/
				char *pMac=NULL;
				pMac = new char[lFileSize-lReverseOffset];
				if(!pMac)
				{
					fclose(m_pfText);
					return 0;
				}
				memset(pMac,0,lFileSize-lReverseOffset);
				int nCount = fread(pMac,1,lFileSize-lReverseOffset,m_pfText);
				if(nCount<lFileSize-lReverseOffset)
				{
					if(pMac)
					{
						delete []pMac;
						pMac = NULL;
					}
					fclose(m_pfText);
					return 0;
				}
				else
				{
					for(int i=0;i<lFileSize-lReverseOffset;i++)
					{
						CalcCrc32(pMac[i], dwCrc32);
					}
				}
				if(pMac)
				{
					delete []pMac;
					pMac = NULL;
				}

			}
			fclose(m_pfText);
		}
		else
		{
			return false;
		}
		dwCrc32 = ~dwCrc32;
		return dwCrc32;
	}
	catch(...)
	{
		fclose(m_pfText);
		return 0;
	}
}	


//***********************************************
unsigned long CCrcClass::BufferCrc(const char* pBuffer,long nBufferLen, unsigned long &dwCrc32) const
{
	FILE *m_pfText = NULL;
	 dwCrc32 = 0xFFFFFFFF;

	for(long l=0;l<nBufferLen;l++)
	{
		CalcCrc32(pBuffer[l], dwCrc32);
	}
	dwCrc32 = ~dwCrc32;
	return dwCrc32;
}						
