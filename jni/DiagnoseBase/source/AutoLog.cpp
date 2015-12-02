#include "AutoLog.h"


CAutoLog::CAutoLog()
{
	LogMessageLength=0;
	SystemLog=NULL;
	RecordFlag=false;
	IsEnterInfo=true;
	SystemTaskId=0;
	IsDataDrive=false;
	DataDriveFileName="";
	
	HideAutoLog=true;//HideAutoLog=false;

	/////�����ǰ����LOG����״̬,�����ڴ�
	//if(CRunEnvironment::GetCheckMode()==false)
	//{
	//	LogMessage=new char[2048];
	//	memset(LogMessage,0,sizeof(LogMessage));
	//}
}

CAutoLog::~CAutoLog()
{
	/////�����ǰ����LOG����״̬,�����ڴ�
	if((CRunEnvironment::GetCheckMode()==false)&&(HideAutoLog==false)&&(CRunEnvironment::GetDemoMode()==false))
	{
		delete []LogMessage;
		
		if(SystemLog!=NULL)
			delete SystemLog;
			
		SystemLog = NULL;		
	}	
}

/*********************************************************************************************************
��ʼ������
***********************************************************************************************************/
void  CAutoLog::InitAutoLog(unsigned int MenuTaskId)
{
	//memset(LogMessage,0,sizeof(LogMessage));			
	//LogMessageLength=0;
	//SystemLog=NULL;
	//RecordFlag=false;
	//IsEnterInfo=true;
	SystemTaskId=MenuTaskId;

	if((CRunEnvironment::GetCheckMode()==false)&&(HideAutoLog==false)&&(CRunEnvironment::GetDemoMode()==false))
	{
		LogMessage=new char[2048];
		memset(LogMessage,0,sizeof(LogMessage));
	}

	//memset(LogMessage,0,sizeof(LogMessage));
}

/*********************************************************************************************************
��ʼ������
***********************************************************************************************************/
void  CAutoLog::InitAutoLog(string ExcelFileName)
{
	//memset(LogMessage,0,sizeof(LogMessage));			
	//LogMessageLength=0;
	//SystemLog=NULL;
	//RecordFlag=false;
	//IsEnterInfo=true;
	//SystemTaskId=MenuTaskId;
	DataDriveFileName=ExcelFileName;

	if((CRunEnvironment::GetCheckMode()==false)&&(HideAutoLog==false)&&(CRunEnvironment::GetDemoMode()==false))
	{
		LogMessage=new char[2048];
		memset(LogMessage,0,sizeof(LogMessage));
	}

	//memset(LogMessage,0,sizeof(LogMessage));
}

/*********************************************************************************************************
��������:PrintString(string fmt,bool PrintMenu,bool IsEnterInfo,bool SaveFile)
����:fmt  string ����,ָ���Ǵ����뱣�������,������Ҫ�ǿ��Ÿ�Ҫ��ӡһЩ�Զ����ַ���,��Ϊ�ر��ע
     PrintMenu  bool����,��ʶ�Ƿ�Ҫ����ǰ�Ĳ˵���ϸ·��һ���ӡ����
	 IsEnterInfo bool����,��ʶ������Ƿ��ǽ�����Ϣ
	 SaveFile bool����,��ʶ�Ƿ��ڱ����굱ǰ��Ϣ��,���Ϲرձ��������ļ�,���ΪFALSE,�����Ŀ�׷��,���ΪTRUE
	                   �򱻹ر�,�´��������ݱ�����������µ��ļ�.

����:��ӡ�Զ����ַ�����Ϣ���ļ�����

����ֵ:void
����:Marco Lin
���ν���ʱ��:2011-12-21     
�޸ļ�¼:

***********************************************************************************************************/
void  CAutoLog::PrintString(string fmt)
{
	char    SystemFileName[128];
	char    FileNameBuff[50];
	int i=0,j=0;
	short StartPos=0;
	short EndPos=0;
	unsigned char    iCount=0;

	/////�����ǰ�Ѿ���LOG����״̬
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////����
	if(HideAutoLog==true)
		return ;

	/////�����ǰ�Ѿ���DEMO����״̬
	if(CRunEnvironment::GetDemoMode()==true)
		return ;

	
	memset(SystemFileName,0,sizeof(SystemFileName));
	memset(FileNameBuff,0,sizeof(FileNameBuff));

	if(IsDataDrive)
	{
		for(i=0,j=0;i<DataDriveFileName.size();i++)
		{
			if(iCount==2)
			{
				StartPos=i;
				iCount++;
			}
			
			if(DataDriveFileName[i]=='_')
			{
				iCount++;
				//EndPos=i;
				//EndPos--;
			}
			
			EndPos=i;

			////�ж��EXCEL�ļ���ʱ��,ֻȡ��һ���ļ���
			if(DataDriveFileName[i]==',')
			{
				EndPos--;
				break;
			}
		}	

		if((StartPos>0)&&(EndPos>0)&&(EndPos>StartPos))
		{
			for(i=StartPos,j=0;i<=EndPos;i++)
			{
				FileNameBuff[j++]=DataDriveFileName[i];
			}	
			FileNameBuff[j]='\0';
		}
	}


	//////���CRUNLOGָ��δ����,Ҳ����δ�����ļ�,�����������Ϣ
	if(SystemLog==NULL)
	{		
		//////��ǰ�������й���,�ļ���Ҫ�ں����BUG��ʶ
		if(RecordFlag==true)
		{
			memset(SystemFileName,0,sizeof(SystemFileName));
			sprintf(SystemFileName,"%c_",CRunEnvironment::GetSerialNumber()[0]);
			for(i=0;i<CInformation::GetVehiclesSystemName().size();i++)
				sprintf(SystemFileName+strlen(SystemFileName),"%c",CInformation::GetVehiclesSystemName()[i]);
			
			sprintf(SystemFileName+strlen(SystemFileName),"_V%.2f_",CInformation::GetVersion());
			sprintf(SystemFileName+strlen(SystemFileName),"%c",(CRunEnvironment::GetLanguage())[0]);
			sprintf(SystemFileName+strlen(SystemFileName),"%c_",(CRunEnvironment::GetLanguage())[1]);
			if(IsDataDrive)
			{
				if(strlen(FileNameBuff)!=0)
					sprintf(SystemFileName+strlen(SystemFileName),"%s_",FileNameBuff);
				else
					sprintf(SystemFileName+strlen(SystemFileName),"%s_","0000");
			}
			else
				sprintf(SystemFileName+strlen(SystemFileName),"%08X_",SystemTaskId);

			time_t t = time(0);
			tm* systm = localtime(&t);
			sprintf(SystemFileName+strlen(SystemFileName),"%02d%02d%02d_%02d%02d%02d_BUG",((systm->tm_year>2000)?(systm->tm_year-2000):systm->tm_year),systm->tm_mon,systm->tm_mday,systm->tm_hour,systm->tm_min,systm->tm_sec);

			strcat(SystemFileName,".txt");		
			SystemLog = new CRunLog;
			if(SystemLog->Init(SystemFileName)==false)////��ʼ��CRUNLOG,����SystemBugFileName����TXT�ļ�
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
		}

		//////�������ϵͳ,Ҳû������,�����ļ�������չ��
		else if(IsEnterInfo==true)
		{
			memset(SystemFileName,0,sizeof(SystemFileName));
			sprintf(SystemFileName,"%c_",CRunEnvironment::GetSerialNumber()[0]);
			for(i=0;i<CInformation::GetVehiclesSystemName().size();i++)
				sprintf(SystemFileName+strlen(SystemFileName),"%c",CInformation::GetVehiclesSystemName()[i]);
			
			sprintf(SystemFileName+strlen(SystemFileName),"_V%.2f_",CInformation::GetVersion());
			sprintf(SystemFileName+strlen(SystemFileName),"%c",(CRunEnvironment::GetLanguage())[0]);
			sprintf(SystemFileName+strlen(SystemFileName),"%c_",(CRunEnvironment::GetLanguage())[1]);
			if(IsDataDrive)
			{
				if(strlen(FileNameBuff)!=0)
					sprintf(SystemFileName+strlen(SystemFileName),"%s_",FileNameBuff);
				else
					sprintf(SystemFileName+strlen(SystemFileName),"%s_","0000");
			}
			else
				sprintf(SystemFileName+strlen(SystemFileName),"%08X_",SystemTaskId);

			time_t t = time(0);
			tm* systm = localtime(&t);
			sprintf(SystemFileName+strlen(SystemFileName),"%02d%02d%02d_%02d%02d%02d",((systm->tm_year>2000)?(systm->tm_year-2000):systm->tm_year),systm->tm_mon,systm->tm_mday,systm->tm_hour,systm->tm_min,systm->tm_sec);
			strcat(SystemFileName,".txt");		
			SystemLog = new CRunLog;
			if(SystemLog->Init(SystemFileName)==false)////��ʼ��CRUNLOG,����SystemBugFileName����TXT�ļ�
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
		}			
		

		/////�����ǰϵͳû����,Ҳ�ǽ���ϵͳ���ݱ���,�ļ���Ҫ�ں����OK��ʶ
		else
		{
			memset(SystemFileName,0,sizeof(SystemFileName));
			sprintf(SystemFileName,"%c_",CRunEnvironment::GetSerialNumber()[0]);
			for(i=0;i<CInformation::GetVehiclesSystemName().size();i++)
				sprintf(SystemFileName+strlen(SystemFileName),"%c",CInformation::GetVehiclesSystemName()[i]);
			
			sprintf(SystemFileName+strlen(SystemFileName),"_V%.2f_",CInformation::GetVersion());
			sprintf(SystemFileName+strlen(SystemFileName),"%c",(CRunEnvironment::GetLanguage())[0]);
			sprintf(SystemFileName+strlen(SystemFileName),"%c_",(CRunEnvironment::GetLanguage())[1]);
			if(IsDataDrive)
			{
				if(strlen(FileNameBuff)!=0)
					sprintf(SystemFileName+strlen(SystemFileName),"%s_",FileNameBuff);
				else
					sprintf(SystemFileName+strlen(SystemFileName),"%s_","0000");
			}
			else
				sprintf(SystemFileName+strlen(SystemFileName),"%08X_",SystemTaskId);

			time_t t = time(0);
			tm* systm = localtime(&t);
			sprintf(SystemFileName+strlen(SystemFileName),"%02d%02d%02d_%02d%02d%02d_OK",((systm->tm_year>2000)?(systm->tm_year-2000):systm->tm_year),systm->tm_mon,systm->tm_mday,systm->tm_hour,systm->tm_min,systm->tm_sec);
			strcat(SystemFileName,".txt");		
			SystemLog = new CRunLog;
			if(SystemLog->Init(SystemFileName)==false)////��ʼ��CRUNLOG,����SystemBugFileName����TXT�ļ�
			{
				delete SystemLog;
				SystemLog = NULL;				
				return ;
			}
		}
		
	}

	/////////////��ӡ��ǰ·���˵�
	//if(PrintMenu==true)
	//	SystemLog->WriteMenu();

	////////////��ӡ���ݵ��ļ�
	SystemLog->WriteContent(fmt,true);

	//////////�������Ͳ��ǽ���,��ÿ�α������Ҫ��ջ���
	//if(IsEnterInfo==false)
	//{
	//	memset(LogMessage,0,sizeof(LogMessage));
	//	LogMessageLength=0;
	//}	
	///////////�����Ҫ���ϱ��沢�ر��ļ�,�ͷŵ�CRUNLOG
	//if(SaveFileNow==true)
	//{
	//	delete SystemLog;
	//	SystemLog = NULL;
	//}
}



/*********************************************************************************************************
��������:LogPrint(char *fmt,bool IsEnterInfo)
����:fmt  char * ����,ָ���Ǵ����뱣�������,ֻ���ǻ���������LogEnterMessage��LogRecordMessage
	 IsEnterInfo bool����,��ʶ������Ƿ��ǽ�����Ϣ


����:��ӡ����Ҫ��¼��������Ϣ���ļ�����

����ֵ:void
����:Marco Lin
���ν���ʱ��:2011-12-21     
�޸ļ�¼:

***********************************************************************************************************/
void  CAutoLog::LogPrint()
{
	//CBinary FMT;
	int i=0,j=0,k=0;
	char *temp;
	//CBinary SubFmt;
	int len=0;
	int SubFmtLen=0;
	char    SystemFileName[128];
	int pos=0;
	char    FileNameBuff[128];
	short StartPos=0;
	short EndPos=0;
	unsigned char    iCount=0;

	memset(SystemFileName,0,sizeof(SystemFileName));
	memset(FileNameBuff,0,sizeof(FileNameBuff));


	/////�����ǰ�Ѿ���LOG����״̬
	if(CRunEnvironment::GetCheckMode()==true)
		return ;
	
	////����
	if(HideAutoLog==true)
		return ;
	
	/////�����ǰ�Ѿ���DEMO����״̬
	if(CRunEnvironment::GetDemoMode()==true)
		return ;

	/////�жϴ��������Ƿ�Ϊ��
	if(LogMessageLength<=0)
		return ;

	/////�жϴ�������
	if(LogMessageLength>2048)
		return ;




	if(IsDataDrive)
	{
		for(i=0,j=0;i<DataDriveFileName.size();i++)
		{
			if(iCount==2)
			{
				StartPos=i;
				iCount++;
			}
			
			if(DataDriveFileName[i]=='_')
			{
				iCount++;
				//EndPos=i;
				//EndPos--;
			}
			EndPos=i;

			////�ж��EXCEL�ļ���ʱ��,ֻȡ��һ���ļ���
			if(DataDriveFileName[i]==',')
			{
				EndPos--;
				break;
			}
		}	

		if((StartPos>0)&&(EndPos>0)&&(EndPos>StartPos))
		{
			for(i=StartPos,j=0;i<=EndPos;i++)
			{
				FileNameBuff[j++]=DataDriveFileName[i];
			}	
			FileNameBuff[j]='\0';
		}
	}
	/////�����ǰ�Ѿ���LOG����״̬
	//if(CRunEnvironment::GetCheckMode()==true)
	//	return ;
	
	//memset(temp,0,sizeof(temp));

	//temp=new char[LogMessageLength];
	//for(i=0;i<LogMessageLength;i++)
	//	temp[i]=LogMessage[i];	

	//FMT.WriteBuffer(temp,LogMessageLength);
	//delete []temp;/////�ͷ�TEMP�ڴ�
	
	//////���CRUNLOGָ��δ����,Ҳ����δ�����ļ�,�����������Ϣ
	if(SystemLog==NULL)
	{		
		//////��ǰ�������й���,�ļ���Ҫ�ں����BUG��ʶ
		if(RecordFlag==true)
		{
			memset(SystemFileName,0,sizeof(SystemFileName));
			sprintf(SystemFileName,"%c_",CRunEnvironment::GetSerialNumber()[0]);
			for(i=0;i<CInformation::GetVehiclesSystemName().size();i++)
				sprintf(SystemFileName+strlen(SystemFileName),"%c",CInformation::GetVehiclesSystemName()[i]);
			
			sprintf(SystemFileName+strlen(SystemFileName),"_V%.2f_",CInformation::GetVersion());
			sprintf(SystemFileName+strlen(SystemFileName),"%c",(CRunEnvironment::GetLanguage())[0]);
			sprintf(SystemFileName+strlen(SystemFileName),"%c_",(CRunEnvironment::GetLanguage())[1]);
			if(IsDataDrive)
			{
				if(strlen(FileNameBuff)!=0)
					sprintf(SystemFileName+strlen(SystemFileName),"%s_",FileNameBuff);
				else
					sprintf(SystemFileName+strlen(SystemFileName),"%s_","0000");
			}
			else
				sprintf(SystemFileName+strlen(SystemFileName),"%08X_",SystemTaskId);
			
			time_t t = time(0);
			tm* systm = localtime(&t);
			sprintf(SystemFileName+strlen(SystemFileName),"%02d%02d%02d_%02d%02d%02d_BUG",((systm->tm_year>2000)?(systm->tm_year-2000):systm->tm_year),systm->tm_mon,systm->tm_mday,systm->tm_hour,systm->tm_min,systm->tm_sec);
			strcat(SystemFileName,".txt");		
			SystemLog = new CRunLog;
			if(SystemLog->Init(SystemFileName)==false)////��ʼ��CRUNLOG,����SystemBugFileName����TXT�ļ�
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
			//SystemLog->SetBufSize(4080);
		}

		//////�������ϵͳ,Ҳû������,�����ļ�������չ��
		else if(IsEnterInfo==true)
		{
			memset(SystemFileName,0,sizeof(SystemFileName));
			sprintf(SystemFileName,"%c_",CRunEnvironment::GetSerialNumber()[0]);
			for(i=0;i<CInformation::GetVehiclesSystemName().size();i++)
				sprintf(SystemFileName+strlen(SystemFileName),"%c",CInformation::GetVehiclesSystemName()[i]);
			
			sprintf(SystemFileName+strlen(SystemFileName),"_V%.2f_",CInformation::GetVersion());
			sprintf(SystemFileName+strlen(SystemFileName),"%c",(CRunEnvironment::GetLanguage())[0]);
			sprintf(SystemFileName+strlen(SystemFileName),"%c_",(CRunEnvironment::GetLanguage())[1]);
			if(IsDataDrive)
			{
				if(strlen(FileNameBuff)!=0)
					sprintf(SystemFileName+strlen(SystemFileName),"%s_",FileNameBuff);
				else
					sprintf(SystemFileName+strlen(SystemFileName),"%s_","0000");
			}
			else
				sprintf(SystemFileName+strlen(SystemFileName),"%08X_",SystemTaskId);

			time_t t = time(0);
			tm* systm = localtime(&t);
			sprintf(SystemFileName+strlen(SystemFileName),"%02d%02d%02d_%02d%02d%02d",((systm->tm_year>2000)?(systm->tm_year-2000):systm->tm_year),systm->tm_mon,systm->tm_mday,systm->tm_hour,systm->tm_min,systm->tm_sec);
			strcat(SystemFileName,".txt");		
			SystemLog = new CRunLog;
			if(SystemLog->Init(SystemFileName)==false)////��ʼ��CRUNLOG,����SystemBugFileName����TXT�ļ�
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
			//SystemLog->SetBufSize(4080);
		}			
		

		/////�����ǰϵͳû����,Ҳ�ǽ���ϵͳ���ݱ���,�ļ���Ҫ�ں����OK��ʶ
		else
		{
			memset(SystemFileName,0,sizeof(SystemFileName));
			sprintf(SystemFileName,"%c_",CRunEnvironment::GetSerialNumber()[0]);
			for(i=0;i<CInformation::GetVehiclesSystemName().size();i++)
				sprintf(SystemFileName+strlen(SystemFileName),"%c",CInformation::GetVehiclesSystemName()[i]);
			
			sprintf(SystemFileName+strlen(SystemFileName),"_V%.2f_",CInformation::GetVersion());
			sprintf(SystemFileName+strlen(SystemFileName),"%c",(CRunEnvironment::GetLanguage())[0]);
			sprintf(SystemFileName+strlen(SystemFileName),"%c_",(CRunEnvironment::GetLanguage())[1]);
			if(IsDataDrive)
			{
				if(strlen(FileNameBuff)!=0)
					sprintf(SystemFileName+strlen(SystemFileName),"%s_",FileNameBuff);
				else
					sprintf(SystemFileName+strlen(SystemFileName),"%s_","0000");
			}
			else
				sprintf(SystemFileName+strlen(SystemFileName),"%08X_",SystemTaskId);

			time_t t = time(0);
			tm* systm = localtime(&t);
			sprintf(SystemFileName+strlen(SystemFileName),"%02d%02d%02d_%02d%02d%02d_OK",((systm->tm_year>2000)?(systm->tm_year-2000):systm->tm_year),systm->tm_mon,systm->tm_mday,systm->tm_hour,systm->tm_min,systm->tm_sec);
			strcat(SystemFileName,".txt");		
			SystemLog = new CRunLog;
			if(SystemLog->Init(SystemFileName)==false)////��ʼ��CRUNLOG,����SystemBugFileName����TXT�ļ�
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
			//SystemLog->SetBufSize(4080);
		}

	}


	////һЩ������Ϣ
	SystemLog->WriteContent("SerialNumber:"+CRunEnvironment::GetSerialNumber());
	SystemLog->WriteContent("DisplayVersion:"+CRunEnvironment::GetDisplayVersion());
	if(CRunEnvironment::GetNewMcuFlag()==true)
		SystemLog->WriteContent("NewMcuFlag:true");
	else
		SystemLog->WriteContent("NewMcuFlag:false");
	SystemLog->WriteContent("----------------------------------------------------------------------------------------");
	SystemLog->WriteContent(" ",true);////����
	
	/////////////��ӡ��ǰ·���˵�
	SystemLog->WriteMenu();
	
	//////��Ҫ��ӡ�Ĵ�����FMT,���ݱ�ʶ��55 aa,55 bbһ��һ���Ļ��д�ӡ���ļ���
	
	for(i=0;i<LogMessageLength;i++)
	{
		if(((unsigned char)LogMessage[i]==0x55)&&(((unsigned char)LogMessage[i+1]==0xaa)||((unsigned char)LogMessage[i+1]==0xbb)))
		{
			len=(unsigned char)LogMessage[i+2];
			pos=i;

			/////�ж��䳤���Ƿ�Ϊ��,Ϊ�ղ���¼,����255�쳣Ҳ����¼
			if((len<=0)||(len>255))
				continue;
				
			//temp=new char[len];
			//for(j=0;j<len;j++)
			//{
			//	temp[j]=FMT.GetAt(i);
			//	i++;
			//}
			//i--;

			//SubFmt.WriteBuffer(temp,len);////�Ӵ�
			//delete []temp;/////�ͷ�TEMP�ڴ�


			SubFmtLen=0;
			temp=new char[len*3];
			//if((SubFmt.GetAt(0)==(unsigned char)0x55)&&(SubFmt.GetAt(1)==(unsigned char)0xaa))
			if(((unsigned char)LogMessage[i]==0x55)&&((unsigned char)LogMessage[i+1]==0xaa))
				SubFmtLen+=sprintf(temp+SubFmtLen,"%s","Req:");
			else
				SubFmtLen+=sprintf(temp+SubFmtLen,"%s","Ans:");

			for(i=pos+3;i<len+pos;i++)
			{
				SubFmtLen+=sprintf(temp+SubFmtLen," %02X",(unsigned char)LogMessage[i]);
			}
			temp[SubFmtLen]='\0';
			i--;
			
			SystemLog->WriteContent(temp);////��ӡ����
			//memset(temp,0,sizeof(temp));
			//SubFmt.FreeExtra();/////�ͷ�CBINARY	
			delete []temp;/////�ͷ�TEMP�ڴ�
		}
	}
		
	SystemLog->WriteContent(" ",true);////����
	
	/////ÿ�δ�ӡ��������Ϣ��Ҫ����ӡ��¼��λ
	RecordFlag=false;
	IsEnterInfo=false;
	//SubFmt.FreeExtra();/////�ͷ�CBINARY	
	if(SystemLog!=NULL)
		delete SystemLog;
	SystemLog = NULL;


	////////ÿ�α������Ҫ��ջ���
	//if(IsEnterInfo==false)
	//{
		memset(LogMessage,0,sizeof(LogMessage));
		LogMessageLength=0;
	//}	

	///////////�����Ҫ���ϱ��沢�ر��ļ�,�ͷŵ�CRUNLOG
	//if(SaveFile==true)
	//{
	//	delete SystemLog;
	//	SystemLog = NULL;
	//}

}

/*********************************************************************************************************
��������:MakeSendReceiveFrame(CSendFrame SendFrame,CReceiveFrame ReceiveFrame)
����:SendCmd  CSendFrame,ָ���Ǵ����뱣��ķ���֡��������
     ReceCmd  CReceiveFrame,ָ���Ǵ����뱣��Ľ���֡��������

����:����ͨѶ�����еķ�������洢,ע��,�ڷ�������ǰ���Ǽ���(55 aa ��֡����),��������ǰ���Ǽ���(55 bb ��֡����)
     Ŀ����Ϊ�����ַ�������,��󷽱�ۿ��ͻ��д���

����ֵ:void
����:Marco Lin
���ν���ʱ��:2011-12-21     
�޸ļ�¼:

***********************************************************************************************************/
void  CAutoLog::MakeSendReceiveFrame(CSendFrame SendFrame,CReceiveFrame ReceiveFrame)
{
	/////�����ǰ�Ѿ���LOG����״̬
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////����
	if(HideAutoLog==true)
		return ;

	/////�����ǰ�Ѿ���DEMO����״̬
	if(CRunEnvironment::GetDemoMode()==true)
		return ;

	/*
    for(short m=0;m<SendFrame.GetFrameNumber();m++)
    {
		  MakeSendReceiveCmd(SendFrame[m].m_binSendCmd,true);
		  if(m<ReceiveFrame.size())
		  {
			for(short n=0;n<ReceiveFrame[m].size();n++)
			{
				MakeSendReceiveCmd(ReceiveFrame[m][n],false);
			}
		  }
    }
	*/

	/////������֡�ն�֡��֡�����
	if((SendFrame.GetFrameNumber()>=2)&&(ReceiveFrame.size()>=2))
	{
		for(short m=0;m<SendFrame.GetFrameNumber();m++)
		{
			  MakeSendReceiveCmd(SendFrame[m].m_binSendCmd,true);
			  if(m==0)
			  {
				for(short n=0;n<ReceiveFrame[m].size();n++)
				{
					MakeSendReceiveCmd(ReceiveFrame[m][n],false);
				}
			  }

			  else if(m==(SendFrame.GetFrameNumber()-1))
			  {
				for(short k=1;k<ReceiveFrame.size();k++)
				{
					for(short n=0;n<ReceiveFrame[k].size();n++)
					{
						MakeSendReceiveCmd(ReceiveFrame[k][n],false);
					}
				}
			  }
			  else
				  ;
		}		
	}
	////////////////////////////////////////////////////////////
	
	else
	{
		for(short m=0;m<SendFrame.GetFrameNumber();m++)
		{
			  MakeSendReceiveCmd(SendFrame[m].m_binSendCmd,true);
			  if(m<ReceiveFrame.size())
			  {
				for(short n=0;n<ReceiveFrame[m].size();n++)
				{
					MakeSendReceiveCmd(ReceiveFrame[m][n],false);
				}
			  }
		}
	}
}

/*********************************************************************************************************
��������:MakeSendFrame(CSendFrame SendFrame)
����:SendCmd  CSendFrame,ָ���Ǵ����뱣��ķ���֡��������

����:����ͨѶ�����еķ�������洢,ע��,�ڷ�������ǰ���Ǽ���(55 aa ��֡����),��������ǰ���Ǽ���(55 bb ��֡����)
     Ŀ����Ϊ�����ַ�������,��󷽱�ۿ��ͻ��д���

����ֵ:void
����:Marco Lin
���ν���ʱ��:2012-7-2     
�޸ļ�¼:

***********************************************************************************************************/
void  CAutoLog::MakeSendFrame(CSendFrame SendFrame)
{
	/////�����ǰ�Ѿ���LOG����״̬
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////����
	if(HideAutoLog==true)
		return ;

	/////�����ǰ�Ѿ���DEMO����״̬
	if(CRunEnvironment::GetDemoMode()==true)
		return ;

	/////
	for(short m=0;m<SendFrame.GetFrameNumber();m++)
	{
		  MakeSendReceiveCmd(SendFrame[m].m_binSendCmd,true);
	}		
}


/*********************************************************************************************************
��������:MakeReceiveFrame(CReceiveFrame ReceiveFrame)
����:ReceCmd  CReceiveFrame,ָ���Ǵ����뱣��Ľ���֡��������

����:����ͨѶ�����еķ�������洢,ע��,�ڷ�������ǰ���Ǽ���(55 aa ��֡����),��������ǰ���Ǽ���(55 bb ��֡����)
     Ŀ����Ϊ�����ַ�������,��󷽱�ۿ��ͻ��д���

����ֵ:void
����:Marco Lin
���ν���ʱ��:2012-7-2     
�޸ļ�¼:

***********************************************************************************************************/
void  CAutoLog::MakeReceiveFrame(CReceiveFrame ReceiveFrame)
{
	/////�����ǰ�Ѿ���LOG����״̬
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////����
	if(HideAutoLog==true)
		return ;

	/////�����ǰ�Ѿ���DEMO����״̬
	if(CRunEnvironment::GetDemoMode()==true)
		return ;


	for(short m=0;m<ReceiveFrame.size();m++)
	{
		for(short n=0;n<ReceiveFrame[m].size();n++)
			MakeSendReceiveCmd(ReceiveFrame[m][n],false);
	}
}


/*********************************************************************************************************
��������:MakeSendReceiveCmd(CBinary CmdFrame,bool IsSendCmd)
����:CmdFrame  CBinary����,ָ���Ǵ����뱣��ķ�����������,�����ݻ���ʱ�ŵ�������LogEnterMessage��LogRecordMessage
	 IsSendCmd   bool����,��ʶ�Ƿ��Ƿ���֡,�Ա�����ǰ���55 aa ������55 bb

����:����ͨѶ�����еķ�������洢,ע��,�ڷ�������ǰ���Ǽ���(55 aa ��֡����),��������ǰ���Ǽ���(55 bb ��֡����)
     Ŀ����Ϊ�����ַ�������,��󷽱�ۿ��ͻ��д���

����ֵ:void
����:Marco Lin
���ν���ʱ��:2011-12-21     
�޸ļ�¼:

***********************************************************************************************************/
void  CAutoLog::MakeSendReceiveCmd(CBinary CmdFrame,bool IsSendCmd)
{

	char a=0;////�м����
	char *temp;////�м�ʹ�õ��ַ���
	int i=0,j=0;
	int len=0;

	/////�����ǰ�Ѿ���LOG����״̬
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////����
	if(HideAutoLog==true)
		return ;

	/////�����ǰ�Ѿ���DEMO����״̬
	if(CRunEnvironment::GetDemoMode()==true)
		return ;
	
	//////////////��������������������������2K,����������ֵ,��ô���ȱ���ķ��ͽ�����������ȱ��ص�
	len=CmdFrame.GetSize()+4;/////���㽫Ҫ��ȡ������ռ��С

	if((IsEnterInfo==true)&&(LogMessageLength+len>=2048))////�����ǽ�������,����2K�Ļ�,�����ǲ�������
		return ;

	if(LogMessageLength+len>=2048)
	{
		/////��λӦ��ȥ����Щ֡��Ϣ
		for(i=len;i<LogMessageLength;i++)
		{
			if((((unsigned char)LogMessage[i])==0x55)&&(((unsigned char)LogMessage[i+1])==0xaa))
			{
				break;
			}
		}

		//////////////////���Ѵ�֡��Ϣ��ǰ���ƶ�,�ճ�����Ŀռ�
		temp=new char[2048];	
		memset(temp,0,sizeof(temp));
		for(j=0;j<LogMessageLength;j++)
		{
			temp[j]=LogMessage[j];
		}

		//temp[j]='\0';
		
		memset(LogMessage,0,sizeof(LogMessage));			
		for(j=0;i<LogMessageLength;i++,j++)
		{
			a=temp[i];
			LogMessage[j]=a;
		}

		LogMessageLength=j;
		delete []temp;
		//memset(temp,0,sizeof(temp));
		//////////////////////////////////////////////////////
		/////55 aa SendCmd
		if(IsSendCmd==true)
		{
			LogMessage[LogMessageLength++]=(char)0x55;
			LogMessage[LogMessageLength++]=(char)0xaa;
			LogMessage[LogMessageLength++]=(char)(CmdFrame.GetSize()+3);
			for(j=0;j<CmdFrame.GetSize();j++)
			{
				LogMessage[LogMessageLength++]=CmdFrame.GetAt(j);
			}
		}
		//////55 bb ReceCmd
		else
		{
			LogMessage[LogMessageLength++]=(char)0x55;
			LogMessage[LogMessageLength++]=(char)0xbb;
			LogMessage[LogMessageLength++]=(char)(CmdFrame.GetSize()+3);
			for(j=0;j<CmdFrame.GetSize();j++)
			{
				LogMessage[LogMessageLength++]=CmdFrame.GetAt(j);
			}
		}
	}
	else
	{
		/////55 aa SendCmd
		if(IsSendCmd==true)
		{
			LogMessage[LogMessageLength++]=(char)0x55;
			LogMessage[LogMessageLength++]=(char)0xaa;
			LogMessage[LogMessageLength++]=(char)(CmdFrame.GetSize()+3);
			for(j=0;j<CmdFrame.GetSize();j++)
			{
				LogMessage[LogMessageLength++]=CmdFrame.GetAt(j);
			}
		}


		//////55 bb ReceCmd
		else
		{
			LogMessage[LogMessageLength++]=(char)0x55;
			LogMessage[LogMessageLength++]=(char)0xbb;
			LogMessage[LogMessageLength++]=(char)(CmdFrame.GetSize()+3);
			for(j=0;j<CmdFrame.GetSize();j++)
			{
				LogMessage[LogMessageLength++]=CmdFrame.GetAt(j);
			}
		}
	}

}
