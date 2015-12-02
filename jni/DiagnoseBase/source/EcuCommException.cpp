/*************************************************************
  Copyright (c) 2006, AUTOBOSS Ltd,.Co 
  All rights reserved.
 
  �ļ����ƣ�EcuCommException.h
  �ĵ���ʶ��ͨ���쳣��Ϣ��
  ժ    Ҫ��

  ��ʷ��¼��
  ----------------------------------------------------------------------------
  ʱ     ��   ����   �汾��	  ��    ��    ��    ��                               
  ----------------------------------------------------------------------------
  2006.6.1    fufs  1.0      ��������
  2006.11.10  wcx            �����������쳣����

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

//�����
	"IO_VOLTAGE",							// -11	0xF5 ��ѹ��ƥ��
	"BUFFER_OVER",							// -12	0xF4 ���������
	"INVALIDATION PARAMETER",				// -13 0xF3 ��Ч����
	"FLASH ERASE FAILURE",					// -14  0xF2 ����FLASHʧ��
	"FLASH WRITE PROTECTED",				// -15 0xF1 д����
	"WRITE_FLASH_FAILURE",					// -16 0xF0 дFLASH ʧ��
	"RUN INVALIDATION PROGRAM PAGE",		// -17 0xEF ������Ч����ҳ��
	"CAN IO ERROR",							// -18 0xEE CAN OPEN ERROR
	"CAN SEND ERROR",						// -19 0xFD CAN SEND ERROR
	"CAN RECV LEN ERROR",					// -20 0xFC ���ݳ��ȳ�������
	"CAN NO ANWSER ERROR",					// -21 0xFB û�н�������
	"CAN INVALIDATION BIT TIMING",			// -22 0xFA ��Чλʱ���趨
	"DATA OVER",							//= -23, // 0xF9 �������
	"NO PULSE FOUND",						//= -24, // 0xF8 û���ڴ����������
	"NO IDLESSE"							//= -25, // 0xF7 δ�ҵ����е�ƽ

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
  ��    �ܣ������쳣���뼰������ 
  ����˵����unsigned short iErrorCode �쳣����
			string strFunctionName �׳��쳣�ĺ�����
			string strOtherMessageString ���ӵ�˵��
  �� �� ֵ����
  ˵    ������
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
  ��    �ܣ������쳣���� 
  ����˵������
  �� �� ֵ���������
  ˵    ������
-----------------------------------------------------------------------------*/
short CEcuCommException::GetErrorCode ()
{
	return m_iErrorCode;
}

#ifdef WIN32 
/*-----------------------------------------------------------------------------
  ��    �ܣ���DEBUG�½��׳���ǰ�쳣�ĺ��������쳣���롢�������������ͨ��������
            ʾ�ӿڵ�adsMessageBox()������ʾ��������RELEASE�汾�²����κβ����� 
  ����˵����unsigned short nType,ΪadsMessageBox()�İ�ť���ͣ�ȱʡΪadsMB_Cancel 
  �� �� ֵ��adsMessageBox()�ķ���ֵID��RELEASE�汾�·���adsIDCANCEL
  ˵    ������
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
