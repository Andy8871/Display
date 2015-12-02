/******************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：ReceiveFrame.cpp
	文档标识：007汽车诊断平台详细设计说明书
	摘    要：本类用于存取汽车ECU返回的数据

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2006.01.03  zhangsh		1.0			创建此类。                                           

******************************************************************************/
#include <assert.h>
#include "receiveframe.h"



CReceiveFrame::CReceiveFrame(void)
{
	m_ReceiveFrame.clear();
}

CReceiveFrame::~CReceiveFrame(void)
{
}

/*****************************************************************
功    能：复制接收帧
参数说明：const CReceiveFrame& ReceiveFrame 接收帧类
返 回 值：无
说    明：无
*****************************************************************/
void CReceiveFrame::operator = (CReceiveFrame ReceiveFrame )
{
	CBinaryGroup groupBin;
	vector< CBinaryGroup >::iterator vIter;

	m_ReceiveFrame.clear();
	for(vIter = ReceiveFrame.m_ReceiveFrame.begin(); vIter != ReceiveFrame.m_ReceiveFrame.end(); vIter++)
	{
		groupBin.assign(vIter->begin(), vIter->end());
		m_ReceiveFrame.push_back(groupBin);
	}
	m_CanParseInfo = ReceiveFrame.GetCanParseInfo();
}


/*****************************************************************
功    能：加入新发送帧数据
参数说明：CBinaryGroup& ReceiveGroup 接收组数据
返 回 值：无
说    明：无
*****************************************************************/
void CReceiveFrame::operator += ( CBinaryGroup& ReceiveGroup )
{
	m_ReceiveFrame.push_back(ReceiveGroup);
}


/*****************************************************************
功    能：取得某一发送帧的应答帧
参数说明：发送帧序号
返 回 值：该发送帧对应的接收帧数组的引用
说    明：无
*****************************************************************/
CBinaryGroup& CReceiveFrame::operator[] (const short iFrameOrder)
{
	assert( (0 <= iFrameOrder) && (iFrameOrder < (short)m_ReceiveFrame.size()) );

	return m_ReceiveFrame[iFrameOrder];
}


/*****************************************************************
功    能：取得发送帧的数量
参数说明：无
返 回 值：发送帧的数量
说    明：无
*****************************************************************/
short CReceiveFrame::GetSendFrameNumber (void)
{
	short iSize = m_ReceiveFrame.size();
	return iSize;
}


///*****************************************************************
//功    能：设置发送帧的数量
//参数说明：unsigned short iSendFrameNumber发送帧的数量
//返 回 值：错误代码
//说    明：无
//*****************************************************************/
//short CReceiveFrame::SetSendFrameNumber (short iSendFrameNumber)
//{
//	short iRet = 0;
//
//	return iRet;
//}
//

/*****************************************************************
功    能：取得第一发送帧的应答帧
参数说明：
返 回 值：
说    明：无
*****************************************************************/
CBinary CReceiveFrame::GetFirstAnswerFrame()
{
	CBinary binReceive;

	if(0 == m_ReceiveFrame.size())
	{
		return binReceive;
	}

	if(0 == m_ReceiveFrame[0].size())
	{
		return binReceive;
	}

	return m_ReceiveFrame[0][0];
}


/*****************************************************************
功    能：取得某一发送帧的应答帧
参数说明：发送帧序号
返 回 值：该发送帧对应的接收帧数组的引用
说    明：无
*****************************************************************/
CBinaryGroup& CReceiveFrame::GetOneFrameAnswer (const short iFrameOrder)
{
	assert( (0<=iFrameOrder) && (iFrameOrder < (short)m_ReceiveFrame.size()) );

	return m_ReceiveFrame[iFrameOrder];
}


/*****************************************************************
功    能：返回接收帧的数组尺寸
参数说明：无
返 回 值：接收帧数组的尺寸
说    明：无
*****************************************************************/
short CReceiveFrame::size()
{
	short iSize = m_ReceiveFrame.size();
	return iSize;
}


/*****************************************************************
功    能：清空接收帧数据
参数说明：无
返 回 值：无
说    明：无
*****************************************************************/
void CReceiveFrame::clear()
{
	m_ReceiveFrame.clear();
}
