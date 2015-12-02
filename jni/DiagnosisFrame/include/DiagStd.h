#ifndef __DIAG_STD__
#define  __DIAG_STD__

#include "adsStd.h"
#include "CommWithEcu.h"
#include "Display.h"
#include "EcuCommException.h"
#include "Binary.h"
#include "Database.h"
#include "SendFrame.h"
#include "ReceiveFrame.h"
#include "DiagnosisFrame.h"
#include "DiagApp.h"
#include "MenuTree.h"

extern	CCommWithEcu*	g_pCommEcu;	    //ȫ��ͨ�Žӿ�
extern	CDisplay	*	g_pDisplay; 	//ȫ����ʾ�ӿ�
extern float g_flLibVersion;

#endif