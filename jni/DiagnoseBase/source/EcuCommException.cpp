/*************************************************************
  Copyright (c) 2006, AUTOBOSS Ltd,.Co 
  All rights reserved.
 
  文件名称：EcuCommException.h
  文档标识：通信异常信息类
  摘    要：

  历史记录：
  ----------------------------------------------------------------------------
  时     间   作者   版本号	  操    作    内    容                               
  ----------------------------------------------------------------------------
  2006.6.1    fufs  1.0      创建此类
  2006.11.10  wcx            加入闪光码异常内容

****************************************************************/
#ifdef WIN32
#include "Display.h"
#ifndef CPP_TEST
extern CDisplay * g_pDisplay;
#endif
#endif
#include "EcuCommException.h"
#include <string>

std::string CEcuCommException::m_astrException[-CEcuCommException::ECX_MAX_ERROR_CODE_NUMBER] = 
{
	"UNKNOW",								// 0
	"OVERTIME",							// -1
	"execute break",						// -2
	"ECU no respond",						// -3
	"UNDEFINE",								// -4
	"UNDEFINE",								// -5
	"UNDEFINE",								// -6
	"UNDEFINE",								// -7
	"UNDEFINE",								// -8
	"UNDEFINE",								// -9
	"DOWN BOARD NO RESPONSION",				// -10

//结果类
	"IO_VOLTAGE",							// -11	0xF5 电压不匹配
	"BUFFER_OVER",							// -12	0xF4 缓冲区溢出
	"INVALIDATION PARAMETER",				// -13 0xF3 无效参数
	"FLASH ERASE FAILURE",					// -14  0xF2 擦除FLASH失败
	"FLASH WRITE PROTECTED",				// -15 0xF1 写保护
	"WRITE_FLASH_FAILURE",					// -16 0xF0 写FLASH 失败
	"RUN INVALIDATION PROGRAM PAGE",		// -17 0xEF 运行无效程序页面
	"CAN IO ERROR",							// -18 0xEE CAN OPEN ERROR
	"CAN SEND ERROR",						// -19 0xFD CAN SEND ERROR
	"CAN RECV LEN ERROR",					// -20 0xFC 数据长度超长错误。
	"CAN NO ANWSER ERROR",					// -21 0xFB 没有接受数据
	"CAN INVALIDATION BIT TIMING",			// -22 0xFA 无效位时间设定
	"DATA OVER",							//= -23, // 0xF9 数据溢出
	"NO PULSE FOUND",						//= -24, // 0xF8 没有期待的脉冲出现
	"NO IDLESSE"							//= -25, // 0xF7 未找到空闲电平

};

CEcuCommException::CEcuCommException()
{
	m_iErrorCode      = 0;
	m_strOtherMessage =  "";
}

CEcuCommException::~CEcuCommException()
{

}
CEcuCommException::CEcuCommException(short iErrorCode, string strOtherMessageString, string strFunctionName)
{
	SetExceptionMessage (iErrorCode, strOtherMessageString, strFunctionName);
}

/*-----------------------------------------------------------------------------
  功    能：设置异常代码及函数名 
  参数说明：unsigned short iErrorCode 异常代码
			string strFunctionName 抛出异常的函数名
			string strOtherMessageString 附加的说明
  返 回 值：无
  说    明：无
-----------------------------------------------------------------------------*/
void CEcuCommException::SetExceptionMessage (short iErrorCode, 
											 string strOtherMessage, 
											 string strFunctionName)
{
	m_iErrorCode      = iErrorCode;
	m_strOtherMessage = strOtherMessage;
	m_strFunctionName = strFunctionName;
}

/*-----------------------------------------------------------------------------
  功    能：读出异常代码 
  参数说明：无
  返 回 值：错误代码
  说    明：无
-----------------------------------------------------------------------------*/
short CEcuCommException::GetErrorCode ()
{
	return m_iErrorCode;
}

#ifdef WIN32 
/*-----------------------------------------------------------------------------
  功    能：在DEBUG下将抛出当前异常的函数名、异常代码、及代码解释文字通过调用显
            示接口的adsMessageBox()函数显示出来，在RELEASE版本下不做任何操作。 
  参数说明：unsigned short nType,为adsMessageBox()的按钮类型，缺省为adsMB_Cancel 
  返 回 值：adsMessageBox()的返回值ID，RELEASE版本下返回adsIDCANCEL
  说    明：无
-----------------------------------------------------------------------------*/
unsigned short CEcuCommException::ReportError(unsigned short nType)
{
//#ifdef DEBUG
//	add test
//	return 0; 
	char szTemp[30];
	string strTemp;

	sprintf(szTemp, "ErrorCode : %2d\n", m_iErrorCode);
	strTemp = szTemp;

	int iErrorCode = m_iErrorCode;
	if ((iErrorCode>0) || (iErrorCode<ECX_MAX_ERROR_CODE_NUMBER))
		iErrorCode = 0;

	strTemp += "What : "+m_astrException[-iErrorCode]+"\n";
	strTemp += "Function : \n  "+m_strFunctionName+"\n";
	strTemp += "Parameter : \n  "+m_strOtherMessage;
#ifndef CPP_TEST
	return g_pDisplay->MessageBox(strTemp, "abnormity", nType);
#else
	return 0;
#endif

}
#endif
