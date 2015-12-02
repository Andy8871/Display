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

	/////如果当前不是LOG功能状态,分配内存
	//if(CRunEnvironment::GetCheckMode()==false)
	//{
	//	LogMessage=new char[2048];
	//	memset(LogMessage,0,sizeof(LogMessage));
	//}
}

CAutoLog::~CAutoLog()
{
	/////如果当前不是LOG功能状态,分配内存
	if((CRunEnvironment::GetCheckMode()==false)&&(HideAutoLog==false)&&(CRunEnvironment::GetDemoMode()==false))
	{
		delete []LogMessage;
		
		if(SystemLog!=NULL)
			delete SystemLog;
			
		SystemLog = NULL;		
	}	
}

/*********************************************************************************************************
初始化工作
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
初始化工作
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
函数名称:PrintString(string fmt,bool PrintMenu,bool IsEnterInfo,bool SaveFile)
参数:fmt  string 类型,指的是传入想保存的内容,这里主要是开放给要打印一些自定义字符串,做为特别加注
     PrintMenu  bool类型,标识是否要将当前的菜单详细路径一起打印出来
	 IsEnterInfo bool类型,标识保存的是否是进入信息
	 SaveFile bool类型,标识是否在保存完当前信息后,马上关闭保存生成文件,如果为FALSE,则后面的可追加,如果为TRUE
	                   则被关闭,下次再有数据保存会再生成新的文件.

功能:打印自定义字符串信息到文件保存

返回值:void
作者:Marco Lin
初次建档时间:2011-12-21     
修改记录:

***********************************************************************************************************/
void  CAutoLog::PrintString(string fmt)
{
	char    SystemFileName[128];
	char    FileNameBuff[50];
	int i=0,j=0;
	short StartPos=0;
	short EndPos=0;
	unsigned char    iCount=0;

	/////如果当前已经是LOG功能状态
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////隐藏
	if(HideAutoLog==true)
		return ;

	/////如果当前已经是DEMO功能状态
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

			////有多个EXCEL文件的时候,只取第一个文件名
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


	//////如果CRUNLOG指针未分配,也就是未设立文件,给分配相关信息
	if(SystemLog==NULL)
	{		
		//////当前功能已有故障,文件名要在后面加BUG标识
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
			if(SystemLog->Init(SystemFileName)==false)////初始化CRUNLOG,建立SystemBugFileName命名TXT文件
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
		}

		//////如果进入系统,也没有问题,保存文件不带扩展符
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
			if(SystemLog->Init(SystemFileName)==false)////初始化CRUNLOG,建立SystemBugFileName命名TXT文件
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
		}			
		

		/////如果当前系统没问题,也非进入系统数据保存,文件名要在后面加OK标识
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
			if(SystemLog->Init(SystemFileName)==false)////初始化CRUNLOG,建立SystemBugFileName命名TXT文件
			{
				delete SystemLog;
				SystemLog = NULL;				
				return ;
			}
		}
		
	}

	/////////////打印当前路径菜单
	//if(PrintMenu==true)
	//	SystemLog->WriteMenu();

	////////////打印内容到文件
	SystemLog->WriteContent(fmt,true);

	//////////保存类型不是进入,则每次保存完后都要清空缓存
	//if(IsEnterInfo==false)
	//{
	//	memset(LogMessage,0,sizeof(LogMessage));
	//	LogMessageLength=0;
	//}	
	///////////如果需要马上保存并关闭文件,释放掉CRUNLOG
	//if(SaveFileNow==true)
	//{
	//	delete SystemLog;
	//	SystemLog = NULL;
	//}
}



/*********************************************************************************************************
函数名称:LogPrint(char *fmt,bool IsEnterInfo)
参数:fmt  char * 类型,指的是传入想保存的内容,只能是缓存区变量LogEnterMessage或LogRecordMessage
	 IsEnterInfo bool类型,标识保存的是否是进入信息


功能:打印所有要记录的命令信息到文件保存

返回值:void
作者:Marco Lin
初次建档时间:2011-12-21     
修改记录:

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


	/////如果当前已经是LOG功能状态
	if(CRunEnvironment::GetCheckMode()==true)
		return ;
	
	////隐藏
	if(HideAutoLog==true)
		return ;
	
	/////如果当前已经是DEMO功能状态
	if(CRunEnvironment::GetDemoMode()==true)
		return ;

	/////判断传入的命令串是否为空
	if(LogMessageLength<=0)
		return ;

	/////判断传入的命令串
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

			////有多个EXCEL文件的时候,只取第一个文件名
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
	/////如果当前已经是LOG功能状态
	//if(CRunEnvironment::GetCheckMode()==true)
	//	return ;
	
	//memset(temp,0,sizeof(temp));

	//temp=new char[LogMessageLength];
	//for(i=0;i<LogMessageLength;i++)
	//	temp[i]=LogMessage[i];	

	//FMT.WriteBuffer(temp,LogMessageLength);
	//delete []temp;/////释放TEMP内存
	
	//////如果CRUNLOG指针未分配,也就是未设立文件,给分配相关信息
	if(SystemLog==NULL)
	{		
		//////当前功能已有故障,文件名要在后面加BUG标识
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
			if(SystemLog->Init(SystemFileName)==false)////初始化CRUNLOG,建立SystemBugFileName命名TXT文件
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
			//SystemLog->SetBufSize(4080);
		}

		//////如果进入系统,也没有问题,保存文件不带扩展符
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
			if(SystemLog->Init(SystemFileName)==false)////初始化CRUNLOG,建立SystemBugFileName命名TXT文件
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
			//SystemLog->SetBufSize(4080);
		}			
		

		/////如果当前系统没问题,也非进入系统数据保存,文件名要在后面加OK标识
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
			if(SystemLog->Init(SystemFileName)==false)////初始化CRUNLOG,建立SystemBugFileName命名TXT文件
			{
				delete SystemLog;
				SystemLog = NULL;
				return ;
			}
			//SystemLog->SetBufSize(4080);
		}

	}


	////一些机器信息
	SystemLog->WriteContent("SerialNumber:"+CRunEnvironment::GetSerialNumber());
	SystemLog->WriteContent("DisplayVersion:"+CRunEnvironment::GetDisplayVersion());
	if(CRunEnvironment::GetNewMcuFlag()==true)
		SystemLog->WriteContent("NewMcuFlag:true");
	else
		SystemLog->WriteContent("NewMcuFlag:false");
	SystemLog->WriteContent("----------------------------------------------------------------------------------------");
	SystemLog->WriteContent(" ",true);////换行
	
	/////////////打印当前路径菜单
	SystemLog->WriteMenu();
	
	//////将要打印的串内容FMT,根据标识符55 aa,55 bb一条一条的换行打印到文件里
	
	for(i=0;i<LogMessageLength;i++)
	{
		if(((unsigned char)LogMessage[i]==0x55)&&(((unsigned char)LogMessage[i+1]==0xaa)||((unsigned char)LogMessage[i+1]==0xbb)))
		{
			len=(unsigned char)LogMessage[i+2];
			pos=i;

			/////判断其长度是否为空,为空不记录,超过255异常也不记录
			if((len<=0)||(len>255))
				continue;
				
			//temp=new char[len];
			//for(j=0;j<len;j++)
			//{
			//	temp[j]=FMT.GetAt(i);
			//	i++;
			//}
			//i--;

			//SubFmt.WriteBuffer(temp,len);////子串
			//delete []temp;/////释放TEMP内存


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
			
			SystemLog->WriteContent(temp);////打印内容
			//memset(temp,0,sizeof(temp));
			//SubFmt.FreeExtra();/////释放CBINARY	
			delete []temp;/////释放TEMP内存
		}
	}
		
	SystemLog->WriteContent(" ",true);////换行
	
	/////每次打印完命令信息后要将打印记录复位
	RecordFlag=false;
	IsEnterInfo=false;
	//SubFmt.FreeExtra();/////释放CBINARY	
	if(SystemLog!=NULL)
		delete SystemLog;
	SystemLog = NULL;


	////////每次保存完后都要清空缓存
	//if(IsEnterInfo==false)
	//{
		memset(LogMessage,0,sizeof(LogMessage));
		LogMessageLength=0;
	//}	

	///////////如果需要马上保存并关闭文件,释放掉CRUNLOG
	//if(SaveFile==true)
	//{
	//	delete SystemLog;
	//	SystemLog = NULL;
	//}

}

/*********************************************************************************************************
函数名称:MakeSendReceiveFrame(CSendFrame SendFrame,CReceiveFrame ReceiveFrame)
参数:SendCmd  CSendFrame,指的是传入想保存的发送帧命令内容
     ReceCmd  CReceiveFrame,指的是传入想保存的接收帧命令内容

功能:保存通讯过程中的发收命令存储,注意,在发送命令前我们加上(55 aa 整帧长度),接收命令前我们加入(55 bb 整帧长度)
     目的是为了区分发收命令,最后方便观看和换行处理

返回值:void
作者:Marco Lin
初次建档时间:2011-12-21     
修改记录:

***********************************************************************************************************/
void  CAutoLog::MakeSendReceiveFrame(CSendFrame SendFrame,CReceiveFrame ReceiveFrame)
{
	/////如果当前已经是LOG功能状态
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////隐藏
	if(HideAutoLog==true)
		return ;

	/////如果当前已经是DEMO功能状态
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

	/////处理发多帧收多帧或单帧的情况
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
函数名称:MakeSendFrame(CSendFrame SendFrame)
参数:SendCmd  CSendFrame,指的是传入想保存的发送帧命令内容

功能:保存通讯过程中的发收命令存储,注意,在发送命令前我们加上(55 aa 整帧长度),接收命令前我们加入(55 bb 整帧长度)
     目的是为了区分发收命令,最后方便观看和换行处理

返回值:void
作者:Marco Lin
初次建档时间:2012-7-2     
修改记录:

***********************************************************************************************************/
void  CAutoLog::MakeSendFrame(CSendFrame SendFrame)
{
	/////如果当前已经是LOG功能状态
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////隐藏
	if(HideAutoLog==true)
		return ;

	/////如果当前已经是DEMO功能状态
	if(CRunEnvironment::GetDemoMode()==true)
		return ;

	/////
	for(short m=0;m<SendFrame.GetFrameNumber();m++)
	{
		  MakeSendReceiveCmd(SendFrame[m].m_binSendCmd,true);
	}		
}


/*********************************************************************************************************
函数名称:MakeReceiveFrame(CReceiveFrame ReceiveFrame)
参数:ReceCmd  CReceiveFrame,指的是传入想保存的接收帧命令内容

功能:保存通讯过程中的发收命令存储,注意,在发送命令前我们加上(55 aa 整帧长度),接收命令前我们加入(55 bb 整帧长度)
     目的是为了区分发收命令,最后方便观看和换行处理

返回值:void
作者:Marco Lin
初次建档时间:2012-7-2     
修改记录:

***********************************************************************************************************/
void  CAutoLog::MakeReceiveFrame(CReceiveFrame ReceiveFrame)
{
	/////如果当前已经是LOG功能状态
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////隐藏
	if(HideAutoLog==true)
		return ;

	/////如果当前已经是DEMO功能状态
	if(CRunEnvironment::GetDemoMode()==true)
		return ;


	for(short m=0;m<ReceiveFrame.size();m++)
	{
		for(short n=0;n<ReceiveFrame[m].size();n++)
			MakeSendReceiveCmd(ReceiveFrame[m][n],false);
	}
}


/*********************************************************************************************************
函数名称:MakeSendReceiveCmd(CBinary CmdFrame,bool IsSendCmd)
参数:CmdFrame  CBinary类型,指的是传入想保存的发送命令内容,此内容会暂时放到缓存区LogEnterMessage或LogRecordMessage
	 IsSendCmd   bool类型,标识是否是发送帧,以便于在前面加55 aa 划或是55 bb

功能:保存通讯过程中的发收命令存储,注意,在发送命令前我们加上(55 aa 整帧长度),接收命令前我们加入(55 bb 整帧长度)
     目的是为了区分发收命令,最后方便观看和换行处理

返回值:void
作者:Marco Lin
初次建档时间:2011-12-21     
修改记录:

***********************************************************************************************************/
void  CAutoLog::MakeSendReceiveCmd(CBinary CmdFrame,bool IsSendCmd)
{

	char a=0;////中间变量
	char *temp;////中间使用的字符串
	int i=0,j=0;
	int len=0;

	/////如果当前已经是LOG功能状态
	if(CRunEnvironment::GetCheckMode()==true)
		return ;

	////隐藏
	if(HideAutoLog==true)
		return ;

	/////如果当前已经是DEMO功能状态
	if(CRunEnvironment::GetDemoMode()==true)
		return ;
	
	//////////////过程命令数据我们设置上限是2K,如果超过这个值,那么最先保存的发送接收命令串将最先被截掉
	len=CmdFrame.GetSize()+4;/////计算将要存取的所需空间大小

	if((IsEnterInfo==true)&&(LogMessageLength+len>=2048))////假如是进入数据,超过2K的话,我们是不存后面的
		return ;

	if(LogMessageLength+len>=2048)
	{
		/////定位应该去除哪些帧信息
		for(i=len;i<LogMessageLength;i++)
		{
			if((((unsigned char)LogMessage[i])==0x55)&&(((unsigned char)LogMessage[i+1])==0xaa))
			{
				break;
			}
		}

		//////////////////将已存帧信息向前面移动,空出后面的空间
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
