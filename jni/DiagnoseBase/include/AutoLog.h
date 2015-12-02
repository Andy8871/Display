#include "stdafx.h"
#include "RunEnvironment.h"
#include "runlog.h"
#include "Display.h"
#include "adsStd.h"
#include "Binary.h"
#include "information.h"
#include "CommWithEcu.h"

class CAutoLog
{
protected:

public:
	CRunLog * SystemLog; ////平台接口记录指针
	bool    RecordFlag;  ////功能是否正常的标志
	bool    IsEnterInfo; ////是否是进入系统标志
	char    *LogMessage; ////记录通讯命令等集合
	int LogMessageLength;  ////记录的长度
	unsigned int  SystemTaskId;      ////车型系统菜单事务ID
	bool    IsDataDrive;       ////数据驱动与否标志
	string  DataDriveFileName; ////数据驱动文件名
	bool    HideAutoLog;       ////取消自动记录功能标志

public:
	CAutoLog();
	~CAutoLog();
	void  InitAutoLog(unsigned int MenuTaskId); //非数据驱动自动LOG初始化,主要是把事务菜单ID存放到SystemTaskId参数,同时给LogMessage动态分配2K空间
	void  InitAutoLog(string ExcelFileName); //数据驱动自动LOG初始化,主要是把协议文件名存放到DataDriveFileName参数,同时给LogMessage动态分配2K空间
	void  LogPrint();  //记录当前存放在LogMessage里的数据内容,最多2K(如果超过2K,属于进入系统的情况是存放最先的,属于功能模块的情况是存放最近的数据)
	void  PrintString(string fmt);  //个性化打印自定义串信息
	void  MakeSendReceiveFrame(CSendFrame SendFrame,CReceiveFrame ReceiveFrame); //存放发送接收帧数据
	void  MakeSendReceiveCmd(CBinary CmdFrame,bool IsSendCmd);  //存放发送或接收CBinary数据
	void  MakeSendFrame(CSendFrame SendFrame);  //存放发送帧数据
	void  MakeReceiveFrame(CReceiveFrame ReceiveFrame);  //存放接收帧数据
};

