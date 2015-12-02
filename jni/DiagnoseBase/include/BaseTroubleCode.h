#ifndef __BASETROUBLECODE_H_
#define __BASETROUBLECODE_H_

#include "adsStd.h"


class CBaseTroubleCode
{
public:
	CBaseTroubleCode(void);
	virtual ~CBaseTroubleCode(void);

public:
	// �������ϴ��벢��ʾ
	virtual short ReadTroubleCode (void) = 0;

	// �����Ƿ���ڹ�����
	virtual short ScanTroubleCode (void) = 0;

	// ������ϴ��벢��ʾ��ɽ�� 
	virtual short ClearTroubleCode (void) = 0;
};

#endif //__BASETROUBLECODE_H_