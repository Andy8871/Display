#ifndef	__RUN_ENVIRONMNET_H__
#define	__RUN_ENVIRONMNET_H__
#include "adsStd.h"
#include <string>
using namespace std;

//hpy add
typedef struct TAG_LANGUAGE_ITEM
{
	char csCountryCode[4];
	char csLanguage[30];
	unsigned int	iCodePage;
}LANGUAGE_ITEM;

class CRunEnvironment
{
	protected:
		static string m_strDisplayDirectory;    // 显示程序工作目录
		static string m_strDiagnosisDirectory;  // 诊断程序工作目录
		static string m_strLanguage;            // 语言代码
		//add by scf 2007.2.13
		static string m_strSerialNumber;         //机器序列号
		static bool      m_bDemoMode;
		static  unsigned char    m_cScreenType;		//屏幕类型
		static string m_strDisplayVersion;
		static string m_ECUIDinfo;
		static bool m_bDsPrintMode;

		static bool m_cHeckMode;    //true：选中记录模式；false:非记录模式
		static string m_menuChoosedDir;
		static unsigned int m_CodePage;
		static bool m_bNewMcuFlag;
		static unsigned char m_ProuctType;

		static unsigned char m_UnitType;//hpy add 20131126 //单位类型

	public:
		// 设置显示程序工作目录
		static void SetDisplayDirectory(const char* pDirectory);

		// 取得显示程序工作目录
		static string GetDisplayDirectory();

		// 设置诊断程序工作目录
		static void SetDiagnosisDirectory(const char* pDirectory);

		// 取得诊断程序工作目录
		static string GetDiagnosisDirectory();

		// 设置语言代码
		static void SetLanguage(const char* pLanguage);

		// 取得语言代码
		static string GetLanguage();

		//add by scf 2007.2.13
		// 设置序列号
		static void SetSerialNumber(const char* pSerailNumber);

		//add by scf 2007.2.13
		// 设置序列号
		static void SetDemoMode(bool bDemoMode=false);


		// 取得序列号
		static string GetSerialNumber();
		
		// 取得演示模式
		static bool GetDemoMode();
		//set the ECUID info
		static void SetECUInfo(string& info);
#ifdef DATA_DRV_EXT
		//设置记录模式
		static void SetCheckMode(bool checkMode = false);
		// 取得记录模式
		static bool GetCheckMode(void);
		//设置menu选择路径
		static void SetMenuChoosedDir(string& dir);
		//获取menu选择路径
		static string GetMenuChoosedDir(void);

		//设置属性页代码
		static void SetCodePage(unsigned int nCodePage);
		// 取得记录模式
		static unsigned int GetCodePage(void);

#endif
		static string GetItemLanguage(string CountryCode);//hpy add
		static unsigned int GetItemCodePage(string CountryCode);//hpy add
		
		//0:黑白屏 1：彩屏 2: PC(1024*768)
		static unsigned char GetScreenType();

		static void SetScreenType(unsigned char uiType);

		// 取得显示程序工作目录
		static string GetDisplayVersion();

		// 设置诊断程序工作目录
		static void SetDisplayVersion(char* pDisplayVersion);
		
		//设置mcu标志信息
		static void SetNewMcuFlag(bool bFlag);
		static bool GetNewMcuFlag();

		//设置mcu标志信息
		static void SetDsPrintMode(bool bFlag);
		static bool GetDsPrintMode();

		//BEN ADD 20131008
		static void SetDBFormat(int iDBFormat);
		static int GetDBFormat();
		static int m_iDBFormat;


		//获取产品类型
		static unsigned char GetProductType();
		static void SetProductType(unsigned char uiProductType);

		//获取单位类型	//hpy add 20131126
		static unsigned char GetUnitType();
		static void SetUnitType(unsigned char uiUnitType);

};
	
#endif	//_RUNENTIRONMNET_H_
