// Link.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "Linklayer.h"
#include "WiFiComm.h"

#define LINK_MODE_UNKNOWN	0x00
#define LINK_MODE_USB		0x01
#define LINK_MODE_WIFI		0x02

static CComm * m_pComm = NULL;

//�Զ�ƥ��COM��
int CheckCommPort( void )
{
	int iPort = 0;

	return iPort;
}


CComm *  CreatInstance( int iLinkMode )
{
	int iCommPort = -1;
	WriteLog("CreateInstance");
	//δָ�����ӷ�ʽ
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

	//ָ��USB���ӷ�ʽ
	if ( LINK_MODE_USB == iLinkMode )
	{
		iCommPort = CheckCommPort();
		//m_pComm = new CSerialComm(iCommPort);
	}
	
	//ָ��WIFI���ӷ�ʽ
	if ( LINK_MODE_WIFI == iLinkMode )
	{
		m_pComm = new CWiFiComm;
	}

	return m_pComm;
}

