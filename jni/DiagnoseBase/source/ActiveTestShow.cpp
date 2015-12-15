#ifdef WIN32
#include "windows.h"
#endif

#include "arithmetic.h"
#include "ActiveTestShow.h"
#include "Database.h"
#include "assert.h"
#include "display.h"

#define INTERVAL_TIME 50


//根据ID从文本库返回字符串
extern string adsGetTextString (CBinary binTextID);
//extern CArithmetic::SetDebugState (unsigned char uiDebugItem, bool bEnable);


CActiveTestShow::CActiveTestShow()
{
	m_iButtonNum = 0;
	m_iHeaderNum = 0;
	m_iAllHeaderTextLen = 0;
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
void CActiveTestShow::Init (const char* pTitle)
{
	assert(NULL != pTitle);

	// 按钮数量清0
	m_iButtonNum = 0;

	// 列数数量清0
	m_iHeaderNum = 0;
	m_iAllHeaderTextLen = 0;

	// 数据流格式复位
	m_iDataStreamFormat = -1;

	// 是否已经添加数据流内容
	m_bAddDataStream    = false; 


	m_strMsg = "";

	m_iTitleLenth = strlen(pTitle)+1;  // 计算标题长度

	char chFuction = DT_ACTIVE_TEST;
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
void CActiveTestShow::Init (string strTitle)
{	
	// 调用重载函数
	Init(strTitle.c_str());
}

/***************************************************************************************
功    能：初始化动作测试
参数说明：CBinary idTitle 动作测试标题，
返 回 值：无
说    明：因为未加入动作测试按钮，需要配合AddButton ()函数一起使用
**************************************************************************************/
void CActiveTestShow::Init (CBinary idTitle)
{
	// 根据ID转换成字符串
	string strTitle = adsGetTextString(idTitle);

	// 调用重载函数
	Init(strTitle);
}


/***************************************************************************************
功    能：添加动做测试界面按钮
参数说明：CBinary idButtonText 添加的按钮标题，
返 回 值：错误代码
说    明：无
**************************************************************************************/
short CActiveTestShow::AddButton (CBinary idButtonText, char byStatus)
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
功    能：设置动作测试的标志位
参数说明：iFlag 0xFF表示此动作测试支持特殊功能

0x01: 增加文字说明的文本框，
0x02: 支持自定义列头
0x03: 单列，无列头
0xFF: 支持鼠标单击
返 回 值：错误代码，成功为1
说    明：
**************************************************************************************/
bool CActiveTestShow::SetFlag(unsigned char byFlag)
{
	unsigned char byTmp = m_pSendAndReceive->GetFlag();//hpy modified 20131216
	byTmp = byTmp | byFlag;
	m_pSendAndReceive->SetFlag(byTmp);
	return true;

//	m_pSendAndReceive->SetFlag(byFlag);
//	return true;
}

/***************************************************************************************
功    能：添加多选项的说明
参数说明：内容文本
返 回 值：错误代码，成功为0
说    明：无 add by scf 2008\2\29
**************************************************************************************/
short CActiveTestShow::AddMsg(string strMsg)
{
	//m_pSendAndReceive->Add(strMsg);
	m_strMsg = strMsg;
	return 0;
}


/***************************************************************************************
功    能：添加多选项列头的文字说明
参数说明：内容文本
返 回 值：错误代码，成功为0
说    明：无 add by scf 2008\10\31,
每添加一项，列数目++，同时设置模式为自定列头模式
**************************************************************************************/

short CActiveTestShow::AddHeaderText(string strHeaderMsg)
{
	//m_pSendAndReceive->SetFlag(0x02);
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum;
	if(m_iHeaderNum == 0)
		m_pSendAndReceive->Add(m_iHeaderNum);
	
	m_iHeaderNum++;
	m_pSendAndReceive->Add(strHeaderMsg);
	m_iAllHeaderTextLen += strlen(strHeaderMsg.c_str())+1;
	m_pSendAndReceive->IncreaseNum(iNum);
	return m_iAllHeaderTextLen;
}


/***************************************************************************************
功    能：添加数据流内容项
参数说明：CBinary idDataStream 数据流ID
		  string strDataStreamValue 数据流数值
返 回 值：错误代码，成功为0
说    明：在第一次添加内容值时写入格式字节，如后面加入的数据格式与第一次不同则不能加入并报错
**************************************************************************************/
short CActiveTestShow::Add (CBinary idDataStream, string strDataStreamValue)
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
	if(m_iHeaderNum>0)
		    iNum = 2 + 2 + 2 + m_iTitleLenth + 2 + m_iAllHeaderTextLen + 6*m_iButtonNum + m_iButtonNum + 1;

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
short CActiveTestShow::Add (CBinary idDataStream, string strDataStreamValue, CBinary idUnit)
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
	if(m_iHeaderNum>0)
		    iNum = 2 + 2 + 2 + m_iTitleLenth + 2 + m_iAllHeaderTextLen + 6*m_iButtonNum + m_iButtonNum + 1;

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
short CActiveTestShow::Add (CBinary idDataStream, string strValue1, string strValue2)
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

	// 数据流单位串
	m_pSendAndReceive->Add(strValue2);

	// 数据流数值串
	m_pSendAndReceive->Add(strValue1);

	// 递增数据流数量  长度(2字节) + 功能代码(1字节) + 调试标志(1字节) + 按钮数量(2字节) + 标题长度 + 按纽文本ID + 按纽状态 + 数据流数据格式(1字节)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	if(m_iHeaderNum>0)
		iNum = 2 + 2 + 2 + m_iTitleLenth + 2 + m_iAllHeaderTextLen +6*m_iButtonNum + m_iButtonNum + 1;

	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}


/****************************************
// [0] DisplayCount
// [1] DiagnoseCount
// [2] Msg
// [3] Valid
****************************************/
unsigned char CActiveTestShow::AcceptMsg ()
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
功    能：显示动作测试界面并回送用户按键
参数说明：char *pMarkCode 按钮掩码指针，如为NULL则无选中项
返 回 值：用户按键序号
说    明：显示程序取走数据后立即回送无户按键序号
**************************************************************************************/
short CActiveTestShow::Show ()
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

	m_pSendAndReceive->Add(m_strMsg);

	// 发送消息
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	// 获取收到的数据d
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
功    能：显示动作测试界面并回送用户所选菜单序号和用户按键
参数说明：iSelNum：用户所选择的菜单序号（以0开始计数）
返 回 值：用户按键序号
说    明：结果需要由用户所选菜单序号和按键来共同决定
**************************************************************************************/
short CActiveTestShow::Show (short &iSelNum)
{
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
//	m_pSendAndReceive->SetFlag(iFlag);
	m_pSendAndReceive->Add(m_strMsg);	//hpy added 20140113

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

	// 第5, 6个字节表示:菜单用户选项
	unsigned char chHigh = pchContain[6];    // 菜单用户选项的高字节部分
    unsigned char chLow  = pchContain[7];    // 菜单用户选项的低字节部分
	if(chHigh == 0xff && chLow == 0xff)
	{
		iSelNum = -1;
	}
	else
	{
		iSelNum = 256*chHigh + chLow;
	}

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
short CActiveTestShow::Add (string strDataStreamName, string strDataStreamValue, string strDataStreamUnit)
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

	// 递增数据流数量  长度(2字节) + 功能代码(1字节) + 调试标志(1字节) + 按钮数量(2字节) + 标题长度 + 按纽文本ID + 按纽状态 + 数据流数据格式(1字节)
	short iNum = 2 + 2 + 2 + m_iTitleLenth + 6*m_iButtonNum + m_iButtonNum + 1;
	if(m_iHeaderNum>0)
		    iNum = 2 + 2 + 2 + m_iTitleLenth + 2 + m_iAllHeaderTextLen + 6*m_iButtonNum + m_iButtonNum + 1;

	m_pSendAndReceive->IncreaseNum(iNum);

	return true;
}
