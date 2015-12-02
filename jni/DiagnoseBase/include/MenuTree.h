/***************************************************************************//**
\file		MenuTree.h
\brief		declaration of the CMenuTree class.
\author		1st version:	Harper Jia
			modified by:	James Zhang		
			2011-5-30 17:13:50		merge CommonMenuTree with Menu Tree
\warning
*******************************************************************************/
#ifndef __MENUTREE_H_
#define __MENUTREE_H_

#pragma warning(disable:4786)
#include <stack>
using namespace std;
#include "adsStd.h"
#include "Binary.h"
#include "MenuShow.h"


class CMenuTree
{
protected:
	class CMenuLevel
	{
	public:
		CBinary idMenu;
		CMenuShow::CMenuStruct structMenu;
	};
	
	stack <CMenuLevel> stackMenuLevel;
#ifndef _TASKIDTYPE
	short (*m_pfnTask) (short iTaskId, CBinary idSelectedText);
#else
	//add by scf for 4字节任务ID2007.8.2
	unsigned int (*m_pfnTask) (unsigned int iTaskId, CBinary idSelectedText);
#endif

public:
	CMenuTree(void);
	CMenuTree(string strMenuDbFileName);
	
	~CMenuTree(void);

public:
	// 显示菜单树
	int ShowMenu (CBinary binIDMenu,unsigned char byFlag=0);

#ifndef _TASKIDTYPE
	// 设置回调函数
	void* SetTaskCallBackFunction(short (*pfnTask) (short iTaskId, CBinary idSelectedText));
#else
		//add by scf for 4字节任务ID2007.8.2
	void* SetTaskCallBackFunction(unsigned int (*pfnTask) (unsigned int iTaskId, CBinary idSelectedText));
#endif
	
protected:
	string m_strFileMenuDb;

private:
	// original version of 显示菜单树
	int Org_ShowMenu (CBinary binIDMenu,unsigned char byFlag);

	bool IsTaskIdOk(unsigned int unTaskId);
};

#endif //_MENUTREE_H_