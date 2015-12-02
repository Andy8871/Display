#ifndef __ECU_COMM_EXCEPTION_H__
#define __ECU_COMM_EXCEPTION_H__

#include "adsStd.h"
#ifdef WIN32
  #include "display.h"
#endif

#include <string>
using namespace std;

class CEcuCommException
{
public:
	enum 
	{
		ECX_OK								= 0,
		ECX_OVERTIME						= -1,

//		ECX_OVERTIME=1,// 通信超时        
		ECX_BREAK							= -2,	// 执行被中断      
		ECX_ECU_NO_RESPONSION				= -3,	// ECU无应答       
		ECX_ECU_RESONSE_NUM_WRONG			= -4,	// 返回接收组数不对
		ECX_SEND_DATA_FAILED				= -5,		// 发送数据失败
																	//	-4 0xFC
																	// -5 0xFB
																	// -6 0xFA
																	// -7 0xF9
																	// -8 0xF8
																	// -9 0xF7
		ECX_DOWN_BOARD_NO_RESPONSION		= -10, // -10 0xF6 下位机无应答	
		//结果类
		ECX_IO_VOLTAGE						= -11, // 0xF5 电压不匹配
		ECX_BUFFER_OVER						= -12, // 0xF4 缓冲区溢出
		ECX_INVALIDATION_PARAMETER			= -13, // 0xF3 无效参数
		ECX_FLASH_ERASE_FAILURE				= -14, // 0xF2 擦除FLASH失败
		ECX_FLASH_WRITE_PROTECTED			= -15, // 0xF1 写保护
		ECX_WRITE_FLASH_FAILURE				= -16, // 0xF0 写FLASH 失败
		ECX_RUN_INVALIDATION_PROGRAM_PAGE	= -17, // 0xEF 运行无效程序页面
		ECX_CAN_IO_ERROR					= -18, // 0xEE CAN OPEN ERROR
		ECX_CAN_SEND_ERROR					= -19, // 0xFD CAN SEND ERROR
		ECX_CAN_RECV_LEN_ERROR				= -20, // 0xFC 数据长度超长错误。
		ECX_CAN_NOANWSER_ERROR				= -21, // 0xFB 没有接受数据
		ECX_CAN_INVALIDATION_BITTIMING		= -22, // 0xFA 无效位时间设定
		EC_DATA_OVER						= -23, // 0xF9 数据溢出
		EC_NO_PULSE_FOUND					= -24, // 0xF8 没有期待的脉冲出现
		EC_NO_IDLESSE						= -25, // 0xF7 未找到空闲电平
		

		ECX_MAX_ERROR_CODE_NUMBER   	    = -26	// 最大异常定义数量    
	};
	
protected:
	short m_iErrorCode;
	string m_strOtherMessage;
	string m_strFunctionName;
	static string m_astrException[-ECX_MAX_ERROR_CODE_NUMBER];
	 
public:
	CEcuCommException();
	virtual ~CEcuCommException();
	CEcuCommException(short iErrorCode, string strOtherMessageString, string strFunctionName);

	virtual void SetExceptionMessage (short iErrorCode, string strOtherMessageString, string strFunctionName);
	virtual short GetErrorCode ();
#ifdef WIN32
	virtual unsigned short ReportError(unsigned short nType = adsMB_Cancel);
#endif

};
	
#endif // _CECUCOMMEXCEPTION_H
