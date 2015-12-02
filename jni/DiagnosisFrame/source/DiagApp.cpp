#include "adsStd.h"
#include "DiagApp.h"
#include <time.h>

CDiagApp *CDiagApp::m_pMainApplication;

void adsSleep(int iMilliseconds)
{
	clock_t goal;
	    goal = iMilliseconds+clock();
	    while (goal>clock())
	    {
	        ;
	    }
}
CDiagApp::CDiagApp(void) 
{
	m_pMainApplication = this;
}


CDiagApp::~CDiagApp(void) 
{
}

short CDiagApp::InitInstance(void) 
{

//以下是示例
#if 0
#if USMENUTREE	//使用菜单树
	CMenuTree MenuTree;

	//设置菜单回调函数，如不设此项缺省调用Task()函数
	MenuTree.SetTaskCallBackFunction(菜单回调函数指针); 

	MenuTree.ShowMenu(显示菜单根ID，缺省为 = 0);
#else
	CDiagnosisUnit *pSystemUnit = NULL;

	/*
	此处放诊断程序员代码，结构如下：
	*/

	//1.菜单选择；  //可以是系统扫描之后支持的系统、固定的菜单或其它方式确定下来的。
	//

	//2.确定工作方式； 
	//其定义在CDiagnosisUnit中，可以是进入诊断、扫描系统、快速测试是否存在故障码、
	//快速删除所有故障码等等
	unsigned short ui16Way = CDiagnosisUnit::DUF_QUICK_READ_TROUBLE_CODE; 

	short iRet = 0;
	switch(用户选项) {
	case ENG:
		pSystemUnit = new CENG;
		break;
	case ABS:
		pSystemUnit = new CABS;
		break;
	case SRS:
		pSystemUnit = new CSRS;
		break;
	case 其它系统代码等
		…
		break;
	default:
		#ifdef ADS_DEBUG
			//通知诊断程序出错，通知方式为写入标准错误
		#endif
		break;
	}

	try 
	{
		if(pSystemUnit) 
		{
			iRet = pSystemUnit->Enter(ui16Way);
			delete pSystemUnit;
			pSystemUnit = NULL;
		}
	}
	catch (int adsError) 
	{
		//出错处理，包括内存释放等
	}
#endif
#endif
	return 0;
}

CDiagApp *adsGetDiagApp () 
{
	return CDiagApp::m_pMainApplication;
}





