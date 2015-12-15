#include <android/log.h>
#include "CommWithEcu.h"
#include "Display.h"
#include "Warrant.h"
#include "DiagnosisFrame.h"
#include "DiagApp.h"
#include "EcuCommException.h"
#include "Information.h"

#include "RunEnvironment.h"
#include "Binary.h"

extern float g_flLibVersion;

#define VEHICLE_NAME	"OBDII"
#define DIAG_VERSION	12.0

int DiagnosisMain();

CDiagnosisFrame::CDiagnosisFrame(void)
{
	CDiagApp::m_pMainApplication = NULL;
}

CDiagnosisFrame::~CDiagnosisFrame(void)
{
}

int CDiagnosisFrame::InitInstance(JNIEnv* env)
{
	CInformation::SetVehiclesSystemName(VEHICLE_NAME);
	CInformation::SetVersion(DIAG_VERSION);
	g_flLibVersion = CInformation::GetVersion();
	unsigned char nRet = CDisplay::GetInstance()->Init(env);

	if (nRet == 0x01)
	{
		short iRet = -1;
		CCommWithEcu::GetInstance()->Init();
		try
		{
			iRet = DiagnosisMain(); // 诊断程序入口函数，进行诊断功能

		} catch (CEcuCommException & e)
		{
			//e.ReportError();
		}
	}
	else
	{
		CCommWithEcu::GetInstance()->Destroy();
		CDisplay::GetInstance()->Destroy();
		return 1;
	}
// #endif	// #ifdef DATA_DRV_EXT
	/*CRunLog * Log = NULL;
	 Log = g_pCommEcu->GetRunLog();
	 if (Log != NULL)
	 Log->Close();
	 */
	CCommWithEcu::GetInstance()->Destroy();
	CDisplay::GetInstance()->Destroy();
	return 0;
}

/*void adsSleep(int iMilliseconds)
 {
 Sleep(iMilliseconds);
 }*/
