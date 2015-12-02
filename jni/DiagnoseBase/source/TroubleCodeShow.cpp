/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：	TroubleCodeShow.cpp
	文档标识：	007汽车诊断平台详细设计说明书
	摘    要：	故障码显示类

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			创建此入口。                                           

**************************************************************************************/
#include "Display.h"
#include "TroubleCodeShow.h"
#include "Database.h"
#include "assert.h"


//根据ID从文本库返回字符串
extern string adsGetTextString (CBinary binTextID);


CTroubleCodeShow::CTroubleCodeShow()
{
	m_pfnGetTroubleCodeString = NULL;
	m_DRV_FileName = "";
}

/***************************************************************************************
功    能：初始化故障码
参数说明：char *pTitle 故障码标题，如为空串显示程序将使用缺省标题
		  unsigned int bMode  0:普通模式 1:数据驱动模式
返 回 值：无
说    明：无
**************************************************************************************/
void CTroubleCodeShow::Init (const char *pTitle,unsigned int bMode)
{
	assert(NULL != pTitle);
	char chFuction = DT_TROUBLE_CODE;
	m_pSendAndReceive->Init(chFuction);  // 填充功能代码和调试标志
	m_dataDrvMode = false;
	// 故障码数量
	short iCodeNum = 0;
	m_pSendAndReceive->Add(iCodeNum);

	// 故障码标题
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);
#ifdef DATA_DRV_EXT
	if(bMode == 1)
	{
		// 数据驱动使用的PTC文件名
		m_dataDrvMode = true;
		if (m_DRV_FileName.length() > 0)
		{
			const char* ptcName = m_DRV_FileName.c_str();
			char t = (char)0xfe;
			m_pSendAndReceive->Add(&t,2);
			m_pSendAndReceive->Add(ptcName, strlen(ptcName)+1);
		}
	}
#endif
}

/***************************************************************************************
功    能：初始化故障码
参数说明：string strTitle故障码显示标题
返 回 值：无
说    明：无
**************************************************************************************/
void CTroubleCodeShow::Init (string strTitle,unsigned int bMode)
{
	// 调用重载函数
	Init(strTitle.c_str(),bMode);
}

/***************************************************************************************
功    能：初始化故障码
参数说明：CBinary idTitle 故障码显示标题ID
返 回 值：无
说    明：无
**************************************************************************************/
void CTroubleCodeShow::Init (CBinary idTitle,unsigned int bMode)
{
	// 根据ID转换成字符串
	string strTitle = adsGetTextString(idTitle);

	// 调用重载函数
	Init(strTitle,bMode);
}

/***************************************************************************************
功    能：设置故障码回调函数指针
参数说明：pfnCallBack 回调函数指针
返 回 值：原回调函数指针
说    明：每添加一个故障码调用回调函数一次，回调函数返回"故障代码"字串，
          用于在显示程序在故障码库中找不到该故障码ID时显示故障代码内容
**************************************************************************************/
void *CTroubleCodeShow::SetTroubleCodeCallBackFunction (string (*pfnCallBack) (CBinary idStroubleCode))
{
	m_pfnGetTroubleCodeString = pfnCallBack;

	return (void *)m_pfnGetTroubleCodeString;
}

/***************************************************************************************
功    能：添加故障码内容
参数说明：CBinary idTroubleCode 故障码ID；string strTroubleStatus 故障码状态字符串
返 回 值：成功-false 失败-true
说    明：无
**************************************************************************************/
bool CTroubleCodeShow::Add (CBinary idTroubleCode, string strTroubleStatus)
{
	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());

	// 故障码ID
	m_pSendAndReceive->Add(idTroubleCode);

	// 故障码状态
	m_pSendAndReceive->Add(strTroubleStatus);

	// 根据ID转换成字符串
	if (!m_dataDrvMode){
		string strTroubleCode;
		if(NULL == m_pfnGetTroubleCodeString)
		{
			strTroubleCode = DefaultStroubleCodeCallBack(idTroubleCode);  // 故障码缺省回调函数
		}
		else
		{		
			strTroubleCode = m_pfnGetTroubleCodeString(idTroubleCode);  // 使用函数指针
		}	
		
		// 故障码代码串
		m_pSendAndReceive->Add(strTroubleCode);
	}
	// 递增故障码数量
	m_pSendAndReceive->IncreaseNum(4);

	return true;
}
/***************************************************************************************
功    能：添加故障码内容
 此接口暂时只对数据驱动模式的诊断程序使用
参数说明：CBinary idTroubleCode 故障码ID；CBinary idDesc 故障码描述；string strTroubleStatus 故障码状态字符串
返 回 值：成功-false 失败-true
说    明：无
**************************************************************************************/
// 
// bool CTroubleCodeShow::Add (CBinary idTroubleCode, CBinary idDesc,string strTroubleStatus)
// {
// 	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());
// 	
// 	// 故障码ID
// 	m_pSendAndReceive->Add(idTroubleCode);
// 	//故障码描述ID
// 	m_pSendAndReceive->Add(idDesc);
// 	
// 	// 故障码状态
// 	if (strTroubleStatus.length() > 0)
// 		m_pSendAndReceive->Add(strTroubleStatus);
// 	else
// 		m_pSendAndReceive->Add("");
// 	
// 	
// 	// 根据ID转换成字符串
// 	// 	if (!m_dataDrvMode){
// 	string strTroubleCode;
// 	CBinary flag = CBinary("\xFF\xFF\xFF\xFF\xFF\xFE",6);
// 	if (idTroubleCode == flag){
// 		//DataDrv especially
// 		unsigned char temp = 0;
// 		if (strTroubleStatus.length() >= 4){
// 			string tempStr = strTroubleStatus.substr(strTroubleStatus.length() - 2);
// 			CHARS2HEX(&temp,tempStr.c_str(),1);
// 			idTroubleCode.SetAt(5,temp);
// 			tempStr = strTroubleStatus.substr(strTroubleStatus.length() - 4,2);
// 			CHARS2HEX(&temp,tempStr.c_str(),1);
// 			idTroubleCode.SetAt(4,temp);
// 		}
// 	}
// 	if(NULL == m_pfnGetTroubleCodeString)
// 	{
// 		
// 		strTroubleCode = DefaultStroubleCodeCallBack(idTroubleCode);  // 故障码缺省回调函数
// 	}
// 	else
// 	{		
// 		strTroubleCode = m_pfnGetTroubleCodeString(idTroubleCode);  // 使用函数指针
// 	}
// 	
// 	// 故障码代码串
// 	m_pSendAndReceive->Add(strTroubleCode);
// 	// 	}
// 	// 递增故障码数量
// 	m_pSendAndReceive->IncreaseNum(4);
// 	
// 	return true;
// }

bool CTroubleCodeShow::Add (CBinary idTroubleCode, CBinary idDesc,string strTroubleStatus,string strSAEFromConv)
{
	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());

	// 故障码ID
	m_pSendAndReceive->Add(idTroubleCode);
	//故障码描述ID
	m_pSendAndReceive->Add(idDesc);

	// 故障码状态
	if (strTroubleStatus.length() > 0)
		m_pSendAndReceive->Add(strTroubleStatus);
	else
		m_pSendAndReceive->Add("");

	string strTroubleCode = "";
	// 根据ID转换成字符串
 	if (m_dataDrvMode){
		CBinary flag = CBinary("\xFF\xFF\xFF\xFF\xFF\xFE",6);
		if (idTroubleCode == flag){
			//DataDrv especially
			if (strSAEFromConv.length() > 0){
				//DataDrv flag "SAE:"
				strTroubleCode += "SAE:";
				strTroubleCode += strSAEFromConv;
			}
		}
	}else{
		if(NULL == m_pfnGetTroubleCodeString)
		{
			
			strTroubleCode = DefaultStroubleCodeCallBack(idTroubleCode);  // 故障码缺省回调函数
		}
		else
		{		
			strTroubleCode = m_pfnGetTroubleCodeString(idTroubleCode);  // 使用函数指针
		}
	}
		// 故障码代码串
	if (strTroubleCode.length() != 0)
		m_pSendAndReceive->Add(strTroubleCode);
// 	}
	// 递增故障码数量
	m_pSendAndReceive->IncreaseNum(4);

	return true;
}

/***************************************************************************************
功    能：添加故障码内容
参数说明：CBinary idTroubleCode 故障码ID；CBinary idTroubleStatus 故障码状态ID
返 回 值：成功-false 失败-true
说    明：无
**************************************************************************************/
bool CTroubleCodeShow::Add (CBinary idTroubleCode, CBinary idTroubleStatus)
{
	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());
	assert(ID_BINARY_LENTH == idTroubleStatus.GetSize());

	// 根据ID转换成字符串
	string strTroubleStatus = adsGetTextString(idTroubleStatus);

	//调用重载函数
	Add(idTroubleCode, strTroubleStatus);

	return true;
}

/***************************************************************************************
功    能：通知显示程序取数据并等待回应
参数说明：无
返 回 值：无
说    明：无
**************************************************************************************/
void CTroubleCodeShow::Show ()
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
}

/***************************************************************************************
功    能：设置动作测试的标志位
参数说明：0x01:支持鼠标点击，并能返回所选行数
返 回 值：错误代码，成功为1
说    明：
**************************************************************************************/
bool CTroubleCodeShow::SetFlag(unsigned char iFlag)
{
	//m_pSendAndReceive->Add(iFlag);
	m_pSendAndReceive->SetFlag(iFlag);
	return true;
}


/***************************************************************************************
功    能：显示动作测试界面并回送用户所选菜单序号和用户按键
参数说明：iSelNum：用户所选择的菜单序号（以0开始计数）
返 回 值：用户按键序号
说    明：结果需要由用户所选菜单序号和按键来共同决定
**************************************************************************************/
short CTroubleCodeShow::Show (short &iSelNum)
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


string  CTroubleCodeShow::GetSelectedItemText(short iCol)
{	
	// 获取收到的数据
/*	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);
	int i=0;
	int nColLen = 0;
	while(1)
	{
		int nLen = strlen(pchContain+8+nColLen);
		if(nLen == 0) 
			return NULL;
		
		if(i==iCol)
			return pchContain+8+nColLen;
		else
			nColLen += nLen;
	}*/
	return 0;
}

/***************************************************************************************
功    能：故障码缺省回调函数
参数说明：CBinary idStroubleCode 故障码ID
返 回 值：无
说    明：在诊断程序员未设定故障码回调函数时，缺省回调函数将ID的低二位字节的ID值转换成PCBU码
         （如ID为 0x01 0x02 0x03 0x04 0x01 0x23 则输出的故障码为"P0123"）
**************************************************************************************/
string CTroubleCodeShow::DefaultStroubleCodeCallBack(CBinary idTroubleCode)
{
	assert(ID_BINARY_LENTH == idTroubleCode.GetSize());
	
	short iLenth = idTroubleCode.GetSize();

	char chHigh;
	char chLow;
	string strTroubleCode;
	for(short i = 0; i < (iLenth-1)/6+1; i++)
	{
		chHigh = idTroubleCode.GetAt((iLenth-1)/6 + 4);  // 取第5个字节
		chLow  = idTroubleCode.GetAt((iLenth-1)/6 + 5);  // 取第6个字节

		strTroubleCode += TanslateToPCBU(chHigh, chLow);
	}
	return strTroubleCode;
}

/***************************************************************************************
功    能：将两个字节转换成PCBU码
参数说明：char chHigh, char chLow
返 回 值：无
说    明：无
**************************************************************************************/
string CTroubleCodeShow::TanslateToPCBU(char chHigh, char chLow)
{
	string strTroubleCode;

	short iTemp = (chHigh>>6)&0x03;
	switch(iTemp)
	{
	case 0:
		strTroubleCode = "P";
		break;

	case 1:
		strTroubleCode = "C";
		break;

	case 2:
		strTroubleCode = "B";
		break;

	case 3:
		strTroubleCode = "U";
		break;

	default:
		break;
	}

	char chBufferTmp[20];
	string strTmp;

	iTemp = (chHigh>>4)&0x03;
	sprintf(chBufferTmp, "%x", iTemp);
	strTmp = chBufferTmp;	
	strTroubleCode += strTmp.c_str();

	iTemp = chHigh&0x0F;
	sprintf(chBufferTmp, "%x", iTemp);
	strTmp = chBufferTmp;	
	strTroubleCode += strTmp.c_str();

	iTemp = (chLow>>4)&0x0F;
	sprintf(chBufferTmp, "%x", iTemp);
	strTmp = chBufferTmp;	
	strTroubleCode += strTmp.c_str();

	iTemp = chLow&0x0F;
	sprintf(chBufferTmp, "%x", iTemp);
	strTmp = chBufferTmp;	
	strTroubleCode += strTmp.c_str();

	return strTroubleCode;
}

/***************************************************************************************
功    能：添加故障码内容
参数说明：string strTroubleCode 故障码字符串；
		  string strTroubleDesc 故障码描述字符串；
		  string strTroubleStatus 故障码状态字符串
返 回 值：成功-false 失败-true
说    明：此接口暂时只供诊断程序在必要的时候使用，以节省共享空间
其    它：hpy added 20131203
****************************************************************************************/
bool CTroubleCodeShow::Add (string strTroubleCode, string strTroubleDesc, string strTroubleStatus)
{
	m_pSendAndReceive->SetFlag(STRING_ONLY);
	
	// 故障码故障码字符串
	m_pSendAndReceive->Add(strTroubleCode);

	//故障码描述字符串
	m_pSendAndReceive->Add(strTroubleDesc);
	
	//故障码状态字符串
	m_pSendAndReceive->Add(strTroubleStatus);

	//递增故障码数量
	m_pSendAndReceive->IncreaseNum(4);
	
	return true;
}


