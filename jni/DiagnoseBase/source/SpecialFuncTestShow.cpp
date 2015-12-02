/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：	SpecialFuncTestShow.cpp
	文档标识：	007汽车诊断平台详细设计说明书
	摘    要：	显示类的成员变量，用于完成动作测试显示功能
	功    能：
				1、	将数据流数据信息、按钮信息按规定格式写入缓冲区，通知显示程序取走数据并显示。
				2、	回送显示程序存贮的用户按键。

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2007.08.23  fufushang	1.0			创建此类。                                           

**************************************************************************************/
#ifdef WIN32
#include "windows.h"
#endif
#include "arithmetic.h"
#include "SpecialFuncTestShow.h"
#include "Database.h"
#include "assert.h"
#include "display.h"

#define INTERVAL_TIME 50


//根据ID从文本库返回字符串
extern string adsGetTextString (CBinary binTextID);
//extern CArithmetic::SetDebugState (unsigned char uiDebugItem, bool bEnable);


CSpecialFuncTestShow::CSpecialFuncTestShow()
{
	m_iButtonNum = 0;
	
	m_iTitleLenth       = 0;     // 动作测试标题长度
	m_iDataStreamFormat = -1;    // 数据流格式
	m_bAddDataStream    = false; // 是否已经添加数据流内容
}

/***************************************************************************************
功    能：初始化动作测试
参数说明：char* pTitle 动作测试标题，如为空显示程序使用缺省标题
返 回 值：无
说    明：因为未加入动作测试按钮，需要配合AddButton ()函数一起使用
**************************************************************************************/
void CSpecialFuncTestShow::Init (const char* pTitle)
{
	assert(NULL != pTitle);

	// 按钮数量清0
	m_iButtonNum = 0;

	// 数据流格式复位
	m_iDataStreamFormat = -1;

	// 是否已经添加数据流内容
	m_bAddDataStream    = false; 

	m_iTitleLenth = strlen(pTitle)+1;  // 计算标题长度

	char chFuction = DT_SPECIAL_TEST;
	m_pSendAndReceive->Init(chFuction);  // 填充功能代码和调试标志

	// 按钮数量
	short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	// 动作测试标题
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);

}

/***************************************************************************************
功    能：初始化动作测试
参数说明：string strTitle 动作测试标题，
返 回 值：无
说    明：因为未加入动作测试按钮，需要配合AddButton ()函数一起使用
**************************************************************************************/
void CSpecialFuncTestShow::Init (string strTitle)
{	
	// 调用重载函数
	Init(strTitle.c_str());
}

/***************************************************************************************
功    能：增加特殊功能说明
参数说明：string Msg 动作测试标题，
返 回 值：无
说    明：因为未加入动作测试按钮，需要配合AddButton ()函数一起使用
**************************************************************************************/
short CSpecialFuncTestShow::AddPrerequisites(const char* strMsg )
{	
	m_pSendAndReceive->Add(strMsg, strlen(strMsg)+1);
	return true;
}

/***************************************************************************************
功    能：增加特殊功能说明
参数说明：string Msg 动作测试标题，
返 回 值：无
说    明：因为未加入动作测试按钮，需要配合AddButton ()函数一起使用
**************************************************************************************/
short CSpecialFuncTestShow::AddProcessCtrl (short nMin,short nMax,short nSteps)
{
	m_pSendAndReceive->Add(nMin);
	m_pSendAndReceive->Add(nMax);
	m_pSendAndReceive->Add(nSteps);
		return true;
}

/***************************************************************************************
功    能：初始化动作测试
参数说明：CBinary idTitle 动作测试标题，
返 回 值：无
说    明：因为未加入动作测试按钮，需要配合AddButton ()函数一起使用
**************************************************************************************/
void CSpecialFuncTestShow::Init (CBinary idTitle)
{
	// 根据ID转换成字符串
	string strTitle = adsGetTextString(idTitle);

	// 调用重载函数
	Init(strTitle);
}


bool CSpecialFuncTestShow::SetFlag(unsigned char byFlag)
{
	m_pSendAndReceive->SetFlag(byFlag);
	return true;
}


/***************************************************************************************
功    能：添加动做测试界面按钮
参数说明：CBinary idButtonText 添加的按钮标题，
返 回 值：错误代码
说    明：无
**************************************************************************************/
short CSpecialFuncTestShow::AddButton (CBinary idButtonText, char byStatus)
{
	assert(!m_bAddDataStream);

	assert(ID_BINARY_LENTH == idButtonText.GetSize());

	// 按钮文本ID串
	m_pSendAndReceive->Add(idButtonText);

	// 按钮状态
	m_pSendAndReceive->Add(byStatus);

	// 递增按纽数量
	m_pSendAndReceive->IncreaseNum(4);
	
	// 记录按纽数量
	m_iButtonNum++;

	return true;
}

/***************************************************************************************
功    能：添加数据流内容项
参数说明：CBinary idDataStream 数据流ID
		  string strDataStreamValue 数据流数值
返 回 值：错误代码，成功为0
说    明：在第一次添加内容值时写入格式字节，如后面加入的数据格式与第一次不同则不能加入并报错
**************************************************************************************/
short CSpecialFuncTestShow::Add (CBinary idDataStream, string strDataStreamValue)
{
	assert(BDF_VW != m_iDataStreamFormat);

//	if(BDF_VW == m_iDataStreamFormat)
//	{
//		return EC_TYPE_DATA_STREAM;  // 数据流格式不一致
//	}
	
	// 一般数据流
	m_iDataStreamFormat = BDF_NOMAL;

	// 如果是第一次添加数据流内容
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;
		
//		// 计算按钮选择掩码字节数
//		short iByteNum = (m_iButtonNum-1)/8 +1;		
//		char chMask = 0x00;		
//		for(short iIndex = 0; iIndex < iByteNum; iIndex++)
//		{
//			m_pSendAndReceive->Add(chMask);
//		}

		// 数据流格式
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// 数据流数量
		short iNum = 0;
		m_pSendAndReceive->Add(iNum);
	}

	// 数据流ID
	m_pSendAndReceive->Add(idDataStream);

	// 数据流数值串
	m_pSendAndReceive->Add(strDataStreamValue);

	// 递增数据流数量  长度(2字节) + 功能代码(1字节) + 调试标志(1字节) + 按钮数量(2字节) + 标题长度 + 按纽文本ID + 按纽状态 + 数据流数据格式(1字节)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}

/***************************************************************************************
功    能：添加数据流内容项
参数说明：CBinary idDataStream 数据流ID
		  string strDataStreamValue 数据流数值
		  CBinary idUnit 数据流单位ID
返 回 值：错误代码，成功为0
说    明：在第一次添加内容值时写入格式字节，如后面加入的数据格式与第一次不同则不能加入并报错
**************************************************************************************/
short CSpecialFuncTestShow::Add (CBinary idDataStream, string strDataStreamValue, CBinary idUnit)
{
	assert(BDF_NOMAL != m_iDataStreamFormat);

//	if(BDF_NOMAL == m_iDataStreamFormat)
//	{
//		return EC_TYPE_DATA_STREAM;  // 数据流格式不一致
//	}
	
	// 单位独立的数据流
	m_iDataStreamFormat = BDF_VW;
	
	// 如果是第一次添加数据流内容
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;
		
//		// 计算按钮选择掩码字节数
//		short iByteNum = (m_iButtonNum-1)/8 +1;		
//		char chMask = 0x00;		
//		for(short iIndex = 0; iIndex < iByteNum; iIndex++)
//		{
//			m_pSendAndReceive->Add(chMask);
//		}

		// 数据流格式
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// 数据流数量
		short iNum = 0;
		m_pSendAndReceive->Add(iNum);
	}
	

	// 数据流ID
	m_pSendAndReceive->Add(idDataStream);

	// 数据流单位ID
	m_pSendAndReceive->Add(idUnit);

	// 数据流数值串
	m_pSendAndReceive->Add(strDataStreamValue);

	// 递增数据流数量  长度(2字节) + 功能代码(1字节) + 调试标志(1字节) + 按钮数量(2字节) + 标题长度 + 按纽文本ID + 按纽状态 + 数据流数据格式(1字节)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}

/***************************************************************************************
功    能：显示动作测试界面并回送用户按键
参数说明：char *pMarkCode 按钮掩码指针，如为NULL则无选中项
返 回 值：用户按键序号
说    明：显示程序取走数据后立即回送无户按键序号
**************************************************************************************/
short CSpecialFuncTestShow::Show ()
{
	//SYSTEMTIME sysTime;
	//GetSystemTime( &sysTime);

	// 7/18/2006 add by wcx
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;
		// 数据流格式
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// 数据流数量
		short iNum = 0;
		m_pSendAndReceive->Add(iNum);
	}
	// 7/18/2006 add end

	static unsigned long last = 0;

	timeval startTm;
	gettimeofday(&startTm, 0);
	static unsigned long start = startTm.tv_usec;
	//static unsigned long start = GetTickCount(); //1000*sysTime.wSecond + sysTime.wMilliseconds;
	//if(start - last > INTERVAL_TIME)

#if 0
	if(start - last < INTERVAL_TIME)
	{
		last = start;
		return 0xFF; //adsMB_NoButton;
	}
#endif

	// 保存上次的时间
	last = start;

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

	// 第4个字节表示:调试标志
	char chDebugFlag = pchContain[3];
	bool bEnable = false;
	if(chDebugFlag == 0x01)
	{
		bEnable = true;
	}

	unsigned char uiDebugItem = CArithmetic::AD_DataStream;
	CArithmetic::SetDebugState (uiDebugItem, bEnable);

	// 第5个字节表示:用户按键序号
	short iUserKey = pchContain[4];

 	return iUserKey;
}


/***************************************************************************************
功    能：添加数据流内容项
参数说明：string strDataStreamName 数据流名称
	      string strDataStreamValue 数据流值
		  string strDataStreamUnit 数据流单位
返 回 值：错误代码，成功为0
说    明：在第一次添加内容值时写入格式字节，如后面加入的数据格式与第一次不同则不能加入并报错
其    它：hpy added 20131203
**************************************************************************************/
short CSpecialFuncTestShow::Add (string strDataStreamName, string strDataStreamValue, string strDataStreamUnit)
{	
	// 如果是第一次添加数据流内容
	if(!m_bAddDataStream)
	{
		m_bAddDataStream = true;

		// 数据流格式
		char chDataStreamFormat = (char)m_iDataStreamFormat;
		m_pSendAndReceive->Add(chDataStreamFormat);

		// 数据流数量
		short iCnt = 0;
		m_pSendAndReceive->Add(iCnt);
	}
	
	//诊断程序控制标识
	m_pSendAndReceive->SetFlag(STRING_ONLY);

	// 数据流名称
	m_pSendAndReceive->Add(strDataStreamName);

	// 数据流值
	m_pSendAndReceive->Add(strDataStreamValue);
	
	// 数据流单位
	m_pSendAndReceive->Add(strDataStreamUnit);

	// 递增数据流数量  长度(2字节) + 功能代码(1字节) + 调试标志(1字节) + 按钮数量(2字节) + 标题长度 + 按纽文本ID + 按纽状态 + 数据流数据格式(1字节)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}
