/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：	DataStreamShow.cpp
	文档标识：	007汽车诊断平台详细设计说明书
	摘    要：	数据流显示类。
	功    能：
				1、	将数据数据流数据按规定格式写入缓冲区，通知显示程序取走数据并显示。
				2、	回送显示程序存贮的用户按键。

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			创建此类。                                           

**************************************************************************************/

#include "display.h"
#ifdef WIN32
#include "windows.h"
#endif
#include "DataStreamShow.h"
#include "Database.h"
#include "arithmetic.h"
#include <assert.h>

#define INTERVAL_TIME 50


//根据ID从文本库返回字符串
extern string adsGetTextString (CBinary binTextID);
//extern CArithmetic::SetDebugState (unsigned char uiDebugItem, bool bEnable);

CDataStreamShow::CDataStreamShow()
{
	m_iDataStreamFormat = -1;  // 数据流格式
}

CDataStreamShow::~CDataStreamShow()
{
}


/***************************************************************************************
功    能：初始化数据流
参数说明：char* pTitle 数据流标题，如为空显示程序使用缺省标题
		  char* pStdValueLibName = NULL 数据流标准库名称，缺省为空
		  unsigned short uiType 数据流按钮显示方式，缺省为不显示前后通道按钮
返 回 值：无
说    明：无
**************************************************************************************/
#if 0
void CDataStreamShow::Init(const char* pTitle, const char* pStdValueLibName, unsigned short uiType)
{
	// 数据流格式复位
	m_iDataStreamFormat = -1;

	//added by johnnyling
	m_iDataStreamType=uiType;

	char chFuction = DT_DATA_STREAM;
	m_pSendAndReceive->Init(chFuction);  // 填充功能代码和调试标志
	
	// 按键显示标志	
	char chType = (char)m_iDataStreamType; //uiType;
	m_pSendAndReceive->Add(chType);  

	// 数据流数据格式
	char chFormat = 0;
	m_pSendAndReceive->Add(chFormat); 

	// 数据流数量
	unsigned short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	// 数据流标准库名称	
	if(NULL == pStdValueLibName)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		m_pSendAndReceive->Add(pStdValueLibName, strlen(pStdValueLibName)+1);
	}

	if(NULL == pTitle)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		// 数据流标题	
		m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
	}
}
#endif

/***************************************************************************************
功    能：初始化数据流
参数说明：string strTitle 数据流标题
		  string strStdValueLibName 数据流标准库名称，缺省为空串
		  unsigned short uiType 数据流按钮显示方式，缺省为不显示前后通道按钮
返 回 值：无
说    明：无
**************************************************************************************/
void CDataStreamShow::Init(string strTitle, string strStdValueLibName, unsigned short uiType)
{
	// 调用重载函数
	//Init(strTitle.c_str(), strStdValueLibName.c_str(), uiType);
	const char* pTitle;
	const char* pStdValueLibName;

	if(strTitle.length())
		pTitle=strTitle.c_str();
	else
		pTitle=NULL;

	if(strStdValueLibName.length())
		pStdValueLibName=strStdValueLibName.c_str();
	else
		pStdValueLibName=NULL;

	// 数据流格式复位
	m_iDataStreamFormat = -1;

	//added by johnnyling
	m_iDataStreamType=uiType;

	char chFuction = DT_DATA_STREAM;
	m_pSendAndReceive->Init(chFuction);  // 填充功能代码和调试标志
	
	// 按键显示标志	
	char chType = (char)m_iDataStreamType; //uiType;
	m_pSendAndReceive->Add(chType);  

	// 数据流数据格式
	char chFormat = 0;
	m_pSendAndReceive->Add(chFormat); 

	// 数据流数量
	unsigned short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	// 数据流标准库名称	
	if(NULL == pStdValueLibName)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		m_pSendAndReceive->Add(pStdValueLibName, strlen(pStdValueLibName)+1);
	}

	if(NULL == pTitle)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		// 数据流标题	
		m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
	}
}


/***************************************************************************************
功    能：初始化数据流
参数说明：
		unsigned short uiTopLine：起始行号
		unsigned short uiAllCount：数据流总行数
		string strTitle 数据流标题
		  string strStdValueLibName 数据流标准库名称，缺省为空串
		  unsigned short uiType 数据流按钮显示方式，缺省为不显示前后通道按钮
返 回 值：无
说    明：无
**************************************************************************************/
void CDataStreamShow::Init(unsigned short uiTopLine,unsigned short uiAllCount,string strTitle, string strStdValueLibName, unsigned short uiType)
{
	// 调用重载函数
	//Init(strTitle.c_str(), strStdValueLibName.c_str(), uiType);
	const char* pTitle;
	const char* pStdValueLibName;

	if(strTitle.length())
		pTitle=strTitle.c_str();
	else
		pTitle=NULL;

	if(strStdValueLibName.length())
		pStdValueLibName=strStdValueLibName.c_str();
	else
		pStdValueLibName=NULL;

	// 数据流格式复位
	m_iDataStreamFormat = -1;

	//added by johnnyling
	m_iDataStreamType=uiType;

	char chFuction = DT_DATA_STREAM;
	m_pSendAndReceive->Init(chFuction);  // 填充功能代码和调试标志

	m_pSendAndReceive->SetFlag(0x01);

	// 按键显示标志	
	char chType = (char)m_iDataStreamType; //uiType;
	m_pSendAndReceive->Add(chType);  

	// 数据流数据格式
	char chFormat = 0;
	m_pSendAndReceive->Add(chFormat); 

	// 数据流数量
	unsigned short iNum = 0;
	m_pSendAndReceive->Add(iNum);

	//////////////add by scf 2008.07.09///////////
	//所有数据流条数
	m_pSendAndReceive->Add(uiAllCount);

	//数据流起始行号
	m_pSendAndReceive->Add(uiTopLine);
	//////////////End add by scf 2008.07.09///////////

	// 数据流标准库名称	
	if(NULL == pStdValueLibName)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		m_pSendAndReceive->Add(pStdValueLibName, strlen(pStdValueLibName)+1);
	}

	if(NULL == pTitle)
	{
		char chTemp = '\0';
		m_pSendAndReceive->Add(chTemp);
	}
	else
	{
		// 数据流标题	
		m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
	}
}

/***************************************************************************************
功    能：设置数据流显示的方法
参数说明：iFlag 0x01表示此数据流采用快速显示
返 回 值：错误代码，成功为1
说    明：
**************************************************************************************/
//unsigned short CDataStreamShow::SetFlag(unsigned short iFlag)
//{
	
//	return true;
//}


/***************************************************************************************
功    能：初始化数据流
参数说明：CBinary idTitle 数据流标题ID
		  CBinary idStdValueLibName 数据流标准库名称ID
		  unsigned short uiType 数据流按钮显示方式，缺省为不显示前后通道按钮
返 回 值：无
说    明：无
**************************************************************************************/
void CDataStreamShow::Init (CBinary idTitle, CBinary idStdValueLibName, unsigned short uiType)
{
	// 根据ID转换成字符串
	string strTitle = adsGetTextString(idTitle);

	// 根据ID转换成字符串
	string strStdValueLibName = adsGetTextString(idStdValueLibName);

	// 调用重载函数
	Init(strTitle, strStdValueLibName, uiType);
}

/***************************************************************************************
功    能：初始化数据流
参数说明：CBinary idTitle 数据流标题ID
返 回 值：无
说    明：无
**************************************************************************************/
void CDataStreamShow::Init (CBinary idTitle, string strStdValueLibName, unsigned short uiType)
{
	Init (adsGetTextString(idTitle), strStdValueLibName, uiType);
}


/***************************************************************************************
功    能：添加数据流内容项
参数说明：CBinary idDataStream 数据流ID
	      string strDataStreamValue 数据流数值
返 回 值：错误代码，成功为0
说    明：在第一次添加内容值时写入格式字节，如后面加入的数据格式与第一次不同则不能加入并报错
**************************************************************************************/
unsigned short CDataStreamShow::Add (CBinary idDataStream, string strDataStreamValue,unsigned short nIndex,bool bUpdateFlag)
{
	if(BDF_VW == m_iDataStreamFormat)
	{
		return EC_TYPE_DATA_STREAM;  // 数据流格式不一致
	}

	if(ID_BINARY_LENTH != idDataStream.GetSize())
	{
		return EC_DATA_FORMAT;  // 数据格式错
	}

	// 一般数据流
	m_iDataStreamFormat = BDF_NOMAL;

	// 数据流ID
	m_pSendAndReceive->Add(idDataStream);

	// 数据流数值串
	m_pSendAndReceive->Add(strDataStreamValue);

	// 递增数据流数量
	m_pSendAndReceive->IncreaseNum(6);

	/*
	typedef struct MSG_DATA
	{
		unsigned char CmdId;
		unsigned short iIndex;
		unsigned char	ID[6];
		CHAR	chValue[30];
		CHAR	chUnit[20];
	}MSG_DATA, *PMSGDATA;
	*/

	if(bUpdateFlag)
	{
		MSG_DATA *pMsgData = new MSG_DATA;
		memset(pMsgData,0,sizeof(MSG_DATA));
		pMsgData->CmdId = DT_DATA_STREAM;
		pMsgData->iIndex = nIndex;
		idDataStream.ReadBuffer(pMsgData->chID,0,6);
		sprintf(pMsgData->chValue,"%s",strDataStreamValue.c_str());
		bool bSend = false;
#ifdef WIN32 //need to modify
		COPYDATASTRUCT copyData; 
		copyData.cbData = sizeof(MSG_DATA); 
		copyData.lpData = pMsgData;
		bSend = m_pSendAndReceive->SendDataToDisplayImmediateResponsion((LPARAM)&copyData);
#endif
		if(pMsgData)
		{
			delete []pMsgData;
			pMsgData = NULL;
		}
		return bSend;
	}

#ifdef DATA_DRV_EXT
	return false;
#endif	//	#ifdef DATA_DRV_EXT
}

/***************************************************************************************
功    能：添加数据流内容项
参数说明：CBinary idDataStream 数据流ID
	      string strDataStreamValue 数据流数值
		  CBinary idUnit 数据流单位ID
返 回 值：错误代码，成功为0
说    明：在第一次添加内容值时写入格式字节，如后面加入的数据格式与第一次不同则不能加入并报错
**************************************************************************************/
unsigned short CDataStreamShow::Add (CBinary idDataStream, string strDataStreamValue, CBinary idUnit,unsigned short nIndex,bool bUpdateFlag)
{
	if(BDF_NOMAL == m_iDataStreamFormat)
	{
		return EC_TYPE_DATA_STREAM;  // 数据流格式不一致
	}
	
	if(ID_BINARY_LENTH != idDataStream.GetSize() || ID_BINARY_LENTH != idUnit.GetSize())
	{
		return EC_DATA_FORMAT;  // 数据格式错
	}

	// 单位独立的数据流
	m_iDataStreamFormat = BDF_VW;

	// 数据流ID
	m_pSendAndReceive->Add(idDataStream);

	// 数据流单位ID
	m_pSendAndReceive->Add(idUnit);

	// 数据流数值串
	m_pSendAndReceive->Add(strDataStreamValue);

	// 递增数据流数量
	m_pSendAndReceive->IncreaseNum(6);
	
	if(bUpdateFlag)
	{
		MSG_DATA *pMsgData = new MSG_DATA;
		memset(pMsgData,0,sizeof(MSG_DATA));
		pMsgData->CmdId = DT_DATA_STREAM;
		pMsgData->iIndex = nIndex;
		idDataStream.ReadBuffer(pMsgData->chID,0,6);
		sprintf(pMsgData->chValue,"%s",strDataStreamValue.c_str());
		bool bSend = false;
#ifdef WIN32 //need to modify
		COPYDATASTRUCT copyData; 
		copyData.cbData = sizeof(MSG_DATA); 
		copyData.lpData = pMsgData;
		bSend = m_pSendAndReceive->SendDataToDisplayImmediateResponsion((LPARAM)&copyData);
#endif
		if(pMsgData)
		{
			delete []pMsgData;
			pMsgData = NULL;
		}
		return bSend;
	}
	return true;
}

bool CDataStreamShow::SetFlag(unsigned char iFlag)
{
	unsigned char byTmp = m_pSendAndReceive->GetFlag();
	byTmp = byTmp | iFlag;
	m_pSendAndReceive->SetFlag(byTmp);
	return true;
}
/***************************************************************************************
功    能：显示数据流并回送用户按键
参数说明：无
返 回 值：用户按键键值
说    明：显示程序取走数据后立即回送无户按键键值，
**************************************************************************************/
unsigned short CDataStreamShow::Show (bool bFlag)
{
	// 修改数据流格式
	char chDataStreamFormat = (char)m_iDataStreamFormat;
	m_pSendAndReceive->Modify(chDataStreamFormat, 5);
	if(bFlag) //add by scf 2009.12.02
	{
		unsigned char byTmp =	m_pSendAndReceive->GetFlag();
		byTmp = byTmp|0xF0;
		byTmp = byTmp&0xF2;
		m_pSendAndReceive->SetFlag(byTmp);
		//m_pSendAndReceive->SetFlag(0x02);
	}
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
		assert( iLenth < 5 );  
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


	// 第5个字节表示:菜单用户选项
	unsigned short iUserKey = pchContain[4];
	
	return iUserKey;
}

/****************************************
// [0] DisplayCount
// [1] DiagnoseCount
// [2] Msg
// [3] Valid
****************************************/
unsigned char CDataStreamShow::AcceptMsg ()
{
	unsigned char *pMsg;
	unsigned char DispCount;
	unsigned char DispValid;
	unsigned char Msg;
	
    //step 1 : 获得共享内存特定单元地址
    char *pchContain = m_pSendAndReceive->GetBuffer();
    pMsg=(unsigned char *)&pchContain[0x10000-4];

	//step 2 : 判断[3]=? 0x55, [0]=?[1]是否相同
    DispValid = pMsg[3];
    DispCount = pMsg[0];
	if(DispValid==0x55)
	{
		if(DispCount!=pMsg[1])
		{
	        //step 3 : 不相同 取[2] Msg  and [1]=[0]//需要实现;
            Msg=pMsg[2];
			pMsg[1]=DispCount;
			return Msg;
		}
	}
    return 0;
}

/***************************************************************************************
功    能：显示数据流并回送用户按键
参数说明：无
返 回 值：用户按键键值
说    明：显示程序取走数据后立即回送无户按键键值，
**************************************************************************************/
unsigned short CDataStreamShow::Show (unsigned short &iTop,unsigned short &iNum,bool bFlag)
{
	// 修改数据流格式
	char chDataStreamFormat = (char)m_iDataStreamFormat;
	m_pSendAndReceive->Modify(chDataStreamFormat, 5);

	//add by scf 2009.12.02
	if(bFlag)
	{
		unsigned char byTmp =	m_pSendAndReceive->GetFlag();
		byTmp = byTmp|0xF0;
		byTmp = byTmp&0xF3;
		m_pSendAndReceive->SetFlag(byTmp);
	}

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
		assert( iLenth < 5 );  
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


	// 第5个字节表示:菜单用户选项
	unsigned short iUserKey = pchContain[4];
	
	//第6、7节表示:当前序号
	iTop = (unsigned char)pchContain[5] * 256 + (unsigned char)pchContain[6];

	//第8、9节表示:当前行数
	iNum = (unsigned char)pchContain[7] * 256 + (unsigned char)pchContain[8];

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
unsigned short CDataStreamShow::Add (string strDataStreamName, string strDataStreamValue, string strDataStreamUnit)
{
	//诊断程序控制标识
	unsigned char flag = m_pSendAndReceive->GetFlag();
	flag |= STRING_ONLY;

	m_pSendAndReceive->SetFlag(flag);
//	m_pSendAndReceive->SetFlag(STRING_ONLY);

	// 数据流名称
	m_pSendAndReceive->Add(strDataStreamName);

	// 数据流值
	m_pSendAndReceive->Add(strDataStreamValue);

	// 数据流单位
	m_pSendAndReceive->Add(strDataStreamUnit);

	// 递增数据流数量
	m_pSendAndReceive->IncreaseNum(6);
	
	return true;
}
