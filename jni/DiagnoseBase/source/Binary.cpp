/******************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�Binary.cpp
	�ĵ���ʶ��007�������ƽ̨��ϸ���˵����
	ժ    Ҫ���������ڴ�Ŷ��������ݡ�ʵ�ֶ��������ݵı��桢���ļ���ȡʹ����
				�ü��������Ч��

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����      �汾��       ��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2005.11.25  zhangsh   1.0AX        �������ࡣ                                           
	2006.07.11  wcx       0.2001       GetSize()�ڱ���δ�����ڴ�ʱ����ԭ����
	                                   ��-1��Ϊ����0
	2006.11.08  wcx       0.6000       ����������+�Ĳ���Ϊ����Ϊ��ʱ������ֵ��
	                                   ��
	2006.11.08  wcx       0.6000       ȱʡ���䳤����1000��Ϊ100
	2006.11.08  wcx       0.6000       �����˲�����+���󣬰�����
	                                   1������Ϊ��ʱ����ֵ��
									   2������Ϊ��ʱ������ֵ�Ƿ�������
									   3���ۼӺ����ݳ���С��4ʱ�ڴ��쳣��
	2006.11.08  wcx       0.6001       �����˲�����>=����Ϊ����ʱ�жϴ���
									   
******************************************************************************/

#include <memory.h>
#include <assert.h>
#include "binary.h"

/**************************************************************
��    �ܣ�ȱʡ���캯��
����˵������
�� �� ֵ����
˵    ������ʼ����Ա����m_pbinData = NULL��
**************************************************************/
CBinary::CBinary(void)
{
	m_pbinData = NULL;
}


/**************************************************************
��    �ܣ����캯��
����˵����CBinary& binData-Դ����������
�� �� ֵ����
˵    ������һ�����������ݳ�ʼ��������
**************************************************************/
CBinary::CBinary(const CBinary& binData)
{
	if(binData.m_pbinData == NULL)
	{
		this->m_pbinData = NULL;
	}
	else
	{
//		this->m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
//		assert(this->m_pbinData != NULL);

//		if(binData.m_pbinData->m_pucBinary == NULL)
//		{			
//			return;
//		}

		this->m_pbinData = binData.m_pbinData;
		this->m_pbinData->m_iCitationCount++;//�ṹ���ô�������һ��
	}
}


/**************************************************************
��    �ܣ����캯���������ڴ�
����˵����unsigned short iLength��������ڴ��ֽ���
�� �� ֵ����
˵    ���������Ա����m_pbinData�ڴ棬�����������ڴ���ṹ��
	����ָ�룻
**************************************************************/
CBinary::CBinary(short iLength)
{
	this->m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
	assert(m_pbinData != NULL);

    unsigned char* pDataBuf = new unsigned char[iLength];
	assert(pDataBuf != NULL);

	memset(this->m_pbinData, 0, sizeof(tagBinaryData));
	memset(pDataBuf, 0, iLength);

	this->m_pbinData->m_pucBinary              = pDataBuf;
	this->m_pbinData->m_iAllocLength           = iLength;
	this->m_pbinData->m_iCitationCount         = 1;
	this->m_pbinData->m_iDefaultAddAllocLength = BN_DEFALT_GROW_LENGTH;
}


/**************************************************************
��    �ܣ����캯������������ΪiBufferLength�Ļ�����������pBuffer
	�����ݸ��Ƶ�������
����˵����iBufferLength��������ڴ泤�ȣ�pBuffer���ݻ�����ָ��
�� �� ֵ����
˵    ������
**************************************************************/
CBinary::CBinary(unsigned char *pBuffer, short iBufferLength)
{
	m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
	assert(m_pbinData != NULL);

	unsigned char* pDataBuf = NULL;
	if(iBufferLength < BN_COMMON_ALLOCLENGTH)
	{
		pDataBuf = new unsigned char[BN_COMMON_ALLOCLENGTH];
		assert(pDataBuf != NULL);
	}
	else
	{
		pDataBuf = new unsigned char[iBufferLength];
		assert(pDataBuf != NULL);
	}	

	memset(m_pbinData, 0, sizeof(tagBinaryData));
	memset(pDataBuf, 0, iBufferLength);

	memcpy(pDataBuf, pBuffer, iBufferLength);
	
	this->m_pbinData->m_iDataLength = iBufferLength;
	this->m_pbinData->m_pucBinary   = pDataBuf;

	// ���մ����ֵ�����ڴ�
	if(iBufferLength < BN_COMMON_ALLOCLENGTH)
	{
		this->m_pbinData->m_iAllocLength = BN_COMMON_ALLOCLENGTH;
	}
	else
	{
		this->m_pbinData->m_iAllocLength = iBufferLength;
	}

	this->m_pbinData->m_iCitationCount         = 1;
	this->m_pbinData->m_iDefaultAddAllocLength = BN_DEFALT_GROW_LENGTH;
}

CBinary::CBinary(string& pBuffer, short iBufferLength)
{
	m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
	assert(m_pbinData != NULL);
	
	unsigned char* pDataBuf = NULL;
	if(iBufferLength < BN_COMMON_ALLOCLENGTH)
	{
		pDataBuf = new unsigned char[BN_COMMON_ALLOCLENGTH];
		assert(pDataBuf != NULL);
	}
	else
	{
		pDataBuf = new unsigned char[iBufferLength*5];
		assert(pDataBuf != NULL);
	}	
	
	memset(m_pbinData, 0, sizeof(tagBinaryData));
	memset(pDataBuf, 0, iBufferLength*5);
	
	memcpy(pDataBuf, pBuffer.c_str(), iBufferLength*5);
	
	this->m_pbinData->m_iDataLength = iBufferLength;
	this->m_pbinData->m_pucBinary   = pDataBuf;
	
	// ���մ����ֵ�����ڴ�
	if(iBufferLength < BN_COMMON_ALLOCLENGTH)
	{
		this->m_pbinData->m_iAllocLength = BN_COMMON_ALLOCLENGTH;
	}
	else
	{
		this->m_pbinData->m_iAllocLength = iBufferLength;
	}
	
	this->m_pbinData->m_iCitationCount         = 1;
	this->m_pbinData->m_iDefaultAddAllocLength = BN_DEFALT_GROW_LENGTH;
}

CBinary::CBinary(const char *pBuffer, short iBufferLength)
{
	m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
	assert(m_pbinData != NULL);

	unsigned char* pDataBuf = NULL;
	if(iBufferLength < BN_COMMON_ALLOCLENGTH)
	{
		pDataBuf = new unsigned char[BN_COMMON_ALLOCLENGTH];
		assert(pDataBuf != NULL);
	}
	else
	{
		pDataBuf = new unsigned char[iBufferLength];
		assert(pDataBuf != NULL);
	}	

	memset(m_pbinData, 0, sizeof(tagBinaryData));
	memset(pDataBuf, 0, iBufferLength);

	memcpy(pDataBuf, pBuffer, iBufferLength);
	
	this->m_pbinData->m_iDataLength = iBufferLength;
	this->m_pbinData->m_pucBinary   = pDataBuf;

	// ���մ����ֵ�����ڴ�
	if(iBufferLength < BN_COMMON_ALLOCLENGTH)
	{
		this->m_pbinData->m_iAllocLength = BN_COMMON_ALLOCLENGTH;
	}
	else
	{
		this->m_pbinData->m_iAllocLength = iBufferLength;
	}

	this->m_pbinData->m_iCitationCount         = 1;
	this->m_pbinData->m_iDefaultAddAllocLength = BN_DEFALT_GROW_LENGTH;
}

/**************************************************************
��    �ܣ���������
����˵������
�� �� ֵ����
˵    ������������1������Ϊ0�ͷŻ��������ṹ�ڴ�
**************************************************************/
CBinary::~CBinary(void)
{
	if(NULL == m_pbinData)
	{
		return;
	}

	m_pbinData->m_iCitationCount--;
	if(0 == m_pbinData->m_iCitationCount)
	{
		if(m_pbinData->m_pucBinary != NULL)
		{
			delete [] m_pbinData->m_pucBinary;
			m_pbinData->m_pucBinary = NULL;
		}

		if(m_pbinData != NULL)
		{
			delete [] m_pbinData;
			m_pbinData = NULL;
		}
	}
}


/************************************************************
��    �ܣ���������������ϲ�����������
����˵����CBinary binData�����ϲ�����
�� �� ֵ����
˵    ����ע�������Ĵ���
************************************************************/
CBinary CBinary::operator + (CBinary& binData)
{
	CBinary binTemp;
	
	if(this->m_pbinData == NULL)
		return binData;
	if(binData.m_pbinData == NULL)
		return *this;

	binTemp.m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
	assert(binTemp.m_pbinData != NULL);
	memset(binTemp.m_pbinData, 0, sizeof(tagBinaryData));

	short dataLength = this->m_pbinData->m_iDataLength + binData.m_pbinData->m_iDataLength;
	unsigned char* pBuf = new unsigned char[dataLength];
	assert(pBuf != NULL);

	memcpy(pBuf, this->m_pbinData->m_pucBinary,  this->m_pbinData->m_iDataLength);
	memcpy(pBuf+this->m_pbinData->m_iDataLength, binData.m_pbinData->m_pucBinary, binData.m_pbinData->m_iDataLength);

	binTemp.m_pbinData->m_pucBinary              = pBuf;
	binTemp.m_pbinData->m_iDataLength            = dataLength;
	binTemp.m_pbinData->m_iAllocLength           = dataLength;
	binTemp.m_pbinData->m_iCitationCount         = 1;
	binTemp.m_pbinData->m_iDefaultAddAllocLength = BN_DEFALT_GROW_LENGTH;

	return binTemp;
}


/****************************************************************************
��    �ܣ������������ݺϲ�
����˵������
�� �� ֵ����
˵    ����
****************************************************************************/
CBinary& CBinary::operator += (CBinary& binData)
{
	if(binData.m_pbinData == NULL)
	{
		return *this;
	}

	this->Append(binData.m_pbinData->m_pucBinary, binData.m_pbinData->m_iDataLength);

	return *this;
}

/*************************************************************
��    �ܣ������������ݻ�����β����һ���ֽ�����
����˵������׷�ӵ��ַ�
�� �� ֵ��CBinary&
˵    ������
*************************************************************/

CBinary& CBinary::operator += (unsigned char ucValue)
{
	Append(&ucValue, 1); 

	return *this;
}


/***********************************************************
��    �ܣ���ȡָ��λ�õ��ַ�����
����˵��������λ��
�� �� ֵ��ָ��λ�õ��ֽ�����
˵    ����
***********************************************************/
const unsigned char CBinary::operator [] (short nIndex)const
{
	assert(this->m_pbinData != NULL);
	assert(nIndex >= 0 && nIndex < this->m_pbinData->m_iDataLength) ;

	return (unsigned char)this->m_pbinData->m_pucBinary[nIndex];
}

/*****************************************************
��    �ܣ�������
����˵����CBinary binData��
�� �� ֵ����
˵    �����ṹָ��ָ����binData�ĵ�ַ������������1��
	ע������ֵ�Ĵ���
*****************************************************/
void CBinary::operator = (const CBinary binData)
{
	if(binData.m_pbinData == NULL)//����ֵΪ��
	{
		if(this->m_pbinData != NULL)
		{			
			if(this->m_pbinData->m_iCitationCount > 1)
			{
				this->m_pbinData->m_iCitationCount--;
			}
			else
			{	
				// ��û�б���������������ͷ��Լ�
				delete [] this->m_pbinData->m_pucBinary;
				delete [] this->m_pbinData;
			}
		}

		this->m_pbinData = NULL;
		return;
	}

	// ����ֵΪ��
	if(this->m_pbinData == NULL) 
	{
		this->m_pbinData = binData.m_pbinData;

		// �ṹ���ô�������һ��
		this->m_pbinData->m_iCitationCount++;

		return;
	}

	//���ݷǿգ����ǡ�A��A��
	if(this->m_pbinData == binData.m_pbinData)
	{
		return;
	}

	// ����ֵ��Ϊ�ǿ�
	if(this->m_pbinData->m_iCitationCount > 1 )
	{
		this->m_pbinData->m_iCitationCount--;
	}
	else
	{
		// ��û�б���������������ͷ��Լ�
		delete [] this->m_pbinData->m_pucBinary;
		delete [] this->m_pbinData;
	}

	this->m_pbinData = binData.m_pbinData;

	// �ṹ���ô�������һ��
	this->m_pbinData->m_iCitationCount++;
}

bool CBinary::operator <= (CBinary& binData)
{
	if(*this < binData)
		return true;
	if(*this == binData)
		return true;

	return false;
}

bool CBinary::operator >= (CBinary& binData)
{
//	return binData < *this;
	
	return !(*this < binData);
}

bool CBinary::operator > (CBinary& binData)
{
	return binData <= *this;
}

/******************************************************
��    �ܣ��Ƚ϶��������ƴ��Ĵ�С
����˵�������Ƚϵ���
�� �� ֵ���ȽϽ��
˵    ������ǰ�������ֽڱȽϣ�
          ���bin1���ֽ�����ֵС��bin2�򷵻��棬
		  ������Ȳ��ȣ��򳤶ȴ�Ĵ�
******************************************************/
bool CBinary::operator < (CBinary& binData)
{
	if(binData.m_pbinData == NULL)
	{
		return false;
	}
	else if(this->m_pbinData == NULL)
	{
		return true;
	}


	short lengMin = min(this->m_pbinData->m_iDataLength, binData.m_pbinData->m_iDataLength);
	int result = memcmp(this->m_pbinData->m_pucBinary, binData.m_pbinData->m_pucBinary, lengMin);

	if(result < 0)
	{
		return true;
	}
	else if(result == 0)
	{
		bool bResult = (this->m_pbinData->m_iDataLength < binData.m_pbinData->m_iDataLength) ? true : false;
		return bResult;
	}
	else
	{
		return false;
	}
}


/******************************************************
��    �ܣ��Ƚ϶������������ݵĴ������Ƿ����
����˵�������Ƚϵ���
�� �� ֵ���ȽϽ��
˵    ������ǰ�������ֽڱȽϣ�
          ���bin1���ֽ�����ֵ����bin2�򷵻��棬���򷵻ؼ١�
******************************************************/
bool CBinary::operator == (const CBinary& binData)
{
	// ����п�ֵ
	if(this->m_pbinData == NULL || binData.m_pbinData == NULL)
	{
		if(this->m_pbinData == NULL && binData.m_pbinData == NULL)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if(this->m_pbinData == binData.m_pbinData)
	{
		return true;
	}

	if(this->m_pbinData->m_iDataLength != binData.m_pbinData->m_iDataLength)
	{
		return false;
	}

	int result = memcmp(this->m_pbinData->m_pucBinary, binData.m_pbinData->m_pucBinary, binData.m_pbinData->m_iDataLength);
	return (result == 0)?true:false;
}


/******************************************************
��    �ܣ�ȡָ��λ�õ��ַ�����
����˵��������λ��
�� �� ֵ��ָ��λ�õ��ֽ�����
˵    ������
******************************************************/
unsigned char CBinary::GetAt(short nIndex)
{
	assert(this->m_pbinData != NULL);
	assert((0 <= nIndex) && (nIndex < this->m_pbinData->m_iDataLength)) ;

	return this->m_pbinData->m_pucBinary[nIndex];
}

/******************************************************
��    �ܣ��ı�ָ��λ�õ��ַ�����
����˵��������λ��
�� �� ֵ���ɹ���false  ʧ�ܡ�true 
˵    ���������Ԫ����ԭԪ����ͬ���޲����������޸�����
	���������·����ڴ棬����ԭ�������ݣ�����Ԫ�����ݡ�
******************************************************/
bool CBinary::SetAt(short nIndex, unsigned char ucNewElement)
{
	if(this->m_pbinData == NULL)
	{
		return false;
	}

	if((nIndex >= this->m_pbinData->m_iDataLength) || (nIndex < 0))
	{
		return false;
	}

	if(this->m_pbinData->m_iCitationCount > 1)
	{
		CopySelf(*this);
	}

	this->m_pbinData->m_pucBinary[nIndex] = ucNewElement;
	return true;
}


/******************************************************
��    �ܣ������������ݻ�����β����һ���ֽ�����
����˵������׷�ӵ��ַ�
�� �� ֵ���ɹ���ture  ʧ�ܡ�false 
˵    ������
******************************************************/
bool CBinary::Add(unsigned char cNewElement)
/*{
	return Add((char) ucNewElement);
}

bool CBinary::Add(char cNewElement)
*/
{
	short iLenth = Append(&cNewElement, 1);

	if(1 != iLenth)
	{
		return false;
	}

	return true;
}


/******************************************************************
��    �ܣ�	�趨����������ĳߴ�
����˵����	iAllocLength���»������ߴ�, 
			iGrowBy��ȱʡһ�����ӵ��ڴ浥Ԫ����
�� �� ֵ��	�ɹ���false  ʧ�ܡ�true
˵    ����	
******************************************************************/
bool CBinary::SetAllocLength(short iAllocLength, short iGrowBy)
{
	if(this->m_pbinData == NULL)
	{
		this->m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
		assert(this->m_pbinData != NULL);

		unsigned char* pDataBuf = new unsigned char[iAllocLength];
		assert(pDataBuf != NULL);

		memset(m_pbinData, 0, sizeof(tagBinaryData));
		memset(pDataBuf,   0, iAllocLength);

		this->m_pbinData->m_pucBinary              = pDataBuf;
		this->m_pbinData->m_iAllocLength           = iAllocLength;
		this->m_pbinData->m_iCitationCount         = 1;
		this->m_pbinData->m_iDefaultAddAllocLength = iGrowBy;
	}
	else
	{
		if(iAllocLength < m_pbinData->m_iDataLength)
		{
			return false;
		}

		if(this->m_pbinData->m_iCitationCount> 1)
		{
			CopySelf(*this);
		}

		unsigned char* pDataBuf = new unsigned char[iAllocLength];
		assert(pDataBuf != NULL);

		memcpy(pDataBuf, this->m_pbinData->m_pucBinary, this->m_pbinData->m_iDataLength);
	
		delete [] this->m_pbinData->m_pucBinary;

		this->m_pbinData->m_pucBinary              = pDataBuf;
		this->m_pbinData->m_iAllocLength           = iAllocLength;
		this->m_pbinData->m_iDefaultAddAllocLength = iGrowBy;
	}

	return true;
}


/*****************************************************************
��    �ܣ�ȡ���ݳ���
����˵������
�� �� ֵ�����ݳ���
˵    ������
*****************************************************************/
short CBinary::GetSize(void)
{
	if(this->m_pbinData != NULL)
	{
		return this->m_pbinData->m_iDataLength;
	}
	else
	{
		return 0 ;
	}
}


/*****************************************************************
��    �ܣ�ȡ����������
����˵������
�� �� ֵ�����ݳ���
˵    �������Խӿ�
*****************************************************************/
short CBinary::GetBufSize(void)
{
	if(this->m_pbinData != NULL)
	{
		return this->m_pbinData->m_iAllocLength;
	}
	else
	{
		return -1 ;
	}
}


/*****************************************************************
��    �ܣ�ȡ���ݵ����ô���
����˵������
�� �� ֵ�����ݳ���
˵    �������Խӿ�
*****************************************************************/
short CBinary::GetCitationCount(void)
{
	if(this->m_pbinData != NULL)
	{
		return this->m_pbinData->m_iCitationCount;
	}
	else
	{
		return -1;
	}
}


/*****************************************************************
��    �ܣ�ȡ���ݵĻ�����Ĭ������ֵ
����˵������
�� �� ֵ�����ݳ���
˵    �������Խӿ�
*****************************************************************/
short CBinary::GetDefaultAddAllocLength(void)
{
	if(this->m_pbinData != NULL)
	{
		return this->m_pbinData->m_iDefaultAddAllocLength;
	}
	else
	{
		return -1 ;
	}
}


/****************************************************************
��    �ܣ�ȡ���ݻ�����ָ��
����˵������
�� �� ֵ�����ݻ�����ָ��
˵    ������
****************************************************************/
const unsigned char* CBinary::GetBuffer(void)//const
{
	if(this->m_pbinData != NULL && this->m_pbinData->m_pucBinary != NULL)
	{
		return	this->m_pbinData->m_pucBinary;
	}
	else
	{
		return NULL;
	}
}

/***************************************************************
��    �ܣ�	�����ȡ���ݵ����ݻ�����
����˵����	char* pSrc ���ݻ�����ָ�룻
			short iStart��ȡ��ʼλ�ã�Ĭ��ֵΪ0����ͷ����
			short iLength��ȡ���ȣ�Ĭ��ֵΪ-1������ʼλ�ö�����β
�� �� ֵ��	ʵ�ʶ�ȡ���ֽ���
˵    ����	������ݳ����趨Ϊ����ĳ���
***************************************************************/
short CBinary::ReadBuffer(unsigned char *pucSrc, short iStart, short iLength)
{
	return ReadBuffer((char *)pucSrc, iStart, iLength);
}

short CBinary::ReadBuffer(char *pSrc, short iStart, short iLength)
{
	if(iStart < 0)
	{
		iStart = 0;
	}

	if(this->m_pbinData == NULL)
	{
		return 0;
	}

	// ����������Χ(δ��������ʱ��Ȼ�ᳬ����Χ)
	if(iStart > this->m_pbinData->m_iDataLength-1)
	{
		return 0;
	}

	if(iLength <= BN_ALL_DATA)
	{
		iLength = this->m_pbinData->m_iDataLength-iStart;
		memcpy(pSrc, this->m_pbinData->m_pucBinary+iStart, iLength);
	}
	else
	{
		if((iStart+iLength) > (this->m_pbinData->m_iDataLength))
		{
			iLength = this->m_pbinData->m_iDataLength - iStart;
		}
		memcpy(pSrc, this->m_pbinData->m_pucBinary+iStart, iLength);
	}

	return iLength;
}


/**************************************************************
��    �ܣ�д���ݵ�������
����˵����Ŀ�껺����ָ��
�� �� ֵ��ʵ��д����ֽ���
˵    ����д��ĳ���Ϊ������ݳ���
**************************************************************/
short CBinary::WriteBuffer(const unsigned char* pucSrc, short iWriteLength)
{
	return WriteBuffer((char*)pucSrc, iWriteLength);
}
short CBinary::WriteBuffer(const char* pSrc, short iWriteLength)
{
	assert(pSrc != NULL);

	if(this->m_pbinData == NULL)
	{
		this->m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
		assert(this->m_pbinData != NULL);

		unsigned char* pBuf = NULL;
		if(iWriteLength < BN_COMMON_ALLOCLENGTH)
		{
			pBuf = new unsigned char[BN_COMMON_ALLOCLENGTH];
			assert(pBuf != NULL);
		}
		else
		{
			pBuf = new unsigned char[iWriteLength];
			assert(pBuf != NULL);
		}

		memcpy(pBuf, pSrc, iWriteLength);
		this->m_pbinData->m_pucBinary = pBuf;
		this->m_pbinData->m_iDataLength = iWriteLength;

		if(iWriteLength < BN_COMMON_ALLOCLENGTH)
		{
			this->m_pbinData->m_iAllocLength = BN_COMMON_ALLOCLENGTH;
		}
		else
		{
			this->m_pbinData->m_iAllocLength = iWriteLength;
		}

		this->m_pbinData->m_iCitationCount         = 1;
		this->m_pbinData->m_iDefaultAddAllocLength = BN_DEFALT_GROW_LENGTH;

		return iWriteLength;
	}

	if(this->m_pbinData->m_iCitationCount>1)
	{
		CopySelf(*this);
	}

	if(this->m_pbinData->m_iAllocLength >= iWriteLength)
	{
		memset(this->m_pbinData->m_pucBinary, 0,    this->m_pbinData->m_iAllocLength);
		memcpy(this->m_pbinData->m_pucBinary, pSrc, iWriteLength);
		this->m_pbinData->m_iDataLength = iWriteLength;
	}
	else
	{
		unsigned char* pBuf = new unsigned char[iWriteLength];
		assert(pBuf != NULL);

		memcpy(pBuf, pSrc, iWriteLength);
		
		delete [] this->m_pbinData->m_pucBinary;
		this->m_pbinData->m_pucBinary = pBuf;
		this->m_pbinData->m_iAllocLength = iWriteLength;
		this->m_pbinData->m_iDataLength = iWriteLength;
	}

	return iWriteLength;
}


/**************************************************************
��    �ܣ�������׷�ӵ��������ݵĽ�β
����˵����char*pBuffer     ���ݻ�����ָ�룻
	      short iLength  ���볤��
�� �� ֵ��ʵ�ʶ�����ֽ���
˵    ����
**************************************************************/
short CBinary::Append(unsigned char *pBuffer, short iWriteLength)
/*
{
	return Append((char *)pucBuffer, iWriteLength);
}

short CBinary::Append(char *pBuffer, short iWriteLength)
*/
{
	assert(pBuffer != NULL);

	if(iWriteLength <= 0)
	{
		return 0;
	}

	if(this->m_pbinData == NULL)
	{
		m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
		assert(m_pbinData != NULL);

		unsigned char* pBuf = NULL;
		if(iWriteLength < BN_COMMON_ALLOCLENGTH)
		{
			pBuf = new unsigned char[BN_COMMON_ALLOCLENGTH];
			assert(pBuf != NULL);
		}
		else
		{
			pBuf = new unsigned char[iWriteLength];
			assert(pBuf != NULL);
		}

		memcpy(pBuf, pBuffer, iWriteLength);
		this->m_pbinData->m_pucBinary = pBuf;
		this->m_pbinData->m_iDataLength = iWriteLength;

		if(iWriteLength < BN_COMMON_ALLOCLENGTH)
		{
			this->m_pbinData->m_iAllocLength = BN_COMMON_ALLOCLENGTH;
		}
		else
		{
			this->m_pbinData->m_iAllocLength = iWriteLength;
		}

		this->m_pbinData->m_iCitationCount         = 1;
		this->m_pbinData->m_iDefaultAddAllocLength = BN_DEFALT_GROW_LENGTH;

		return iWriteLength;
	}


	if(this->m_pbinData->m_iCitationCount > 1)
	{
		CopySelf(*this);
	}

	if(this->m_pbinData->m_iAllocLength >= 	this->m_pbinData->m_iDataLength+iWriteLength)
	{
		memcpy((this->m_pbinData->m_pucBinary+this->m_pbinData->m_iDataLength),
				pBuffer,
				iWriteLength);
		this->m_pbinData->m_iDataLength += iWriteLength;
	}
	else
	{
		short iBufLength;
		if(iWriteLength < BN_DEFALT_GROW_LENGTH)
		{
			iBufLength = this->m_pbinData->m_iDataLength + BN_DEFALT_GROW_LENGTH;
		}
		else
		{
			iBufLength = this->m_pbinData->m_iDataLength + iWriteLength;
		}

		unsigned char* pBuf = new unsigned char[iBufLength];
		assert(pBuf != NULL);

		memcpy(pBuf, this->m_pbinData->m_pucBinary, this->m_pbinData->m_iDataLength);
		memcpy(pBuf+this->m_pbinData->m_iDataLength, pBuffer, iWriteLength);

		delete [] this->m_pbinData->m_pucBinary;
		this->m_pbinData->m_pucBinary = pBuf;
		this->m_pbinData->m_iDataLength = this->m_pbinData->m_iDataLength + iWriteLength;
		this->m_pbinData->m_iAllocLength = iBufLength;		
	}

	return iWriteLength;
}


/**************************************************************
��    �ܣ��ж��������Ƿ�Ϊ��
����˵������
�� �� ֵ��Ϊ�ա�true; ��Ϊ�ա�false 
˵    �������Խӿ�
**************************************************************/
bool CBinary::IsEmpty(void)
{
	if(this->m_pbinData == NULL)
	{
		return true;
	}

	if(this->m_pbinData->m_iDataLength > 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/**************************************************************
��    �ܣ��ͷ�δʹ�õ��ڴ�
����˵������
�� �� ֵ���� 
˵    ������
**************************************************************/
void CBinary::FreeExtra(void)
{
	if(this->m_pbinData == NULL)
	{
		return;
	}

	if(this->m_pbinData->m_iAllocLength > this->m_pbinData->m_iDataLength)
	{
		if(this->m_pbinData->m_iCitationCount > 1)
		{
			CopySelf(*this);
		}

		short iBufLength;
		iBufLength = this->m_pbinData->m_iDataLength;
		unsigned char* pBuf = new unsigned char[iBufLength];
		assert(pBuf != NULL);

		memcpy(pBuf, this->m_pbinData->m_pucBinary, this->m_pbinData->m_iDataLength);

		delete [] this->m_pbinData->m_pucBinary;
		this->m_pbinData->m_pucBinary = pBuf;
		this->m_pbinData->m_iAllocLength = iBufLength;
		this->m_pbinData->m_iDataLength = iBufLength;
	}
}


/**************************************************************
��    �ܣ������������ظ�����
����˵����CBinary binData����Ҫ���Ƶ������
�� �� ֵ���� 
˵    �������ƣ���ԭ�ṹ�����ô�����Ҫ��һ��˽�нӿ�
**************************************************************/
void CBinary::CopySelf(CBinary binData)
{
	if(binData.m_pbinData == NULL)
	{
		return;
	}	 

	this->m_pbinData = (tagBinaryData*)new unsigned char[sizeof(tagBinaryData)];
	assert(this->m_pbinData != NULL);

	short iBufLength = binData.m_pbinData->m_iAllocLength;
	unsigned char* pBuf = new unsigned char[iBufLength];
	assert(pBuf != NULL);

	memcpy(pBuf, binData.m_pbinData->m_pucBinary, iBufLength);

	this->m_pbinData->m_pucBinary              = pBuf;	
	this->m_pbinData->m_iDataLength            = binData.m_pbinData->m_iDataLength;
	this->m_pbinData->m_iAllocLength           = binData.m_pbinData->m_iAllocLength;
	this->m_pbinData->m_iCitationCount         = 1;
	this->m_pbinData->m_iDefaultAddAllocLength = BN_DEFALT_GROW_LENGTH;

	// ԭ�ṹ�����ô�����һ
	binData.m_pbinData->m_iCitationCount--; 
}
