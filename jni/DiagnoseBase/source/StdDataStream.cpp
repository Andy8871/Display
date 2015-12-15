/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�	StdDataStream.cpp
	�ĵ���ʶ��	007�������ƽ̨��ϸ���˵����	
	ժ    Ҫ��	����������Ķ�ȡ����ʾ��
				���������������ID���ɴ�������ȡ��������������û�ѡ��
				Ҫ��ʾ��������Ŀ�����������������ECU�ظ����ݽ��м��㲢��ʾ��
				�����������ݡ����û�����BACK�����˳���ʾ��ȫ���̡�
				�����������ھ�����������������ܣ��ڲ���������ϳ�����Ҫʱ����
				�ϳ���Ա������ɶ�������������

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.01.12  zhangsh		1.0			�������ࡣ                                           

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
#define	NODATASTREAMITEM	-1		// �����������ID����Ϊ�գ�û����������
#define	NOITEMINDSLIB		-2		// ������������δ�ҵ���Ӧ����������
#define	NOITEMINTXTLIB		-3		// ���ı�����δ�ҵ���Ӧ���ı�����

// define item position of database
#define	TXTIDINDSLIB		0	   // ���������ı���ID�����������е���λ��
#define	STRINTXTLIB			0	   // �ı����������ı����е���λ��
#define	UNITINDSLIB			1	   // ���������еĵ�λ�����������λ��
#define	CMDIDINDSLIB		4	   // ���������е�����ID�����е���ʼλ�ã���������ж�����


//extern CCommWithEcu * g_pCommWithEcu;	//ȫ��ͨ�Žӿ�

CStdDataStream::CStdDataStream(void) 
{ 

}


CStdDataStream::~CStdDataStream(void) 
{ 

}

/**************************************************************************
�������ܣ�		a. �����麯������ȡ�������⣬����������ݵ�m_aSendFrame��
					�������±�ֵд��m_lstDsIdSearchSendFrame��
				b. ����������������ӵ��㷨���У�m_Arithmetic. AddDataStreamItem ()��
����˵����		��
�� �� ֵ��		��
˵��������		��
***************************************************************************/
#if 0 
void CStdDataStream::DsIdToSendFrame (void)
{
	CDatabase dsLib;
	dsLib.Open(CDatabase::DB_DATA_STREAM);	// ����������

	list<CDsIdSendFrame>::iterator vIter;
	int iOrder = 0;

	for (vIter = m_listDsIdSendFrame.begin(); vIter != m_listDsIdSendFrame.end(); vIter++, iOrder++)
	{
		vector<CBinary> dsContent;
		dsContent = dsLib.SearchId(vIter->idDataStream);

		// û�����ݻ�����5�����ǲ���ȷ��
		if (dsContent.size() < 5) 
		{
			break;
		}

		// ��������в��ҵ������ӵ�����֡����
		CSendFrame sendFrame;
		sendFrame.AddFromCmdLib(dsContent[CMDIDINDSLIB]);
		m_SendFrame += sendFrame;

		// �����±�ֵд��m_lstDsIdSearchSendFrame��
		vIter->i16SendFrameOrder = m_SendFrame.GetFrameNumber() - 1;	// �����ڷ���֡�е��±�
		vIter->idUnit			 = dsContent[UNITINDSLIB];	// ��λ����

		// ����������������ӵ��㷨����
		int ret = m_Arithmetic.AddDataStreamItem(vIter->idDataStream, dsContent);
		if(ret != 0)
			break;
	}

	dsLib.Close();
}

#else

//��һ�β���ʱ���Ѿ���ס��ÿ�����������������
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

		//����ȷ�ϴ������Ƿ��Ѿ����͹���������͹��Ͳ���Ҫ�ٷ���,ֻ���ס�������ݵı��
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
				// �����±�ֵд��m_lstDsIdSearchSendFrame��
				vIter->i16SendFrameOrder = m_SendFrame.GetFrameNumber() + sendFrame.GetFrameNumber() - 1;	// �����ڷ���֡�е��±�
				m_SendFrame += sendFrame;
				//vIter->idUnit			 = vIter->listDsContent[UNITINDSLIB];	// ��λ����
			}
			else //��ͬ��ȡ������������һ�����������Ӧ����Ϊ�������Ӧ��
			{
				vIter->i16SendFrameOrder = i + sendFrame.GetFrameNumber() - 1;	// �����ڷ���֡�е��±�
			}
		}
		else
		{
			// �����±�ֵд��m_lstDsIdSearchSendFrame��
			vIter->i16SendFrameOrder = m_SendFrame.GetFrameNumber() + sendFrame.GetFrameNumber() - 1;	// �����ڷ���֡�е��±�
			m_SendFrame += sendFrame;
			//vIter->idUnit			 = vIter->listDsContent[UNITINDSLIB];	// ��λ����
		}

		// ����������������ӵ��㷨����
		int ret = m_Arithmetic.AddDataStreamItem(vIter->idDataStream, vIter->listDsContent);

		if(ret != 0)
			break;
	}

}

#endif


/**************************************************************************
�������ܣ�		��ɶ�����������ʾ
����˵����		vector < CBinary> *paidDataStream ������ID
�� �� ֵ��		�������
˵��������		����ԭ�麯��
					�������̣�
					{
						do{ 
							if (��ʾ��ѡ��) {
								��ʾ���ݶ���ѡ��
								if (����CANCEL��ֵ)
										break;
							}
							���m_lstDsIdSearchSendFrame��m_aSendFrame����
							��ѡ�е���������ID����m_lstDsIdSearchSendFrame��;
							DsIdToSendFrame ();
							�������������;
							do {
								���������ȡECUӦ��
								if ((strDataStreamValue = m_Arithmetic.DataStream(idDataStream, abinFromEcu))==����)
									strDataStreamValue = ExceptionArithmetic (idDataStream, abinFromEcu);
								��ʾ��������ֵ
							} while (�û�δ���˳�)��
						}while (true); 
					}
**************************************************************************/

short CStdDataStream::ReadDataStream (vector<CBinary> *paidDataStream)
{
	short iRet = 0;
//	short i,j;

	if (paidDataStream->size() == 0)//��������������
	{
		return NODATASTREAMITEM;
	}

	CDatabase dsLib;
	vector<CBinary> DsBuff;
	dsLib.Open(CDatabase::DB_DATA_STREAM);	// ����������
	short vSize = 0;
	vector<CBinary>::iterator vIter;
	CBaseDataStream::CDsIdSendFrame dsSendFrame;
	CReceiveFrame ReceiveFrame;
	CBinaryGroup OneReceiveFrame;

	do
	{//����������ʼ
		if (m_bShowDataStreamSelectWindow)
		{
			CMultiSelectShow::CSelectedItemData aSelected;
			// ��������ʾ��ѡ����ѡ��

			CDisplay::GetInstance()->MultiSelect.Init("");

			for(vIter = paidDataStream->begin(); vIter != paidDataStream->end(); vIter++)
			{
				CDisplay::GetInstance()->MultiSelect.Add(*vIter);

			}
			if (!CDisplay::GetInstance()->MultiSelect.Show(aSelected))//�û��Ƿ�OK�����ǡ�true ��false
			{
				break;
			}

			// ���m_listDsIdSendFrame��m_aSendFrame����
			m_listDsIdSendFrame.clear();
			m_SendFrame.Clear();

			// ѡȡ��ѡ�������������������
			vSize = 0;
			for (vIter = paidDataStream->begin(); vIter != paidDataStream->end(); vIter++, vSize++)
			{
				if (aSelected.IsSelected(vSize))
				{
					dsSendFrame.idDataStream = *vIter;
				
					dsSendFrame.listDsContent=dsLib.SearchId(*vIter);  //����������

					if(dsSendFrame.listDsContent.size() < 5)
					{
						return iRet = NOITEMINDSLIB; 
					}

					//dsSendFrame.idUnit = dsSendFrame.listDsContent[1];//����ʾ����ת����string 

					dsSendFrame.i16SendFrameOrder = 0;	//���±���ȫ��ʼ��Ϊ0

					m_listDsIdSendFrame.push_back(dsSendFrame);
				}
			}
		}//if��ʾ��ѡ����
		// û�����ö�ѡ����ѡ���ȫ��ѡ��(���ڹ��ܲ˵���һѡ�����������Ͱ�����������ID�������ȥ)
		else  
		{
			for (vIter = paidDataStream->begin(); vIter != paidDataStream->end(); vIter++)
			{
				dsSendFrame.idDataStream = *vIter;
				DsBuff = dsLib.SearchId(*vIter);

				dsSendFrame.listDsContent=dsLib.SearchId(*vIter);  //����������

				if(dsSendFrame.listDsContent.size() < 5)
				{
					return iRet = NOITEMINDSLIB; 
				}

				//dsSendFrame.idUnit = dsSendFrame.listDsContent[1];//����ʾ����ת����string 

				dsSendFrame.i16SendFrameOrder = 0; //���±���ȫ��ʼ��Ϊ0

				m_listDsIdSendFrame.push_back(dsSendFrame);
			}
		}

		DsIdToSendFrame ();	
		
		// ��֯����������㷨
		// ѭ����������������ݼ���ʾ����

		bool bLoopDsProcess = true;
		list<CDsIdSendFrame>::iterator lIter;
		string strDsValue;

		OnBeforeSendDsCmd();

		do
		{
#if 1
			m_ReceiveFrame = CCommWithEcu::GetInstance()->SendReceive(m_SendFrame, true);//���Ͳ�ȡ�÷���ֵ,������ѭ��ִ�б�־
			
			if(0==m_ReceiveFrame.size())
			{
				//�з���֡δ�õ�Ӧ��ֱ�ӷ���?
				//����һ�����ֹͣ��������������
				CCommWithEcu::GetInstance()->GetStatus(GS_CHANNEL_SIGNAL);

				return -22;
			}
			
			if(m_ReceiveFrame.size() < m_SendFrame.GetFrameNumber())
			{
				//�з���֡δ�õ�Ӧ��ֱ�ӷ���?
				//����һ�����ֹͣ��������������
				CCommWithEcu::GetInstance()->GetStatus(GS_CHANNEL_SIGNAL);
				return -22;
			}

			lIter = m_listDsIdSendFrame.begin();
			if(lIter->i16SendFrameOrder>=m_ReceiveFrame.size())
			{
				//�з���֡δ�õ�Ӧ��ֱ�ӷ���?
				//����һ�����ֹͣ��������������
				CCommWithEcu::GetInstance()->GetStatus(GS_CHANNEL_SIGNAL);
				return -22;
			}

#else	//���Դ��룬����Ӧ��֡������		
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

				//���崨��ӣ�ԭ��û�е��ã��޷����㷨���������������û����� ExceptionArithmetic �Լ�����
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

							//���μ��빫ʽ����������ֵ��
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

							//���һ��
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

			// ��ʾ���ؽ��
			if (CDisplay::GetInstance()->DataStream.Show() == adsIDBACK)
			{
				bLoopDsProcess = false;
			}

			//Sleep(100);

		}while(bLoopDsProcess);

		//����һ�����ֹͣ��������������
		CCommWithEcu::GetInstance()->GetStatus(GS_CHANNEL_SIGNAL);

	}while(true);  // ������������


	return 1;
}

/*
��    �ܣ���������·���ֵ�ϵͳ�ؽ�ϵͳʹ��
����˵������
�� �� ֵ����
˵    ����������λ������������ͨ������֮ǰ���ã� */
void CStdDataStream::OnBeforeSendDsCmd ()
{

}
