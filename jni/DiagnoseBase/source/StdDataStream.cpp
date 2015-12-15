/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：	StdDataStream.cpp
	文档标识：	007汽车诊断平台详细设计说明书	
	摘    要：	完成数据流的读取及显示。
				按照输入的数据流ID项，完成从命令库读取读数据流命令、让用户选择
				要显示的数据项目、发读数据流命令、按ECU回复数据进行计算并显示流
				据流完整内容、到用户按《BACK》键退出显示的全过程。
				本程序适用于绝大多数读数据流功能，在不能满足诊断程序需要时，诊
				断程序员自行完成读数据流工作。

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2006.01.12  zhangsh		1.0			创建此类。                                           

**************************************************************************************/


#include "StdDataStream.h"
#include "MultiSelectShow.h"
#include "Database.h"
#include "CommWithEcu.h"
#include "DataStreamShow.h"
#include "display.h"

#include "assert.h"

/*extern CDisplay * g_pDisplay;*/
//extern CCommWithEcu * g_pCommWithEcu;

// define error
#define	NODATASTREAMITEM	-1		// 传入的数据流ID数组为空，没有数据流项
#define	NOITEMINDSLIB		-2		// 在数据流库中未找到相应的数据流项
#define	NOITEMINTXTLIB		-3		// 在文本库中未找到相应的文本串项

// define item position of database
#define	TXTIDINDSLIB		0	   // 数据流的文本串ID在数据流库中的项位置
#define	STRINTXTLIB			0	   // 文本串内容在文本库中的项位置
#define	UNITINDSLIB			1	   // 数据流库中的单位内容在项的中位置
#define	CMDIDINDSLIB		4	   // 数据流库中的命令ID在项中的起始位置（命令可能有多条）


//extern CCommWithEcu * g_pCommWithEcu;	//全局通信接口

CStdDataStream::CStdDataStream(void) 
{ 

}


CStdDataStream::~CStdDataStream(void) 
{ 

}

/**************************************************************************
功　　能：		a. 重载虚函数，读取数据流库，填加命令内容到m_aSendFrame，
					并将其下标值写入m_lstDsIdSearchSendFrame；
				b. 将数据流库内容添加到算法类中（m_Arithmetic. AddDataStreamItem ()）
参数说明：		无
返 回 值：		无
说　　明：		无
***************************************************************************/
#if 0 
void CStdDataStream::DsIdToSendFrame (void)
{
	CDatabase dsLib;
	dsLib.Open(CDatabase::DB_DATA_STREAM);	// 打开数据流库

	list<CDsIdSendFrame>::iterator vIter;
	int iOrder = 0;

	for (vIter = m_listDsIdSendFrame.begin(); vIter != m_listDsIdSendFrame.end(); vIter++, iOrder++)
	{
		vector<CBinary> dsContent;
		dsContent = dsLib.SearchId(vIter->idDataStream);

		// 没有内容或少于5条都是不正确的
		if (dsContent.size() < 5) 
		{
			break;
		}

		// 在命令库中查找到命令并添加到发送帧数组
		CSendFrame sendFrame;
		sendFrame.AddFromCmdLib(dsContent[CMDIDINDSLIB]);
		m_SendFrame += sendFrame;

		// 将其下标值写入m_lstDsIdSearchSendFrame；
		vIter->i16SendFrameOrder = m_SendFrame.GetFrameNumber() - 1;	// 命令在发送帧中的下标
		vIter->idUnit			 = dsContent[UNITINDSLIB];	// 单位内容

		// 将数据流的内容添加到算法类中
		int ret = m_Arithmetic.AddDataStreamItem(vIter->idDataStream, dsContent);
		if(ret != 0)
			break;
	}

	dsLib.Close();
}

#else

//第一次查库的时候已经记住了每项数据流后面的内容
void CStdDataStream::DsIdToSendFrame (void)
{
	list<CDsIdSendFrame>::iterator vIter;
	int iOrder = 0;
	int nFrameCount;
	int i,j;
	int nEqual;
	int nRet;

	for (vIter = m_listDsIdSendFrame.begin(); vIter != m_listDsIdSendFrame.end(); vIter++, iOrder++)
	{
		CSendFrame sendFrame;

		nRet=sendFrame.AddFromCmdLib(vIter->listDsContent[CMDIDINDSLIB]);
		assert(nRet);

		//查找确认此命令是否已经发送过，如果发送过就不需要再发送,只需记住接收数据的编号
		nFrameCount=m_SendFrame.GetFrameNumber();
        char iNum=sendFrame.GetFrameNumber();

		if(nFrameCount>=sendFrame.GetFrameNumber())
		{
			for(i=0;i<(nFrameCount - sendFrame.GetFrameNumber() + 1);i++)
			{
				nEqual=1;
				for(j=0;j<sendFrame.GetFrameNumber();j++)
				{
					if(!(m_SendFrame[i+j].m_binSendCmd==sendFrame[j].m_binSendCmd))
					{
						nEqual=0;
						break;
					}
				}

				if(nEqual)
					break;
			}

			if(!nEqual)
			{
				// 将其下标值写入m_lstDsIdSearchSendFrame；
				vIter->i16SendFrameOrder = m_SendFrame.GetFrameNumber() + sendFrame.GetFrameNumber() - 1;	// 命令在发送帧中的下标
				m_SendFrame += sendFrame;
				//vIter->idUnit			 = vIter->listDsContent[UNITINDSLIB];	// 单位内容
			}
			else //相同，取该组命令的最后一条发送命令的应答做为此命令的应答
			{
				vIter->i16SendFrameOrder = i + sendFrame.GetFrameNumber() - 1;	// 命令在发送帧中的下标
			}
		}
		else
		{
			// 将其下标值写入m_lstDsIdSearchSendFrame；
			vIter->i16SendFrameOrder = m_SendFrame.GetFrameNumber() + sendFrame.GetFrameNumber() - 1;	// 命令在发送帧中的下标
			m_SendFrame += sendFrame;
			//vIter->idUnit			 = vIter->listDsContent[UNITINDSLIB];	// 单位内容
		}

		// 将数据流的内容添加到算法类中
		int ret = m_Arithmetic.AddDataStreamItem(vIter->idDataStream, vIter->listDsContent);

		if(ret != 0)
			break;
	}

}

#endif


/**************************************************************************
功　　能：		完成读数据流并显示
参数说明：		vector < CBinary> *paidDataStream 数据流ID
返 回 值：		错误代码
说　　明：		重载原虚函数
					程序流程：
					{
						do{ 
							if (显示多选项) {
								显示数据多项选择；
								if (返回CANCEL键值)
										break;
							}
							清空m_lstDsIdSearchSendFrame和m_aSendFrame内容
							将选中的数据流项ID加入m_lstDsIdSearchSendFrame中;
							DsIdToSendFrame ();
							组合数据流命令;
							do {
								发送命令，读取ECU应答
								if ((strDataStreamValue = m_Arithmetic.DataStream(idDataStream, abinFromEcu))==””)
									strDataStreamValue = ExceptionArithmetic (idDataStream, abinFromEcu);
								显示数据流数值
							} while (用户未按退出)；
						}while (true); 
					}
**************************************************************************/

short CStdDataStream::ReadDataStream (vector<CBinary> *paidDataStream)
{
	short iRet = 0;
//	short i,j;

	if (paidDataStream->size() == 0)//数据流无内容项
	{
		return NODATASTREAMITEM;
	}

	CDatabase dsLib;
	vector<CBinary> DsBuff;
	dsLib.Open(CDatabase::DB_DATA_STREAM);	// 打开数据流库
	short vSize = 0;
	vector<CBinary>::iterator vIter;
	CBaseDataStream::CDsIdSendFrame dsSendFrame;
	CReceiveFrame ReceiveFrame;
	CBinaryGroup OneReceiveFrame;

	do
	{//读数据流开始
		if (m_bShowDataStreamSelectWindow)
		{
			CMultiSelectShow::CSelectedItemData aSelected;
			// 设置了显示多选窗口选项

			CDisplay::GetInstance()->MultiSelect.Init("");

			for(vIter = paidDataStream->begin(); vIter != paidDataStream->end(); vIter++)
			{
				CDisplay::GetInstance()->MultiSelect.Add(*vIter);

			}
			if (!CDisplay::GetInstance()->MultiSelect.Show(aSelected))//用户是否按OK键，是—true 否—false
			{
				break;
			}

			// 清空m_listDsIdSendFrame和m_aSendFrame内容
			m_listDsIdSendFrame.clear();
			m_SendFrame.Clear();

			// 选取被选择的项填入数据流数组
			vSize = 0;
			for (vIter = paidDataStream->begin(); vIter != paidDataStream->end(); vIter++, vSize++)
			{
				if (aSelected.IsSelected(vSize))
				{
					dsSendFrame.idDataStream = *vIter;
				
					dsSendFrame.listDsContent=dsLib.SearchId(*vIter);  //查数据流库

					if(dsSendFrame.listDsContent.size() < 5)
					{
						return iRet = NOITEMINDSLIB; 
					}

					//dsSendFrame.idUnit = dsSendFrame.listDsContent[1];//是显示程序转换成string 

					dsSendFrame.i16SendFrameOrder = 0;	//将下标先全初始化为0

					m_listDsIdSendFrame.push_back(dsSendFrame);
				}
			}
		}//if显示多选窗口
		// 没有设置多选窗口选项，则全部选择(即在功能菜单中一选中数据流，就把所有数据流ID的命令发出去)
		else  
		{
			for (vIter = paidDataStream->begin(); vIter != paidDataStream->end(); vIter++)
			{
				dsSendFrame.idDataStream = *vIter;
				DsBuff = dsLib.SearchId(*vIter);

				dsSendFrame.listDsContent=dsLib.SearchId(*vIter);  //查数据流库

				if(dsSendFrame.listDsContent.size() < 5)
				{
					return iRet = NOITEMINDSLIB; 
				}

				//dsSendFrame.idUnit = dsSendFrame.listDsContent[1];//是显示程序转换成string 

				dsSendFrame.i16SendFrameOrder = 0; //将下标先全初始化为0

				m_listDsIdSendFrame.push_back(dsSendFrame);
			}
		}

		DsIdToSendFrame ();	
		
		// 组织数据流命令及算法
		// 循环发送命令、接收数据及显示过程

		bool bLoopDsProcess = true;
		list<CDsIdSendFrame>::iterator lIter;
		string strDsValue;

		OnBeforeSendDsCmd();

		do
		{
#if 1
			m_ReceiveFrame = CCommWithEcu::GetInstance()->SendReceive(m_SendFrame, true);//发送并取得返回值,并设置循环执行标志
			
			if(0==m_ReceiveFrame.size())
			{
				//有发送帧未得到应答，直接返回?
				//发送一条命令，停止发送数据流命令
				CCommWithEcu::GetInstance()->GetStatus(GS_CHANNEL_SIGNAL);

				return -22;
			}
			
			if(m_ReceiveFrame.size() < m_SendFrame.GetFrameNumber())
			{
				//有发送帧未得到应答，直接返回?
				//发送一条命令，停止发送数据流命令
				CCommWithEcu::GetInstance()->GetStatus(GS_CHANNEL_SIGNAL);
				return -22;
			}

			lIter = m_listDsIdSendFrame.begin();
			if(lIter->i16SendFrameOrder>=m_ReceiveFrame.size())
			{
				//有发送帧未得到应答，直接返回?
				//发送一条命令，停止发送数据流命令
				CCommWithEcu::GetInstance()->GetStatus(GS_CHANNEL_SIGNAL);
				return -22;
			}

#else	//调试代码，设置应答帧的数据		
			vector<CBinary> listRecv;

			static unsigned char bb1=0;
			static unsigned char bb2=0;

			CBinary binCmd1(CBinary("\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11",20));

			m_ReceiveFrame.clear();

			for(i=0;i<m_listDsIdSendFrame.size();i++)
			{
				for(j=0;j<binCmd1.GetSize();j++)
					binCmd1.SetAt(j,bb1);

				bb1++;

				listRecv.push_back(binCmd1);
				m_ReceiveFrame+=listRecv;
			}
#endif

			CDisplay::GetInstance()->DataStream.Init();

			for (lIter = m_listDsIdSendFrame.begin(); lIter != m_listDsIdSendFrame.end(); lIter++)
			{	
				ReceiveFrame.clear();

				//wcx 9/11/2006 
				//OneReceiveFrame = m_ReceiveFrame[lIter->i16SendFrameOrder];
				//ReceiveFrame += OneReceiveFrame;
				for(int i=lIter->i16SendFrameOrder; i<m_ReceiveFrame.size(); i++){
					ReceiveFrame += m_ReceiveFrame[i];
				}
				//wcx 9/11/2006

				//strDsValue = (CBaseDataStream::m_Arithmetic).DataStream(lIter->idDataStream, ReceiveFrame);

				strDsValue = (CBaseDataStream::m_Arithmetic).DataStreamEx(lIter->idDataStream, ReceiveFrame);

				//凌义川添加，原来没有调用，无法用算法描述的数据流，用户重载 ExceptionArithmetic 自己处理
				if(strDsValue.length()>=6)//strlen("@@??@@"))
				{
					vector<string> szParam;
					string szFlag=strDsValue.substr(0,6);
					string szTemp=strDsValue.substr(6,strDsValue.length() - 6);
					string szItem;

					int nstart,npos;

					if(szFlag=="@@??@@")
					{
						if(szTemp.length()>0)
						{
							npos=0;
							nstart=0;

							//依次加入公式，参数，真值表
							while(npos<szTemp.length())
							{
								if(szTemp[npos]==';')
								{
									szItem=szTemp.substr(nstart,npos - nstart);
									szParam.push_back(szItem);

									nstart=npos + 1;
								}

								npos++;
							}

							//最后一条
							if(npos > nstart)
							{
								szItem=szTemp.substr(nstart,npos - nstart);
								szParam.push_back(szItem);
							}						
						}
						
						strDsValue = ExceptionArithmetic (lIter->idDataStream, &ReceiveFrame,szParam);
					}				
				}

				CDisplay::GetInstance()->DataStream.Add(lIter->idDataStream, strDsValue);
			}

			// 显示返回结果
			if (CDisplay::GetInstance()->DataStream.Show() == adsIDBACK)
			{
				bLoopDsProcess = false;
			}

			//Sleep(100);

		}while(bLoopDsProcess);

		//发送一条命令，停止发送数据流命令
		CCommWithEcu::GetInstance()->GetStatus(GS_CHANNEL_SIGNAL);

	}while(true);  // 反复读数据流


	return 1;
}

/*
功    能：用于无链路保持的系统重进系统使用
参数说明：无
返 回 值：无
说    明：在向下位机发送数据流通信命令之前调用； */
void CStdDataStream::OnBeforeSendDsCmd ()
{

}
