#ifndef __TROUBLECODESHOW_H_
#define __TROUBLECODESHOW_H_

#include <string>
using namespace std;
#include "adsStd.h"
#include "Binary.h"
#include "BaseDisplay.h"


class CTroubleCodeShow : public CBaseDisplay
{
public:
	CTroubleCodeShow();

	enum 
	{
		DT_TROUBLE_CODE         = 10
    };

	void Init(const char *pTitle = NULL,unsigned int bMode=0);
	void Init(string strTitle,unsigned int bMode=0);
	void Init(CBinary idTitle,unsigned int bMode=0);

	void *SetTroubleCodeCallBackFunction(string (*pfnCallBack) (CBinary idStroubleCode));  // 带函数指针参数的函数

	bool Add(CBinary idTroubleCode, string strTroubleStatus);
	bool Add(CBinary idTroubleCode, CBinary idTroubleStatus);

	//hpy added 20131203
	bool Add (string strTroubleCode, string strTroubleDesc, string strTroubleStatus);

	//此接口暂时只对数据驱动模式的诊断程序使用
	bool Add (CBinary idTroubleCode, CBinary idDesc,string strTroubleStatus,string strSAEFromConv = "");
// 	bool Add (CBinary idTroubleCode, CBinary idDesc,string strTroubleStatus);

	void Show();
	short Show (short &iSelNum);
	bool SetFlag(unsigned char iFlag);
	string DefaultStroubleCodeCallBack(CBinary idTroubleCode);

	string GetSelectedItemText(short iCol);
	void SetDRVFileName(string name){m_DRV_FileName ="";if (name.length() > 0){m_DRV_FileName = name;}};
protected:
	string (*m_pfnGetTroubleCodeString) (CBinary idStroubleCode);  // 函数指针
private:
	string TanslateToPCBU(char chHigh, char chLow);
	string m_DRV_FileName;
	bool   m_dataDrvMode;

};

#endif // __TROUBLECODESHOW_H_