// WiFiComm.h: interface for the CWiFiComm class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "BaseComm.h"
#include <string>
using namespace std;

class CWiFiComm : public CComm  
{
public:
	/**
	 * Default constructor.
     */
	CWiFiComm();
	/**
	 * destructor.
     */
	virtual ~CWiFiComm();
public:
	/**
	 * access to a specified WLAN & connect to a specified server.
     */
	bool Init ( int iOpenMode );

	/**
	 * send data to server.
	 */
	int	Send (unsigned char* pBuffer, int iLength);

	/**
	 * receive data from server.
	 */
	int	Receive (unsigned char *pBuffer, int iLength);

	/**
	 * disconnect with the server & reset WLAN.
	 */
	void Close (void);

	/**
	 * clear the RX buffer.
	 */
	int ClearBuffer();
	bool TestLink(char * pCmdData, int iLength);

	bool ResetVDM(char * pData, int uiSize);

public:
	/**
	 * connect to VCI, here VCI is work as server
	 */
	int ConnectVDM();
	/**
	 * GET WIFI CONNECTION
	 */
	bool GetWifiConnetion();
	/**
	 * disconnect with VCI, here VCI is work as server
	 */
	long DisconnectVDM() const;

	/**
	 * Set ip address and port
	 */
	long SetIPAddress(const char *pIPAddress);

	/**
	 * Get ip address and port
	 */
	long GetIPAddress(char *pIPAddress);

/*
	//����VDMͨ��
	bool TestLink(unsigned char * pCmdData, int iLength);

	//ˢ��VDM�̼�
	bool ResetVDM(unsigned char * pData, unsigned int uiSize);*/

	//����VDM
	bool Reset(void);

	//ˢ��VDM
	bool Refresh(void);

	//����WIFI
	bool ResetWifi(void);

private:
	static string m_szServerIP;
	int m_socketfd;
	int m_nCount;
};
