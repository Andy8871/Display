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

//		ECX_OVERTIME=1,// ͨ�ų�ʱ        
		ECX_BREAK							= -2,	// ִ�б��ж�      
		ECX_ECU_NO_RESPONSION				= -3,	// ECU��Ӧ��       
		ECX_ECU_RESONSE_NUM_WRONG			= -4,	// ���ؽ�����������
		ECX_SEND_DATA_FAILED				= -5,		// ��������ʧ��
																	//	-4 0xFC
																	// -5 0xFB
																	// -6 0xFA
																	// -7 0xF9
																	// -8 0xF8
																	// -9 0xF7
		ECX_DOWN_BOARD_NO_RESPONSION		= -10, // -10 0xF6 ��λ����Ӧ��	
		//�����
		ECX_IO_VOLTAGE						= -11, // 0xF5 ��ѹ��ƥ��
		ECX_BUFFER_OVER						= -12, // 0xF4 ���������
		ECX_INVALIDATION_PARAMETER			= -13, // 0xF3 ��Ч����
		ECX_FLASH_ERASE_FAILURE				= -14, // 0xF2 ����FLASHʧ��
		ECX_FLASH_WRITE_PROTECTED			= -15, // 0xF1 д����
		ECX_WRITE_FLASH_FAILURE				= -16, // 0xF0 дFLASH ʧ��
		ECX_RUN_INVALIDATION_PROGRAM_PAGE	= -17, // 0xEF ������Ч����ҳ��
		ECX_CAN_IO_ERROR					= -18, // 0xEE CAN OPEN ERROR
		ECX_CAN_SEND_ERROR					= -19, // 0xFD CAN SEND ERROR
		ECX_CAN_RECV_LEN_ERROR				= -20, // 0xFC ���ݳ��ȳ�������
		ECX_CAN_NOANWSER_ERROR				= -21, // 0xFB û�н�������
		ECX_CAN_INVALIDATION_BITTIMING		= -22, // 0xFA ��Чλʱ���趨
		EC_DATA_OVER						= -23, // 0xF9 �������
		EC_NO_PULSE_FOUND					= -24, // 0xF8 û���ڴ����������
		EC_NO_IDLESSE						= -25, // 0xF7 δ�ҵ����е�ƽ
		

		ECX_MAX_ERROR_CODE_NUMBER   	    = -26	// ����쳣��������    
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
