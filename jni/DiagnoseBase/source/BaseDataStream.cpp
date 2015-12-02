/******************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：BaseDataStream.cpp
	文档标识：007汽车诊断平台详细设计说明书
	摘    要：数据流的读取及显示虚基类	

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2006.01.11  zhangsh		1.0			创建此类。                                           

******************************************************************************/
#include "BaseDataStream.h"


CBaseDataStream::CBaseDataStream(void)
{
	m_bShowDataStreamSelectWindow = true;
}

inline CBaseDataStream::~CBaseDataStream(void)
{
}


/******************************************************************************
功    能：	设置是否显示数据流选择项
参数说明：	bool bEnable 是否显示，显示—true 不显示—false 
返 回 值：	无
说    明：	m_ bShowDataStreamSelectWindow ：= bEnable
******************************************************************************/
void CBaseDataStream::EnableShowMultiSelected (bool bEnable = true)
{
	m_bShowDataStreamSelectWindow = bEnable;
}
