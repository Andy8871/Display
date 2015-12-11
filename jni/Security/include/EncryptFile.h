// EncryptFile.h: interface for the CEncryptFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCRYPTFILE_H__0A36B84C_9B9A_4C75_B333_6709224D6D28__INCLUDED_)
#define AFX_ENCRYPTFILE_H__0A36B84C_9B9A_4C75_B333_6709224D6D28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stdio.h>
#include "adsStd.h"

class CEncryptFile  
{
public:
	CEncryptFile();

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
	bool Write(const void *buffer, unsigned int nCount);
	//bool Write(const void *buffer, unsigned int nCount, bool bWriteNow = false);//by added for debug

	//////////////////////////////////////////////////////////////////////////////////
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
	bool Open( const char *filename, const char *mode );

	//////////////////////////////////////////////////////////////////////////////////
	// Closes the resource associated with this object, and makes the FILE 
	// unavailable for reading or writing.
	// If you  have not closed the object before destroying the object, the destructor 
	// closes it for you. 
	//
	void Close();

	//////////////////////////////////////////////////////////////////////////////////
	// Destructor 
	// If you  have not closed the object before destroying the object, the destructor 
	// closes it for you. 
	//
	virtual ~CEncryptFile();

private:
	// The FILE structure pointer which attached with this object.
	FILE *m_fileStream;
	// Write the file header Encrypt info.
	bool WriteEncryptHeadInfo();
	// content wait to write
	unsigned char *m_lpszWriteDataSuspended;
	// content size in bytes wait to write
	int m_nSuspendedCount;
	// Encryptor
	void *m_pEncryptor;
	//////////////////////////////////////////////////////////////////
	// init Encryptor
	// initVector: initialization vector
	bool InitEncryptor(unsigned char * initVector = 0);
	// Encrypt and Write data to file
	bool EncryptWrite(const void *buffer, unsigned int nCount, bool bWriteAll=false);
};

#endif // !defined(AFX_ENCRYPTFILE_H__0A36B84C_9B9A_4C75_B333_6709224D6D28__INCLUDED_)
