/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：	ShortTestShow.cpp
	文档标识：	007汽车诊断平台详细设计说明书
	摘    要：	显示类的成员变量，用于完成快速测试显示功能
	功    能：
				1、	将快速测试数据按规定格式写入缓冲区，通知显示程序取走数据并显示。
				2、	回送显示程序存贮的用户按键。

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			创建此类。                                           

**************************************************************************************/

#include "ShortTestShow.h"
#include "display.h"
#include "assert.h"

/***************************************************************************************
功    能：初始化快速测试
参数说明：char* pTitle 快速测试标题，如为空显示程序使用缺省标题
返 回 值：无
说    明：无
**************************************************************************************/
void CShortTestShow::Init (const char* pTitle,short Hide_Btn_Stop)
{
	assert(NULL != pTitle);

	char chFuction = DT_SHORT_TEST;
	m_pSendAndReceive->Init(chFuction);  // 填充功能代码和调试标志

	// 测试项数量
	short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	// 快速测试标题
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
#ifdef DATA_DRV_EXT
	short dataDrvMode = Hide_Btn_Stop;
	m_pSendAndReceive->Add(dataDrvMode);
#endif

}

/***************************************************************************************
功    能：初始化快速测试
参数说明：string strTitle 快速测试标题，如果是空串则使用缺少标题
返 回 值：无
说    明：无
**************************************************************************************/
void CShortTestShow::Init (string strTitle)
{
	// 调用重载函数
	Init(strTitle.c_str());
}

/***************************************************************************************
功    能：初始化快速测试
参数说明：CBinary idTitle快速测试标题ID，如果ID长度为0则使用缺省标题
返 回 值：无
说    明：无
**************************************************************************************/
void CShortTestShow::Init (CBinary idTitle)
{
	// 根据ID转换成字符串
	string strTitle = adsGetTextString(idTitle);

	// 调用重载函数
	Init(strTitle);
}



bool CShortTestShow::SetFlag(unsigned char iFlag)
{
	m_pSendAndReceive->SetFlag(iFlag);
	return true;
}



/***************************************************************************************
功    能：添加快速测试项内容项
参数说明：char* pContain 快速测试项内容
返 回 值：错误代码，成功为0
说    明：添加项目内容后，其数值项自动添为空串
**************************************************************************************/
short CShortTestShow::AddItem (const char* pContain)
{
	assert(NULL != pContain);

	// 测试项提示
	m_pSendAndReceive->Add(pContain, strlen(pContain)+1);

	// 递增测试项数量
	m_pSendAndReceive->IncreaseNum(4);

	return true;
}

/***************************************************************************************
功    能：添加快速测试项内容项
参数说明：string strContain 快速测试项内容
返 回 值：错误代码，成功为0
说    明：添加项目内容后，其数值项自动添为空串
**************************************************************************************/
short CShortTestShow::AddItem (string strContain)
{
	// 调用重载函数
	return AddItem(strContain.c_str());
}

/***************************************************************************************
功    能：添加快速测试项内容项
参数说明：CBinary idContain 快速测试项内容ID
返 回 值：错误代码，成功为0
说    明：添加项目内容后，其数值项自动添为空串
**************************************************************************************/
short CShortTestShow::AddItem (CBinary idContain)
{
	// 根据ID转换成字符串
	string strContain = adsGetTextString(idContain);
	
	// 调用重载函数
	return AddItem(strContain);
}

/***************************************************************************************
功    能：添加或修改快速测试项数值
参数说明：CBirnay idValue 快速测试项数值ID
返 回 值：错误代码，成功为0
说    明：无
**************************************************************************************/
short CShortTestShow::ChangeValue (CBinary idValue)
{
	// 根据ID转换成字符串
	string strValue = adsGetTextString(idValue);
	
	// 调用重载函数
	return ChangeValue(strValue);
}

/***************************************************************************************
功    能：添加或修改快速测试项数值
参数说明：string strValue 快速测试项数值
返 回 值：错误代码，成功为0
说    明：无
**************************************************************************************/
short CShortTestShow::ChangeValue (string strValue)
{
	// 测试项数值
	m_pSendAndReceive->Add(strValue);

	return true;
}

/***************************************************************************************
功    能：添加或修改快速测试项数值
参数说明：char* pValue 快速测试项数值
返 回 值：错误代码，成功为0
说    明：无
**************************************************************************************/
short CShortTestShow::ChangeValue (const char* pValue)
{
	assert(NULL != pValue);

	// 测试项数值
	m_pSendAndReceive->Add(pValue, strlen(pValue)+1);

	return true;
}

/***************************************************************************************
功    能：显示快速测试并回送用户按键
参数说明：无
返 回 值：用户按键键值
说    明：显示程序取走数据后立即回送用户按键键值，
**************************************************************************************/
#if 0
short CShortTestShow::Show ()
{
	// 发送消息
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分

	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// 菜单用户选项
	short iUserKey = pchContain[iLenth-1];

 	return iUserKey;
}
#else

short CShortTestShow::Show ()
{
	// 发送消息
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分

	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	if(iLenth < 5)
	{
		assert(iLenth < 5);
	}	

	// 菜单用户选项
	short iUserKey = pchContain[4];

	if(adsIDSHORTTESTENTER == iUserKey)
	{
		m_nEnterIndex=(unsigned char)pchContain[5]*256 + (unsigned char)pchContain[6];
	}

 	return iUserKey;
}

#endif

short CShortTestShow::GetEnterIndex(void)
{
	return m_nEnterIndex;
}
