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

extern	CCommWithEcu*	g_pCommEcu;	    //全局通信接口
extern	CDisplay	*	g_pDisplay; 	//全局显示接口
extern float g_flLibVersion;

#endif