#ifndef __BASETROUBLECODE_H_
#define __BASETROUBLECODE_H_

#include "adsStd.h"


class CBaseTroubleCode
{
public:
	CBaseTroubleCode(void);
	virtual ~CBaseTroubleCode(void);

public:
	// 读出故障代码并显示
	virtual short ReadTroubleCode (void) = 0;

	// 测试是否存在故障码
	virtual short ScanTroubleCode (void) = 0;

	// 清除故障代码并显示完成结果 
	virtual short ClearTroubleCode (void) = 0;
};

#endif //__BASETROUBLECODE_H_