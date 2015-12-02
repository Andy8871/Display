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

extern CCommWithEcu* g_pCommEcu; //全局通信接口
extern CDisplay * g_pDisplay; //全局显示接口
extern float g_flLibVersion;

#define VEHICLE_NAME	"OBDII"
#define DIAG_VERSION	12.0

/*typedef CDisplay *(*pGetDisplayObject)(void);
typedef CCommWithEcu *(*pGetCommWithEcuObject)(void);*/

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

	g_pDisplay = getDisplayObject();
	g_pCommEcu = getCommWithEcuObject();

	g_flLibVersion = CInformation::GetVersion();
	unsigned char nRet = g_pDisplay->Init(env);

	if (nRet == 0x01)
	{
		short iRet = -1;
		g_pCommEcu->Init();
		try
		{
			__android_log_write(ANDROID_LOG_DEBUG, "CDiagnosisFrame",
					"DiagnosisMain()");
			iRet = DiagnosisMain(); // 诊断程序入口函数，进行诊断功能

		} catch (CEcuCommException & e)
		{
			//e.ReportError();
		}
	}
	else
	{
			g_pCommEcu->Destroy();
			g_pDisplay->Destroy();
		return 1;
	}
// #endif	// #ifdef DATA_DRV_EXT
	/*CRunLog * Log = NULL;
	 Log = g_pCommEcu->GetRunLog();
	 if (Log != NULL)
	 Log->Close();
	 */
	g_pCommEcu->Destroy();
	g_pDisplay->Destroy();
	return 0;
}

/*void adsSleep(int iMilliseconds)
 {
 Sleep(iMilliseconds);
 }*/
