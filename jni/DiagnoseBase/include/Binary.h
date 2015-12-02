#ifndef		__BINARY_H__
#define		__BINARY_H__

#include "adsStd.h"
//#include "CommWithEcu.h"


class CBinary
{
protected:
	struct tagBinaryData 
	{
		unsigned char *m_pucBinary;
		short m_iDataLength;
		short m_iAllocLength;
		short m_iDefaultAddAllocLength;
		short m_iCitationCount;
	}*m_pbinData;

	enum
	{
		BN_ALL_DATA           = -1,
		BN_COMMON_ALLOCLENGTH = 6,
		BN_DEFALT_GROW_LENGTH = 100
	};

public:
	// 缺省构造函数
	CBinary(void);

	// 构造函数
	CBinary(const CBinary& binData);

	// 构造函数，分配内存
	CBinary(short iLength); 

	// 构造函数，建立长度为iLength的缓冲区，并将pBuffer的内容复制到缓冲区
	CBinary(const char *pBuffer, short iBufferLength);
	CBinary(unsigned char *pucBuffer, short iBufferLength);
	CBinary(string& pBuffer, short iBufferLength);

	// 析构函数
	~CBinary(void);

public:
	// 复制类
	void operator = (const CBinary binData);

	// 将二进制数据合并
	CBinary& operator += (CBinary& binData);

	// 将二进制数据缓冲区尾部加一个字节数据
	CBinary& operator += (unsigned char ucValue);
//	CBinary& operator += (char cValue);

	// 将二进制数据类合并，返回新类
	CBinary operator + (CBinary& binData);

	// 取指定位置的字符数据
	const unsigned char operator [] (short nIndex)const; 

	// 比较二个二进制串的大小
	bool operator < (CBinary& binData);
	bool operator <= (CBinary& binData);
	bool operator >= (CBinary& binData);
	bool operator > (CBinary& binData);

	// 比较二个二进制串的大小
	bool operator == (const CBinary& binData);

	// 取指定位置的字符数据
	unsigned char GetAt(short nIndex);

	// 改变指定位置的字符数据
	bool SetAt(short nIndex, unsigned char ucNewElement);

	// 将二进制数据缓冲区尾部加一个字节数据
//	bool Add(char cNewElement);
	bool Add(unsigned char ucNewElement);

	// 设定缓冲区分配的尺寸
	bool SetAllocLength(short iAllocLength, short iGrowBy = BN_DEFALT_GROW_LENGTH);

	// 取数据长度
	short GetSize(void);

	// 取数据缓冲区指针
	const unsigned char *GetBuffer(void);//const;

	// 读类中数据到目标缓冲区
	short ReadBuffer(unsigned char *pucSrc, short iStart=0, short iLength = BN_ALL_DATA);
	short ReadBuffer(char *pSrc, short iStart=0, short iLength = BN_ALL_DATA);
	
	// 从指定缓冲区数据写入对象内部
	short WriteBuffer(const unsigned char * pucSrc, short iWriteLength);
	short WriteBuffer(const char* pSrc, short iWriteLength);

    // 将数据追加到类中数据的结尾
//	short Append(char *pBuffer, short iWriteLength);
	short Append(unsigned char *pucBuffer, short iWriteLength);

	// 类数据是否为空
	bool IsEmpty(void);

	// 释放未使用的内存
	void FreeExtra(void);

	//以下用于调试目的
	short GetBufSize(void);
	short GetCitationCount(void);
	short GetDefaultAddAllocLength(void);

private:
	void CopySelf(CBinary binData);
};

#endif		//__BINARY_H_