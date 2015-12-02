/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：	BaseDisplay.cpp
	文档标识：	007汽车诊断平台详细设计说明书
	摘    要：	显示基类

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			创建此入口。                                           

**************************************************************************************/

#include "BaseDisplay.h"
#include "assert.h"

CBaseDisplay::CBaseDisplay() 
{ 
	m_pSendAndReceive = NULL; 	
}
	
CBaseDisplay::~CBaseDisplay()
{
 
}

void CBaseDisplay::SetCommunicationPointer(CCommunicationWithDisplay* pSendAndReceive)
{
	assert(NULL != pSendAndReceive);
	m_pSendAndReceive = pSendAndReceive;
}



