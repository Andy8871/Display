// EncryptFile.cpp: implementation of the CEncryptFile class.
//
//////////////////////////////////////////////////////////////////////aaa
#include <string.h>
#include "EncryptFile.h"
#include "rijndaelRunlog.h"
#include "adsStd.h"
// 加密基本的块大小(版本2)
#define BASE_BLOCK_SIZE_ENCRYPT 16

///////////////////////////////////////////////////////////////////////////
// Construction.
CEncryptFile::CEncryptFile()
{
	m_fileStream = NULL;
	m_nSuspendedCount = 0;
	// content wait to write
	m_lpszWriteDataSuspended = new unsigned char[BASE_BLOCK_SIZE_ENCRYPT];
	// Encryptor
	m_pEncryptor = new Rijndael;
}

///////////////////////////////////////////////////////////////////////////
// Destructor 
// If you  have not closed the object before destroying the object, the destructor 
// closes it for you. 
//
CEncryptFile::~CEncryptFile()
{
	Close();
	if(m_lpszWriteDataSuspended)
	{
		delete m_lpszWriteDataSuspended;
	}
	if(m_pEncryptor)
	{
		delete m_pEncryptor;
	}
}

///////////////////////////////////////////////////////////////////////////
// Closes the resource associated with this object, and makes the FILE 
// unavailable for reading or writing .
// If you  have not closed the object before destroying the object, the destructor 
// closes it for you. 
//
void CEncryptFile::Close()
{
	if(!m_fileStream)
	{
		return;
	}

	//写入剩余的字节数据
	//
	EncryptWrite(NULL, 0, true);

	//关闭资源
	//
	fclose(m_fileStream);
	m_fileStream = NULL;
	m_nSuspendedCount = 0;
}

///////////////////////////////////////////////////////////////////////////
// Open a file, and attach it with this object.
//
// Parameters:
//		filename
//			Filename
//		mode
//			Type of access permitted. More detail please see fopen(...) API.
// 
// Return:
//		true if the open was successful; otherwise false. 
bool CEncryptFile::Open( const char *filename, const char *mode )
{
	if(!m_lpszWriteDataSuspended
		||	!m_pEncryptor)	//没能分配到内存
	{
		return false;
	}

	Close();
	
	if(!InitEncryptor())	// 初始化加密机
	{
		return false;
	}

	m_fileStream = fopen(filename, mode );
	if(m_fileStream)
	{
		//判断是否用"a"或"a+"方式打开
		bool bAppending = false;
		for(int i = strlen(mode)-1; i >= 0; i--)
		{
			if(mode[i]=='a')
			{
				bAppending = true;
				break;
			}
		}
		if(!bAppending)
		{
			return WriteEncryptHeadInfo();
		}
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////
// Writes data to FILE which opened by Open(...) function.
//
// Parameters:
//		buffer
//			Pointer to data to be written
//		nCount
//			size in bytes to be written
//
// Return:
//		true if successful; otherwise false. 
//bool CEncryptFile::Write(const void *buffer, unsigned int nCount, bool bWriteNow)//by for debug
bool CEncryptFile::Write(const void *buffer, unsigned int nCount)
{
	if(!m_fileStream
		|| !buffer
		|| !nCount)
	{
		return false;
	}

	return EncryptWrite(buffer, nCount, false);
	//return EncryptWrite(buffer, nCount, bWriteNow);//by for debug
}

// Write the file header Encrypt info.
bool CEncryptFile::WriteEncryptHeadInfo()
{

	//写入加密标志
	//
	char plszEncryptMark[] = "{27689ED3-A22F-4E7B-8ED6-6517104FCA45}";   // 38 Bytes
	unsigned int nWrite = fwrite(plszEncryptMark, 1, 38, m_fileStream);
	if(nWrite != 38)
	{
		return false;
	}

	//写入版本号和时间
	//
	unsigned long dwVersion = 0x00000002;
	nWrite = fwrite(&dwVersion, 1, sizeof(unsigned long), m_fileStream);
	if(nWrite != sizeof(unsigned long))
	{
		return false;
	}

#ifdef WIN32
	//写入时间
	//
	SYSTEMTIME SystemTime;
	FILETIME fileTime;
	GetSystemTime(&SystemTime);
	SystemTimeToFileTime(&SystemTime,&fileTime);
	nWrite = fwrite(&fileTime, 1, sizeof(FILETIME), m_fileStream);
	if(nWrite != sizeof(FILETIME))
	{
		return false;
	}
#endif
	//写入加密文件长度
	//
	unsigned long dwLength = 0x00000000;
	nWrite = fwrite(&dwLength, 1, sizeof(unsigned long), m_fileStream);
	if(nWrite != sizeof(unsigned long))
	{
		return false;
	}

	//写入保留长度10 bytes
	//
	char szReserved[10] = "Hi!"; 
	nWrite = fwrite(szReserved, 1, 10, m_fileStream);
	if(nWrite != 10)
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////
// 初始化加密机
//
// initVector: initialization vector, you will usually use 0 here
//
bool CEncryptFile::InitEncryptor(unsigned char * initVector /* = 0 */)
{
	//密钥
	UINT8 key[32]={//0x18,0x29,0x3A,0x47,0x57,0x67,0x77,0x87,
						0x12,0x22,0x32,0x42,0x5A,0x62,0x72,0x82,
						//0x13,0xF2,0x33,0x43,0x5B,0x62,0x73,0x83,
						0xFF,0x24,0x3A,0x44,0x53,0x62,0xC4,0x84 };
	int nRet = ((Rijndael*)m_pEncryptor)->init(Rijndael::CBC, Rijndael::Encrypt	, key, Rijndael::Key16Bytes, initVector);
	if(nRet == RIJNDAEL_SUCCESS)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////
// Encrypt and Write data to file
//
// Parameters:
//		buffer
//			Pointer to data to be written
//		nCount
//			size in bytes to be written
//		bWriteAll
//			If write suspened data to file 
// Return:
//		true if successful; otherwise false. 
bool CEncryptFile::EncryptWrite(const void *buffer, unsigned int nCount, bool bWriteAll)
{
	const unsigned char *pPosAdd = (const unsigned char *)buffer;
	int nAddCount;
	unsigned char BufferIn[BASE_BLOCK_SIZE_ENCRYPT];
	unsigned char BufferOut[BASE_BLOCK_SIZE_ENCRYPT];
	int nEncryptCount;
	int nTotal = m_nSuspendedCount + nCount;
	while(nTotal >= (BASE_BLOCK_SIZE_ENCRYPT -1))	//需要写
	{
		if(m_nSuspendedCount > 0)
		{
			memcpy(BufferIn, m_lpszWriteDataSuspended, m_nSuspendedCount);
		}
		nAddCount = BASE_BLOCK_SIZE_ENCRYPT - 1 - m_nSuspendedCount;
		memcpy(&BufferIn[m_nSuspendedCount], pPosAdd, nAddCount);
		m_nSuspendedCount = 0;
		pPosAdd += nAddCount;
		nTotal -= (BASE_BLOCK_SIZE_ENCRYPT - 1);

		//块的最后一字节为块中的有效字节个数
		BufferIn[BASE_BLOCK_SIZE_ENCRYPT-1] = BASE_BLOCK_SIZE_ENCRYPT-1;
		nEncryptCount = ((Rijndael*)m_pEncryptor)->blockEncrypt(BufferIn
								, BASE_BLOCK_SIZE_ENCRYPT * 8, BufferOut);
        if (nEncryptCount > 0)
        {
            nEncryptCount /= 8;
        }
        if (nEncryptCount > 0)
        {
            if(nEncryptCount != fwrite(BufferOut, 1, nEncryptCount, m_fileStream))
			{
				return false;
			}
        }
	}

	//if(bWriteAll)fflush(m_fileStream);//by for debug

	if(nTotal > 0 
		&& nTotal > m_nSuspendedCount)
	{
		memcpy(&m_lpszWriteDataSuspended[m_nSuspendedCount],
					pPosAdd, nTotal - m_nSuspendedCount);
	}
	m_nSuspendedCount = nTotal;

	if(!bWriteAll)
	{
		return true;
	}

	//将剩余的数据写入文件
	//
	if(m_nSuspendedCount < 1)
	{
		return true;
	}
	memcpy(BufferIn, m_lpszWriteDataSuspended, m_nSuspendedCount);
	//块的最后一字节为块中的有效字节个数
	BufferIn[BASE_BLOCK_SIZE_ENCRYPT-1] = m_nSuspendedCount;
	nEncryptCount = ((Rijndael*)m_pEncryptor)->blockEncrypt(BufferIn
							, BASE_BLOCK_SIZE_ENCRYPT * 8, BufferOut);
    if (nEncryptCount > 0)
    {
        nEncryptCount /= 8;
    }
    if (nEncryptCount > 0)
    {
		if(nEncryptCount != fwrite(BufferOut, 1, nEncryptCount, m_fileStream))
		{
			return false;
		}
    }

	//if(bWriteAll)fflush(m_fileStream);//by for debug

	m_nSuspendedCount = 0;
	return true;
}
