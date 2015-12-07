/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：	VehicleInfo.cpp
	文档标识：	007汽车诊断平台详细设计说明书
	摘    要：	显示类的成员变量，用于完成车辆ECU版本信息显示功能
	功    能：
				1、	将车辆ECU版本信息相关数据按规定格式写入缓冲区，通知显示程序取走数据并显示。
				2、	在用户接《OK》后，显示程序才回送结果。

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			创建此类。                                           

**************************************************************************************/


#include "VehicleInfo.h"
#include "Database.h"
#include "assert.h"
#include "LogCat.h"

//根据ID从文本库返回字符串
extern string adsGetTextString (CBinary binTextID);


/***************************************************************************************
功    能：初始化版本信息选择框
参数说明：char* pTitle 版本信息选择框标题，如为空显示程序使用缺省标题
返 回 值：无
说    明：无
**************************************************************************************/
void CVehicleInfo::Init (const char* pTitle)
{
	assert(NULL != pTitle);

	char chFuction = DT_VEHICLE_INFORMATION;
	m_pSendAndReceive->Init(chFuction);  // 填充功能代码和调试标志
	
	// 汽车信息内容数量
	short iContainNum = 0;
	m_pSendAndReceive->Add(iContainNum);

	// 故障码标题
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
}

/***************************************************************************************
功    能：初始化版本信息选择框
参数说明：string strTitle 版本信息选择框标题
返 回 值：无
说    明：无
**************************************************************************************/
void CVehicleInfo::Init (string strTitle)
{
	// 调用重载函数
	Init(strTitle.c_str());
}

bool CVehicleInfo::SetFlag(unsigned char byFlag)
{
	m_pSendAndReceive->SetFlag(byFlag);
	return true;
}

/***************************************************************************************
功    能：初始化版本信息选择框
参数说明：CBinary idTitle 版本信息选择框标题
返 回 值：无
说    明：无
**************************************************************************************/
void CVehicleInfo::Init (CBinary idTitle)
{
	// 根据ID转换成字符串
	string strTitle = adsGetTextString(idTitle);

	// 调用重载函数
	Init(strTitle);
}

/***************************************************************************************
功    能：版本信息内容项
参数说明：string strExplain 版本信息项目提示文字ID
		  string strContain 版本信息项目内容文字
返 回 值：错误代码，成功为0
说    明：无
**************************************************************************************/
short CVehicleInfo::Add (string strExplain, string strContain)
{
	// 提示文本串
	m_pSendAndReceive->Add(strExplain);

	// 内容文本串
	m_pSendAndReceive->Add(strContain);

	// 递增汽车内容数量
	m_pSendAndReceive->IncreaseNum(4);

	return true;
}

/***************************************************************************************
功    能：版本信息内容项
参数说明：CBinary idExplain版本信息项目提示文字ID
string strContain 版本信息项目内容文字
返 回 值：错误代码，成功为0
说    明：无
**************************************************************************************/
short CVehicleInfo::Add (CBinary idExplain, string strContain)
{
	// 根据ID转换成字符串
	string strExplain = adsGetTextString(idExplain);

	// 调用重载函数
	return Add (strExplain, strContain);	
}

/***************************************************************************************
功    能：版本信息并取得用户按键
参数说明：无
返 回 值：用户按键序号
说    明：在用户按《OK》之后对回送用户按键序号
**************************************************************************************/
short CVehicleInfo::Show ()
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

	// 第5个字节表示:菜单用户选项
	short iUserKey = pchContain[4];

 	return iUserKey;
}

