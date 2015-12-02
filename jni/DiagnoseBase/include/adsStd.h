/***************************************************************************//**
 * \file		adsStd.h
 * \brief	used as global header fot the whole DiagnosisBaseClass library
 * \author
 *******************************************************************************/

#ifndef __I386_TYPES_H_
#define __I386_TYPES_H_

#undef DATA_DRV_EXT
#define DATA_DRV_EXT	// wrap extended contents specially for data driven task
// 2010-01-04 18:49:18	James Zhang

#pragma  warning(disable:4786)
#include <string>
// #include <sstream>	seems not support under V30's ARM compiler (under WinCE)

#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
using namespace std;

#ifdef DATA_DRV_EXT
#include <stdlib.h>
#ifdef WIN32
#include <xstring>//avoid for ios
#include <windows.h>//avoid for ios
#include "SpxGasLibError.h"	// make use of its macro mechanism
#include "DataCommon.h"		// make use of its debug/log output mechanism
#endif
// #include <tchar.h>	// to use _T

//#include "System_Resouce_Header.h"

#define BT_FF					((unsigned char)~0)
#ifndef DW_FFFFFFFF
#define DW_FFFFFFFF				((unsigned long)~0)
#endif	// #ifndef DW_FFFFFFFF
#define  DATA_INVALIDATION			0			//added by bin
#define SZ_EMPTY				""
#define SZ_BS					"\\"			// back slash
#define SZ__					"_"
#define SZ_DOT_DAT				".dat"
#define SZ_WS					" "

// #define SCRIPT_NO_FIGS		6	/** script no.(dec) has 6 figures at most */
// #define SCRIPT_NO_UPBOUND	1000000

// pre-defined invalid unsigned long value
#define INVALID_DWORD		DW_FFFFFFFF

// pre-defined file header length
#define DATA_DRV_FILE_HEAD_LEN	64

#endif	// #ifdef DATA_DRV_EXT
#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif



#define MAX_PATH	260

#define ID_BINARY_LENTH 6

//������붨��
#define EC_SUCCESS						0 	//ִ�гɹ�
//ͨ����
#define EC_OVERTIME						-1	//ͨ�ų�ʱ
#define EC_BREAK						-2	//ִ�б��ж�
#define EC_ECU_NO_RESPONSION			-3	//ECU��Ӧ��
#define EC_ECU_RESONSE_NUM_WRONG		-4	//���ؽ�����������
#define EC_SEND_DATA_FAILED     		-5	//��������ʧ��
#define EC_DOWN_BOARD_NO_RESPONSION		-10	//��λ����Ӧ��
//�����
#define EC_IO_VOLTAGE_MATCH             -11	// ��ѹ��ƥ��
//ע����IO�ڵ�ѹΪ5Vʱ�����Ե��趨ʹ��12V��ƽͨ�������ָô�����룻��֮��Ȼ��
#define EC_BUFFER_OVER					-12	//���������
//������
#define EC_ID_NUMBER					-101	//IDλ����ƥ��
#define EC_PARAMETER					-102	//��Ч����
//	ע��������ֵȡֵ��Χ���Ϸ�

#define EC_DATA_FORMAT					-103	//���ݸ�ʽ��
//ע����CBinary��׼ID����Ϊ6���ֽڣ���ò����в�Ϊ6λ�򱨴˴�

#define EC_DB_DATA_STREAM               -104    // ����������û�ҵ�
#define EC_TYPE_DATA_STREAM             -105    // ���������ʽ��һ��
#define EC_ORDER_DATA_STREAM            -106    // ��������������������������ݺ�������Ӱ�Ŧ
// ������������
#define DATASTEAM_ERRORDSITEM			-1	//��������������
#define	DATASTEAM_NOEXPRESS		    	-2	//�������Ĺ�ʽ���ʽȱʧ
#define DATASTEAM_NOFORMAT		     	-3	//�������ĸ�ʽ�����ַ���ȱʧ

// ����ѡ����

#define EC_OVER_ITEM_NUM                -108    // ���õı�ѡ����Ŵ���ѡ������
#define EC_OVER_MASK_LEN                -109    // ���볤�Ȳ���

#define  CHECKSTRING(x)	((x).length() == 0 ? false:true)
#define  CHECKDATA(x)	(((x) == DATA_INVALIDATION || !CHECKADDR(x)) ? false:true)

void adsSleep(int iMilliseconds);

#endif	//	__I386_TYPES_H_
