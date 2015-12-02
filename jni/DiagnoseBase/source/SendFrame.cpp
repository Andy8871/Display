/******************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：SendFrame.cpp
	文档标识：007汽车诊断平台详细设计说明书(诊断分册)
	摘    要：汽车通信命令类。

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2005.01.12  zhangsh   1.0			创建此类。                                           

******************************************************************************/

#include "SendFrame.h"
#include "Database.h"
#include "assert.h"
//#include "EcuCommException.h"



#define	NOCMDINLIB		-1
#define	CMDLIBERROR		-2

// 静态成员变量声明
int CSendFrame::m_iDefaultReceiveFrameNumber = 1;


CSendFrame::CSendFrame(void)
{
}

CSendFrame::~CSendFrame(void)
{
}


/******************************************************************
功    能：	赋值
参数说明：	const CSendFrame& SendFrame 发送帧
返 回 值：	无
说    明：	无
******************************************************************/
void CSendFrame::operator = (CSendFrame SendFrame)
{
	m_SendFrame.clear();

	if(SendFrame.m_SendFrame.empty())
	{
		return;
	}

	vector<COneSendFrame>::iterator vIter;
	for(vIter = SendFrame.m_SendFrame.begin(); vIter != SendFrame.m_SendFrame.end(); vIter++)
	{
		m_SendFrame.push_back(*vIter);
	}
}


/******************************************************************
功    能：	赋值
参数说明：	const vector<CBinary>& binCmd 从命令库读出的命令，
返 回 值：	无
说    明：	参数中偶数位置为命令，奇数位置为接收帧数量
******************************************************************/
void CSendFrame::operator = (vector<CBinary>& binCmd)
{
	m_SendFrame.clear();

	if (binCmd.empty())
	{
		return;
	}

	COneSendFrame oneSendFrame;
	if (1 == binCmd.size())	 // 只有一条命令且回复帧数缺省
	{
		oneSendFrame.m_binSendCmd = binCmd[0];
		oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

		m_SendFrame.push_back(oneSendFrame);	
	}
	else
	{
		vector<CBinary>::size_type vIter;
		for (vIter = 0; vIter < binCmd.size(); vIter += 2)
		{
			oneSendFrame.m_binSendCmd = binCmd[vIter];
			oneSendFrame.m_iReceiveFrameNumber = (unsigned char)(binCmd[vIter+1])[0];

			m_SendFrame.push_back(oneSendFrame);
		}		
	}
}


/******************************************************************
功    能：	加入新发送帧数据
参数说明：	const vector<CBinary>& binCmd 从命令库读出的命令
返 回 值：	无
说    明：	参数中偶数位置为命令，奇数位置为接收帧数量
******************************************************************/
void CSendFrame::operator += (vector<CBinary>& binCmd)
{
	if (binCmd.empty())
	{
		return;
	}

	COneSendFrame oneSendFrame;
	if(1 == binCmd.size())	//只有一条命令且回复帧数缺省
	{
		oneSendFrame.m_binSendCmd = binCmd[0];
		oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

		m_SendFrame.push_back(oneSendFrame);	
	}
	else
	{
		vector<CBinary>::size_type vIter;
		for (vIter = 0; vIter < binCmd.size(); vIter += 2)
		{
			oneSendFrame.m_binSendCmd = binCmd[vIter];
			oneSendFrame.m_iReceiveFrameNumber = (unsigned char)(binCmd[vIter+1])[0];

			m_SendFrame.push_back(oneSendFrame);
		}
	}
}

/******************************************************************
功    能：	赋值
参数说明：	const vector<CBinary>& binCmd 从命令库读出的命令，
返 回 值：	无
说    明：	参数中偶数位置为命令，奇数位置为接收帧数量
******************************************************************/
void CSendFrame::operator = (const CBinary binData)
{
	m_SendFrame.clear();

	COneSendFrame oneSendFrame;

	oneSendFrame.m_binSendCmd = binData;
	oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

	m_SendFrame.push_back(oneSendFrame);	
}


/******************************************************************
功    能：	加入新发送帧数据
参数说明：	const vector<CBinary>& binCmd 从命令库读出的命令
返 回 值：	无
说    明：	参数中偶数位置为命令，奇数位置为接收帧数量
******************************************************************/
void CSendFrame::operator += (CBinary binData)
{
	COneSendFrame oneSendFrame;

	oneSendFrame.m_binSendCmd = binData;
	oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

	m_SendFrame.push_back(oneSendFrame);	
}

/******************************************************************
功    能：	加入新发送帧数据
参数说明：	const vector<CBinary>& binCmd 从命令库读出的命令
返 回 值：	无
说    明：	无
******************************************************************/
void CSendFrame::operator += (CSendFrame& SendFrame)
{
	if(SendFrame.m_SendFrame.empty())
	{
		return;
	}

	vector<COneSendFrame>::iterator vIter;
	for(vIter = SendFrame.m_SendFrame.begin(); vIter != SendFrame.m_SendFrame.end(); vIter++)
	{
		m_SendFrame.push_back(*vIter);
	}
}


/******************************************************************
功    能：	加入新发送帧数据
参数说明：	COneSendFrame& oneSendFrame某一发送命令包
返 回 值：	无
说    明：	无
******************************************************************/
void CSendFrame::operator += (COneSendFrame& oneSendFrame)
{
	if(oneSendFrame.m_binSendCmd.GetSize() < 1)
	{
		return;
	}

	m_SendFrame.push_back(oneSendFrame);
}

/******************************************************************
  功    能：取得一个发送帧
  参数说明：发送帧序号 
  返 回 值：一个发送帧
  说    明：无
******************************************************************/
CSendFrame::COneSendFrame CSendFrame::operator [] (short nIndex)
{
	return m_SendFrame[nIndex];
}


/******************************************************************
功    能：	设置缺省接收帧帧数
参数说明：	short iDefaultReceiveFrame 缺省接收帧帧数，缺省值为1
返 回 值：	前次设置缺省接收帧帧数
说    明：	无
******************************************************************/
short	CSendFrame::SetDefaultReceiveFrameNumber (short iDefaultReceiveFrame)
{
	short preDefaultReceiveFrameNuber = m_iDefaultReceiveFrameNumber;
	m_iDefaultReceiveFrameNumber = iDefaultReceiveFrame;

	return preDefaultReceiveFrameNuber;
}


/******************************************************************
功    能：	加入命令库ID对应的发送帧
参数说明：	CBinary idCmd 命令库ID
返 回 值：	成功否
说    明：	无
******************************************************************/
bool CSendFrame::AddFromCmdLib(CBinary idCmd)
{
	vector<CBinary> vBinCmd;	

	CDatabase dbCmd;
	
	if(!dbCmd.Open(CDatabase::DB_COMMAND))
		return false;

	vBinCmd = dbCmd.SearchId(idCmd);
	dbCmd.Close();

	if (vBinCmd.empty())
	{
		return false;
	}

	COneSendFrame oneSendFrame;
	if(vBinCmd.size() == 1)	//只有一条命令且回复帧数缺省
	{
		oneSendFrame.m_binSendCmd = vBinCmd[0];
		oneSendFrame.m_iReceiveFrameNumber = m_iDefaultReceiveFrameNumber;

		m_SendFrame.push_back(oneSendFrame);	
	}
	else
	{
		// 如同一个ID中包含多个命令时不能省略: ECU回应的数据帧数
		assert( vBinCmd.size()%2 == 0 );

		vector<CBinary>::iterator vIter;
		for (vIter = vBinCmd.begin(); vIter != vBinCmd.end(); vIter += 2)
		{
			oneSendFrame.m_binSendCmd = *vIter;
			oneSendFrame.m_iReceiveFrameNumber = (unsigned char)(vIter+1)->GetAt(0);

			m_SendFrame.push_back(oneSendFrame);
		}
	}

	return true;
}

/******************************************************************
功    能：	追加发送帧
参数说明：	const CSendFrame& SendFrame 发送帧
返 回 值：	加入的数量
说    明：	无
******************************************************************/
short	CSendFrame::AddSendFrame(CSendFrame& SendFrame)
{
	if(SendFrame.m_SendFrame.empty())
	{
		return 0;
	}

	short nAddSize = 0;
	vector<COneSendFrame>::iterator vIter;
	for(vIter = SendFrame.m_SendFrame.begin(); vIter != SendFrame.m_SendFrame.end(); vIter++, nAddSize++)
	{
		m_SendFrame.push_back(*vIter);
	}

	return nAddSize;
}


/******************************************************************
功    能：	清空发送帧
参数说明：	无
返 回 值：	无
说    明：	无
******************************************************************/
void CSendFrame::Clear (void)
{
	m_SendFrame.clear();
}


/******************************************************************
功    能：	取得发送帧数量
参数说明：	无
返 回 值：	发送帧数量
说    明：	无
******************************************************************/
short	CSendFrame::GetFrameNumber (void)
{
	short iFrameNumber =  m_SendFrame.size();
	return iFrameNumber;
}


/******************************************************************
功    能：	得到发送帧的返回帧数
参数说明：	无
返 回 值：	发送帧的返回帧数
说    明：	无
******************************************************************/
short CSendFrame::GetAllReceiveFrameNumber(void)
{
	short recvFrameNum = 0;
	vector<COneSendFrame>::iterator iSendFrame;	
	for(iSendFrame = m_SendFrame.begin(); iSendFrame != m_SendFrame.end(); iSendFrame++)
	{
		recvFrameNum += iSendFrame->m_iReceiveFrameNumber;
	}

	return recvFrameNum;
}


/******************************************************************
功    能：	得到发送帧中单帧的返回帧数
参数说明：	int iOrder发送帧中单帧的顺序号
返 回 值：	发送帧中单帧的返回帧数
说    明：	无
******************************************************************/
short CSendFrame::GetOneReceiveFrameNumber(int iOrder)
{
	return m_SendFrame[iOrder].m_iReceiveFrameNumber;
}


/**************************************************************************************
功    能：	将发送帧命令内容按“[命令长度＋内容+回复长度]...”的格式输出到指定的缓冲区
参数说明：	无
返 回 值：	缓冲区填充长度
说    明：	无
**************************************************************************************/
int CSendFrame::PutCmdDataToBuffer(unsigned char* dataBuf, int bufLength)
{
	if (m_SendFrame.size() == 0)
	{
		return 0;
	}

	int dataLength = 0;
	vector<COneSendFrame>::iterator vIter;

	for (vIter = m_SendFrame.begin(); vIter < m_SendFrame.end(); vIter++)
	{
		CBinary cmdBin = vIter->m_binSendCmd;

		dataBuf[dataLength] = (unsigned char)cmdBin.GetSize();
		dataLength++;

		char* pbuf = (char*)cmdBin.GetBuffer();
		memcpy(dataBuf+dataLength, pbuf, cmdBin.GetSize());
		dataLength += cmdBin.GetSize();

		dataBuf[dataLength] = (unsigned char)vIter->m_iReceiveFrameNumber;
		dataLength++;
	}

	return dataLength;
}