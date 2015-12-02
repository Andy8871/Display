#ifndef _CRCCLASS_H_
#define _CRCCLASS_H_


class CCrcClass
{
public:
	CCrcClass();
	virtual ~CCrcClass();

	void Init(void);
	void Free(void);


	unsigned long FileCrc(const char* szFilename,long lReverseOffset , unsigned long &dwCrc32) const;
	unsigned long BufferCrc(const char* pBuffer,long nBufferLen , unsigned long &dwCrc32) const;

protected:
	inline void CalcCrc32(const unsigned char byte, unsigned long &dwCrc32) const;
	unsigned long *m_pdwCrc32Table;
};

#endif
