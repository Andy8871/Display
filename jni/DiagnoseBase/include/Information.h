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
		// 设置车系名称
		static void SetVehiclesSystemName(char *pVehicleSystemName);

		// 获取车系名称
		static string GetVehiclesSystemName();

		// 设置版本信息
		static void SetVersion(double fVersion);

		// 获取版本信息
		static double GetVersion();

		// 获取设备名称
		static string GetEquipmentName();

		// 获取车系ID
		static unsigned short GetVehicleID();

		// 设置车系ID
		static void SetVehicleID(unsigned short wVehicleID);
};
	
#endif // _INFORMATION_h
