// Link.cpp : 定义 DLL 应用程序的导出函数。
//

#include "Linklayer.h"
#include "WiFiComm.h"

#define LINK_MODE_UNKNOWN	0x00
#define LINK_MODE_USB		0x01
#define LINK_MODE_WIFI		0x02

static CComm * m_pComm = NULL;

//自动匹配COM口
int CheckCommPort( void )
{
	int iPort = 0;

	return iPort;
}


CComm *  CreatInstance( int iLinkMode )
{
	int iCommPort = -1;
	WriteLog("CreateInstance");
	//未指定连接方式
	if ( LINK_MODE_UNKNOWN == iLinkMode )
	{
		iCommPort = CheckCommPort();		
		if ( iCommPort > 0 )
		{
			//m_pComm = new CSerialComm(iCommPort);
		}
		else
		{
			m_pComm = new CWiFiComm;
		}
	}

	//指定USB连接方式
	if ( LINK_MODE_USB == iLinkMode )
	{
		iCommPort = CheckCommPort();
		//m_pComm = new CSerialComm(iCommPort);
	}
	
	//指定WIFI连接方式
	if ( LINK_MODE_WIFI == iLinkMode )
	{
		m_pComm = new CWiFiComm;
	}

	return m_pComm;
}

