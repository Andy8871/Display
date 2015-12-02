#include "MenuShow.h"
#include "Database.h"
#include "assert.h"
#include "Display.h"
#include "RunLog.h"
#include "CommWithEcu.h"
#include "DataCommon.h"
#include "RunEnvironment.h"
//根据ID从文本库返回字符串
extern string adsGetTextString (CBinary binTextID);

CDatabase m_MenuTxtDb;		//FOR MENU DISPLAY


#if 1// _WIN32_WCE
	const wchar_t* MENU_FLAG = L"MENU_CHOOSED";
#else
	const char* MENU_FLAG= "MENU_CHOOSED";
#endif


CMenuShow::CMenuShow()
{

}

CMenuShow::~CMenuShow()
{
	m_MenuTxtDb.Close();//by added FOR MENU DISPLAY OPTIMIZE 2012-5-18
}


/***************************************************************************************
功    能：初始化菜单
参数说明：char *pTitle 菜单标题，如为空显示程序将使用缺省标题
返 回 值：无
说    明：按照与显示数包格式对缓冲区格式化
**************************************************************************************/
void CMenuShow::Init(const char *pTitle)
{
	assert(NULL != pTitle);

	char chFuction = DT_MENU;
	m_pSendAndReceive->Init(chFuction);  // 填充功能代码和调试标志

	// 菜单缺省选项
	short iDefault = 0; 
	m_pSendAndReceive->Add(iDefault);

	// 当前屏第一行菜单序号
	short i16MenuScreenFirstLineItem = 0; 
	m_pSendAndReceive->Add(i16MenuScreenFirstLineItem);

	// 菜单数量
	short iMenuNum = 0;
	m_pSendAndReceive->Add(iMenuNum);

	//添加Java字符串解析分割标识
	char cDelimiters = 0;
	m_pSendAndReceive->Add(cDelimiters);
	// 菜单标题
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);


}

/***************************************************************************************
功    能：初始化菜单
参数说明：string strTitle 标题文本字符串
返 回 值：无
说    明：无
**************************************************************************************/
void CMenuShow::Init(string strTitle)
{
	// 调用重载函数
	Init(strTitle.c_str());
}

/***************************************************************************************
功    能：初始化菜单
参数说明：CBrinary idTitle 标题文本ID
返 回 值：无
说    明：无
**************************************************************************************/
void CMenuShow::Init(CBinary idTitle)
{
	// 根据ID转换成字符串
	string strTitle = adsGetTextString(idTitle);

	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// 调用重载函数
	Init(strTitle);
}

/***************************************************************************************
功    能：设置标志位
参数说明：byFlag 一个字节
返 回 值：无
说    明：无
**************************************************************************************/

bool CMenuShow::SetFlag (unsigned char byFlag)
{
	m_pSendAndReceive->SetFlag(byFlag);
	return true;
}

/***************************************************************************************
功    能：添加菜单内容项
参数说明：string strMenuItem 菜单内容文本字符串
返 回 值：成功-false 失败-true 
说    明：无
**************************************************************************************/
bool CMenuShow::Add(string strMenuItem)
{
	// 添加菜单文本
	m_pSendAndReceive->Add(strMenuItem);

	// 递增菜单数量
	m_pSendAndReceive->IncreaseNum(8);  // 参数8表示菜单数量是从第8个字节开始
	
	return true;
}

/***************************************************************************************
功    能：添加菜单内容项
参数说明：CBrinary idMenuItem 菜单内容文本ID
返 回 值：成功-false 失败-true
说    明：无
**************************************************************************************/
bool CMenuShow::Add(CBinary idMenuItem)
{
	// 根据ID转换成字符串
	string strMenuItem = adsGetTextString(idMenuItem);

	if(strMenuItem.size()<=0){
		strMenuItem = CDisplay::IdToString(idMenuItem);
	}
		
	// 调用重载函数
	Add(strMenuItem);

	return true;
}

/***************************************************************************************
功    能：显示菜单
参数说明：CMenuStruct &MenuParameter 菜单选择相关信息
返 回 值：被选中的菜单序号，如选《BACK》键则返回的菜单序号为-1
说    明：1、在用户选《BACK》时，MenuParameter.i16MenuSelected的值是菜单加亮条的序号，而函数返回值为-1；
             所以诊断程序员只能使用函数返回值来判断菜单回退。
          2、参数MenuParameter仅仅用户存放菜单正确显示相关的数据，诊断程序员只需声明该变量并加入些参数即可
**************************************************************************************/
short CMenuShow::Show(CMenuStruct &MenuParameter)
{	
	// 获取收到的数据
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);
DO_MENU:

//end add
	// 修改  〈菜单缺省选项〉+〈当前屏第一行菜单序号〉
	m_pSendAndReceive->Modify(MenuParameter.m_i16MenuSelected, 4);
	m_pSendAndReceive->Modify(MenuParameter.m_i16MenuScreenFirstLineItem, 6);
	// 发送消息
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	int lenOfFlag = 0;

#if 1//_WIN32_WCE
	lenOfFlag = wcslen(MENU_FLAG);
	wstring nameFlag = L"";
	if (wmemcmp(MENU_FLAG,(wchar_t*)(pchContain+8),lenOfFlag) == 0)//by modified for Bug V30_PCMAX_OE-3863  2012-5-9	
	{
		nameFlag = (wchar_t*)(pchContain+8+lenOfFlag*2);//"MENU_CHOOSED"的宽字符，占24个单字节   by2012-5-31
		string strbuf = "";
		//字符转换
		//Wstr2Str(nameFlag,strbuf);
		CRunEnvironment::SetMenuChoosedDir(strbuf);
	}

#else	
	lenOfFlag = strlen(MENU_FLAG);
	string nameFlag = "";	
	if (memcmp(MENU_FLAG,pchContain+8,lenOfFlag) == 0)//by modified for Bug V30_PCMAX_OE-3863  2012-5-9
	{
		nameFlag = pchContain+8+lenOfFlag;
		CRunEnvironment::SetMenuChoosedDir(nameFlag);
	}	
#endif

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分

	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	/*if(iLenth < 6)
	{
		assert(iLenth < 6);
	}	*/

	// 第5, 6个字节表示:菜单用户选项
	unsigned char chHigh = pchContain[4];    // 菜单用户选项的高字节部分
    unsigned char chLow  = pchContain[5];    // 菜单用户选项的低字节部分
	int i16MenuSelected;
	if(chHigh == 0xff && chLow == 0xff)
	{
		i16MenuSelected = -1;
	}
	else
	{
		i16MenuSelected = 256*chHigh + chLow;
	}

	// 第7, 8个字节表示:当前屏第一行菜单序号
	iHigh = pchContain[6];    // 高字节部分
    iLow  = pchContain[7];    // 低字节部分
	short i16MenuScreenFirstLineItem = 256*iHigh + iLow;

	MenuParameter.m_i16MenuSelected            = i16MenuSelected;
	MenuParameter.m_i16MenuScreenFirstLineItem = i16MenuScreenFirstLineItem;

	return i16MenuSelected;
}


