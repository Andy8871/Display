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
	CRunLog * SystemLog; ////ƽ̨�ӿڼ�¼ָ��
	bool    RecordFlag;  ////�����Ƿ������ı�־
	bool    IsEnterInfo; ////�Ƿ��ǽ���ϵͳ��־
	char    *LogMessage; ////��¼ͨѶ����ȼ���
	int LogMessageLength;  ////��¼�ĳ���
	unsigned int  SystemTaskId;      ////����ϵͳ�˵�����ID
	bool    IsDataDrive;       ////������������־
	string  DataDriveFileName; ////���������ļ���
	bool    HideAutoLog;       ////ȡ���Զ���¼���ܱ�־

public:
	CAutoLog();
	~CAutoLog();
	void  InitAutoLog(unsigned int MenuTaskId); //�����������Զ�LOG��ʼ��,��Ҫ�ǰ�����˵�ID��ŵ�SystemTaskId����,ͬʱ��LogMessage��̬����2K�ռ�
	void  InitAutoLog(string ExcelFileName); //���������Զ�LOG��ʼ��,��Ҫ�ǰ�Э���ļ�����ŵ�DataDriveFileName����,ͬʱ��LogMessage��̬����2K�ռ�
	void  LogPrint();  //��¼��ǰ�����LogMessage�����������,���2K(�������2K,���ڽ���ϵͳ������Ǵ�����ȵ�,���ڹ���ģ�������Ǵ�����������)
	void  PrintString(string fmt);  //���Ի���ӡ�Զ��崮��Ϣ
	void  MakeSendReceiveFrame(CSendFrame SendFrame,CReceiveFrame ReceiveFrame); //��ŷ��ͽ���֡����
	void  MakeSendReceiveCmd(CBinary CmdFrame,bool IsSendCmd);  //��ŷ��ͻ����CBinary����
	void  MakeSendFrame(CSendFrame SendFrame);  //��ŷ���֡����
	void  MakeReceiveFrame(CReceiveFrame ReceiveFrame);  //��Ž���֡����
};

