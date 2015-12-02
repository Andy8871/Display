#ifndef _INFORMATION_h
#define _INFORMATION_h

#include "adsStd.h"

#include <string>
using namespace std;

class CInformation
{
protected:
		static string m_strVehicleSystemName;
		static double m_fVersion;
		static string m_strEquipmentName;
		static unsigned short   m_iVehicleID;
public:
		// ���ó�ϵ����
		static void SetVehiclesSystemName(char *pVehicleSystemName);

		// ��ȡ��ϵ����
		static string GetVehiclesSystemName();

		// ���ð汾��Ϣ
		static void SetVersion(double fVersion);

		// ��ȡ�汾��Ϣ
		static double GetVersion();

		// ��ȡ�豸����
		static string GetEquipmentName();

		// ��ȡ��ϵID
		static unsigned short GetVehicleID();

		// ���ó�ϵID
		static void SetVehicleID(unsigned short wVehicleID);
};
	
#endif // _INFORMATION_h
