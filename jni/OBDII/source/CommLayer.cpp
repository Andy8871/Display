// CommLayer.cpp: implementation of the CCommLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "DiagStd.h"
#include "CommWithEcu.h"
#include "Display.h"
#include "adsStd.h"
#include "MenuTree.h"
#include "Binary.h"
#include "Database.h"
#include "information.h"
#include "EcuCommException.h"
#include "DiagnosisUnit.h"
#include "RunEnvironment.h"

#include "CommLayer.h"
#include "AutoLog.h"

#include "CommonTools.h"
#include "DrivenStd.h"
#include "LogCat.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TAG "CCommLayer"
CCommLayer::CCommLayer()
{
	bCommStatus=Comm_Status_NA;
}

CCommLayer::~CCommLayer()
{

}

short CCommLayer::SetupComm()
{
	CBinary binIndex;
	short iRet=0;
	short iPos=0;

	bCommStatus=Comm_Status_NA;

	binIndex.Add(bBrand_ID);
	binIndex+=binCommECU_ID;
	binIndex.Add(bCommBUS_ID);
	binIndex.Add(0x01);
	binIndex.Add(0x00);

	while(1)
	{
		if(!CommonTools->ReadDBFile(Link_Info,"LINKLAYER.DB",binIndex))
		{
			return 0;
		}
		if(!InitLinkLayer())
		{
			return 0;
		}

		iRet=SendEnterSysCmd();
		if(iRet==Rece_Positive)
			break;
		binIndex.SetAt(6,binIndex[6]+1);
	}
	
	return iRet;
}

short CCommLayer::InitLinkLayer()
{
	short Num=0;
	
	Num=Link_Info.size();
	
	if(Num<31)
		return 0;

	g_pCommEcu->InitAndResetCOMM();
	
	//����Э������
	if(!SetProtocolID())
		return 0;
		
	//����ʱ�����
	if(!SetTimeParameter())
		return 0;
	
	//���ò�����
	if(!SetBaudrate())
		return 0;
	
	//����ͨѶ����
	if(!SetIOPort())
		return 0;

	//����CANBUS������֡
	SetFlowControl();
	
	return 1;
}


short CCommLayer::SetProtocolID()
{
	CBinary CBTemp;
	CCommWithEcu::CFilterOrLengthRecognise FilterCond;

	FilterCond.Empty();
	
	iPosReceTimeOut=0;
	iNegReceTimeOut=0;

	//���ý���֡������Ϣ
	SetFrameLength(FilterCond);

	//���ý��չ���
	SetReceFilter(FilterCond);

	switch(bCommBUS_ID)
	{
	case P_KWP2000_ON_K_LINE:
	case P_ED_ISO_14230_K_LINE:
		g_pCommEcu->ProtocolSet(PROTOCOL_KWP,&FilterCond);
		g_pCommEcu->SetNegativeHandleMethod(0,0x7f,2,0x78,1,500);
		iPosReceTimeOut=500;
		iNegReceTimeOut=5000;
		break;
	case P_KWP2000_ON_CAN:
	case P_UDS_ON_CAN:
		SetCanIDFilter(FilterCond,2);
		g_pCommEcu->ProtocolSet(PROTOCOL_CANBUS,&FilterCond);
		g_pCommEcu->SetNegativeHandleMethod(4,0x7f,6,0x78,1,150);
		iPosReceTimeOut=500;
		iNegReceTimeOut=5000;
		break;
	case P_ED_ON_CAN:
		SetCanIDFilter(FilterCond,4);
		g_pCommEcu->ProtocolSet(PROTOCOL_CANBUS,&FilterCond);
		g_pCommEcu->SetNegativeHandleMethod(5,0x7f,7,0x78,1,150);
		iPosReceTimeOut=500;
		iNegReceTimeOut=5000;
		break;
	case P_ED_ISO_9141_K_LINE:
		g_pCommEcu->ProtocolSet(PROTOCOL_ISO,&FilterCond);
		g_pCommEcu->SetNegativeHandleMethod(3,0x7f,5,0x78,1,150);
		break;
	case P_UDS_ON_K_LINE:
		g_pCommEcu->ProtocolSet(PROTOCOL_NORMAL,&FilterCond);
		g_pCommEcu->SetNegativeHandleMethod(3,0x7f,5,0x78,1,150);
		iPosReceTimeOut=500;
		iNegReceTimeOut=5000;
		break;
	case P_ED_VPW:
		g_pCommEcu->ProtocolSet(PROTOCOL_VPW,&FilterCond);
		g_pCommEcu->SetNegativeHandleMethod(3,0x7f,5,0x78,1,150);
		break;
	case P_ED_PWM:
		g_pCommEcu->ProtocolSet(PROTOCOL_PWM,&FilterCond);
		g_pCommEcu->SetNegativeHandleMethod(3,0x7f,5,0x78,1,150);
		break;
	default:
		break;
	}

	return 1;
}


short CCommLayer::SetFrameLength(CCommWithEcu::CFilterOrLengthRecognise &FilterCond)
{
	CBinary binPosition;
	CBinary binMask;
	CBinary binOffSet;
	short iPosition=0;
	short iOffSet=0;
	
	binPosition=Link_Info[Pos_FrameLengthPosition];
	binMask=Link_Info[Pos_Mark];
	binOffSet=Link_Info[Pos_Offset];
	
	if(!CommonTools->IsCBinaryAvailable(binPosition))
		return 0;
	if(!CommonTools->IsCBinaryAvailable(binMask))
		return 0;
	if(!CommonTools->IsCBinaryAvailable(binOffSet))
		return 0;
	
	iPosition=CommonTools->StrBinToINT16(binPosition);
	iOffSet=CommonTools->StrBinToINT16(binOffSet);
	FilterCond.SetNormalLengthRecogniseMode(iPosition,binMask[0],iOffSet);
	
	return 1;
}


short CCommLayer::SetReceFilter(CCommWithEcu::CFilterOrLengthRecognise &FilterCond)
{
	CBinary binPosition;
	CBinary binContain;
	vector<unsigned short> vecPostions;
	char *strContain=NULL;
	short iLen=0;
	short i=0;
	
	binPosition=Link_Info[Pos_FilterPositions];
	binContain=Link_Info[Pos_FilterContain];
	
	if(!CommonTools->IsCBinaryAvailable(binPosition))
		return 0;
	if(!CommonTools->IsCBinaryAvailable(binContain))
		return 0;
	
	if(!CommonTools->StrBinToVecInt(binPosition,vecPostions))
		return 0;
	iLen=binContain.GetSize();
	strContain=new char[iLen*4+1];
// 	for(i=0;i<iLen;i++)
// 	{
// 		sprintf(strContain+i*4,"\\x%02X",binContain[i]);
// 	}
	binContain.ReadBuffer(strContain);
	FilterCond.SetNormalFilterMode(vecPostions[0],vecPostions.size(),strContain);
	delete strContain;
	
	return 1;
}


short CCommLayer::SetCanIDFilter(CCommWithEcu::CFilterOrLengthRecognise &FilterCond,unsigned char IDLen)
{
	CBinary binCanFilterId;
	CBinary binECUCANID;
	short iLen=0;
	short i=0;
	short j=0;
	
	binECUCANID=Link_Info[Pos_ECUCANID];
	iLen=binECUCANID.GetSize();
	if(iLen<2)
		return 0;
	
	for(i=0;i<iLen/IDLen;i++)
	{
		binCanFilterId=NULL;
		for(j=0;j<IDLen;j++)
		{
			binCanFilterId.Add(binECUCANID.GetAt(i*IDLen+j));
		}
		FilterCond.AddCanFilterId(binCanFilterId);
	}
	
	return 1;
}


short CCommLayer::SetTimeParameter()
{
	CBinary CBTemp;
	unsigned int MaxCommTime=0;
	unsigned int ReceTimeOut=0;
	unsigned int SendWaitMinTime=0;
	double SendIntervalTime=0;
	
	//����ʱ�����
	CBTemp=Link_Info[Pos_MaxCommTime];
	if(!CommonTools->IsCBinaryAvailable(CBTemp))
		return 0;
	MaxCommTime=CommonTools->StrBinToUINT32(CBTemp);
	
	g_pCommEcu->WaitCommPacketMaxTime(MaxCommTime);
	
	CBTemp=Link_Info[Pos_ReceTimeOut];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		ReceTimeOut=CommonTools->StrBinToUINT32(CBTemp);
	}
	
	if(iPosReceTimeOut)
	{
		ReceTimeOut=iPosReceTimeOut;
	}
	
	CBTemp=Link_Info[Pos_SendWaitMinTime];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		SendWaitMinTime=CommonTools->StrBinToUINT32(CBTemp);
	}
	
	CBTemp=Link_Info[Pos_SendIntervalTime];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		SendIntervalTime=CommonTools->StrBinToUINT32(CBTemp);
	}
	
	g_pCommEcu->TimeIntervalOver(ReceTimeOut,SendWaitMinTime,SendIntervalTime,iNegReceTimeOut);
	
	return 1;
}


short CCommLayer::SetBaudrate()
{
	CBinary CBTemp;
	double Baudrate=9600;
	unsigned char ParityBit=PB_NONE;
	unsigned char DataBit=8;
	
	//���ò�����
	CBTemp=Link_Info[Pos_Baudrate];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		Baudrate=CommonTools->StrBinToFLOAT(CBTemp);
	}
	
	CBTemp=Link_Info[Pos_ParityBit];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		ParityBit=CommonTools->StrBinToBYTE(CBTemp);
	}
	
	CBTemp=Link_Info[Pos_DataBit];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		DataBit=CommonTools->StrBinToBYTE(CBTemp);
	}
	
	g_pCommEcu->SetBps(Baudrate,ParityBit,DataBit);
	
	return 1;
}


short CCommLayer::SetIOPort()
{
	unsigned char Send_Pin=DB15_PIN7;
	unsigned char Send_Logic=IOS_INPUT_POSITIVE_LOGIC;
	unsigned char Rece_Pin=DB15_PIN7;
	unsigned char Rece_Logic=IOS_OUTPUT_POSITIVE_LOGIC;
	unsigned char VoltageValue=12;
	unsigned char Other_Pin=0;
	unsigned char Control_Pin=0;
	CBinary CBTemp;
	short BYTE_Num=0;
	vector<unsigned short> vecIOPort;
	
	//��÷���������Ϣ
	CBTemp=Link_Info[Pos_Send_Pin];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		CommonTools->StrBinToVecInt(CBTemp,vecIOPort);
		BYTE_Num=vecIOPort.size();
		if(BYTE_Num)
		{
			Send_Pin=(unsigned char)vecIOPort[0];
			if(BYTE_Num==2)
			{
				if(vecIOPort[1]==0)
				{
					Send_Logic=IOS_INPUT_NEGATIVE_LOGIC;
				}
			}
			Rece_Pin=Send_Pin; //����������źͷ�������һ��
			//Rece_Logic=Rece_Logic; //��������߼��ͷ����߼�һ��
		}
	}
	
	//��ý���������Ϣ
	CBTemp=Link_Info[Pos_Rece_Pin];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		CommonTools->StrBinToVecInt(CBTemp,vecIOPort);
		BYTE_Num=vecIOPort.size();
		if(BYTE_Num)
		{
			Rece_Pin=(unsigned char)vecIOPort[0];
			if(BYTE_Num==2)
			{
				if(vecIOPort[1]==0)
				{
					Rece_Logic=IOS_INPUT_NEGATIVE_LOGIC;
				}
			}
		}
	}

	//��õ�ѹֵ
	CBTemp=Link_Info[Pos_VoltageValue];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		VoltageValue=CommonTools->StrBinToBYTE(CBTemp);
	}
	
	//��÷�K��Э���������Ϣ
	CBTemp=Link_Info[Pos_Other_Pin];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		Other_Pin=CommonTools->StrBinToBYTE(CBTemp);
		switch(Other_Pin)
		{
		case 1:
			Send_Pin=DB15_CAN_PIN6_14;
			Rece_Pin=DB15_CAN_PIN6_14;
			VoltageValue=5;
			break;
		case 2:
			Send_Pin=DB15_CAN_PIN3_11;
			Rece_Pin=DB15_CAN_PIN3_11;
			VoltageValue=5;
			break;
		case 3:
			Send_Pin=DB15_PIN1;
			Rece_Pin=DB15_PIN1;
			VoltageValue=5;
			break;
		case 4:
			Send_Pin=DB15_VPW;
			Rece_Pin=DB15_VPW;
			VoltageValue=8;
			//Send_Logic=IOS_INPUT_NEGATIVE_LOGIC;
			//Rece_Logic=IOS_OUTPUT_NEGATIVE_LOGIC;
			break;
		case 5:
			Send_Pin=DB15_PWM;
			Rece_Pin=DB15_PWM;
			VoltageValue=8;
			break;
		default:
			break;
		}
	}
		
	g_pCommEcu->SetIoPort(Send_Pin,Rece_Pin,VoltageValue,Send_Logic | Rece_Logic);
	
	//��ÿ����ߵ�������Ϣ
	CBTemp=Link_Info[Pos_Control_Pin];
	if(CommonTools->IsCBinaryAvailable(CBTemp))
	{
		Control_Pin=CommonTools->StrBinToBYTE(CBTemp);
		if(Control_Pin==0x0F)
			Control_Pin=0x05;
		g_pCommEcu->EnableOutputIoLine(Control_Pin);
	}

	return 1;
}


short CCommLayer::SetFlowControl()
{
	CCommWithEcu::CFilterOrLengthRecognise FilterCond;
	CSendFrame sf;
	CSendFrame::COneSendFrame OneSendFrm;
	CBinary binToolCANID=NULL;
	CBinary binPosition=NULL;
	CBinary binMask=NULL;
	CBinary binFrameID=NULL;
	CBinary binFCCmd=NULL;
	unsigned short iPosition=0;
	short i=0;
	
	binToolCANID=Link_Info[Pos_ToolCANID];
	binPosition=Link_Info[Pos_FlowControlPosition];
	binMask=Link_Info[Pos_FlowControlMask];
	binFrameID=Link_Info[Pos_FlowControlFrame];
	
	if(!CommonTools->IsCBinaryAvailable(binPosition))
		return 0;
	if(!CommonTools->IsCBinaryAvailable(binMask))
		return 0;
	if(!CommonTools->IsCBinaryAvailable(binFrameID))
		return 0;
	
	iPosition=CommonTools->StrBinToUINT16(binPosition);
	
	FilterCond.Empty();
	FilterCond.SetCanFilterMode(iPosition,binMask[0],iPosition+1);
	if(!MakeSendFrame(sf,binFrameID))
		return 0;
	//������֡��CANID
	OneSendFrm=sf[0];
	binFCCmd=OneSendFrm.m_binSendCmd;
	for(i=0;i<binToolCANID.GetSize();i++)
	{
		if(binFCCmd.GetSize()>(i+1))
		{
			binFCCmd.SetAt(i+1,binToolCANID[i]);
		}
	}
	OneSendFrm.m_binSendCmd=binFCCmd;
	sf.Clear();
	sf+=OneSendFrm;
	///////////////////
	g_pCommEcu->SetAutoSendContinuedFrame(FilterCond,sf);
	
	return 1;
}

short CCommLayer::SendEnterSysCmd()
{
	short iRet=0;
	CBinary binInitMode;
	CBinary binParameter;
	CBinary binAdrrType;
	CSendFrame SendFrame;
	CReceiveFrame ReceFrame;
	CBinaryGroup vecReceFrm;

	if(!MakeSendFrame(SendFrame,binEnterCmdID))
		return Rece_Error;

	binInitMode=Link_Info[Pos_Init_Mode];
	binParameter=Link_Info[Pos_Parameter];
	binAdrrType=Link_Info[Pos_AddressType];

	if(CommonTools->IsCBinaryAvailable(binInitMode))
	{
		vector<unsigned short> vecIntParameter;
		unsigned char bAddress=0x00;
		double fBPS=5.0;
		unsigned char bType=1;
		short iWaitTime=30;
		unsigned short iParameter=ACE_BPS_AUTO_RECOGNIZE | ACE_KW2_REVERSE_TO_ECU | ACE_GET_ADDRESS_REVERSE_FROM_ECU;

		switch(binInitMode[0])
		{
		case 1: //�͸����崥��
		{
			CommonTools->StrBinToVecInt(binParameter,vecIntParameter);
			int nParamLen = vecIntParameter.size();
			unsigned short* arrTemp = new unsigned short[nParamLen];
			for (int i = 0; i < nParamLen ; ++i)
			{
				arrTemp[i] = vecIntParameter[i];
			}
			g_pCommEcu->Begin();
			g_pCommEcu->VoltageHighLowTime(nParamLen,arrTemp);
			g_pCommEcu->SendReceive(SendFrame); 
			ReceFrame = g_pCommEcu->End();
			if (arrTemp != NULL)
			{
				delete []arrTemp;
				arrTemp = NULL;
			}
			break;
		}
		case 2: //��ַ�봥��
			if(CommonTools->IsCBinaryAvailable(binAdrrType))
			{
				bAddress=binAdrrType[0];
				if(binAdrrType.GetSize()>1)
					fBPS=binAdrrType[1];
				if(binAdrrType.GetSize()>2)
					bType=binAdrrType[2];
				if(binAdrrType.GetSize()>3)
					iWaitTime=binAdrrType[3];
				g_pCommEcu->Begin();
				switch(bType)
				{
				case 1:
					iParameter=ACE_BPS_AUTO_RECOGNIZE | ACE_KW2_REVERSE_TO_ECU | ACE_GET_ADDRESS_REVERSE_FROM_ECU;
					break;
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				default:
					break;
				}
				g_pCommEcu->VoltageHighLowTime(1,500);
				g_pCommEcu->AddressCodeEnter(bAddress,iParameter,fBPS,iWaitTime);
				ReceFrame = g_pCommEcu->End();
				if(!ReceFrame.size())
					return Rece_Error;
			}
			ReceFrame=g_pCommEcu->SendReceive(SendFrame);
			break;
		default:
			ReceFrame=g_pCommEcu->SendReceive(SendFrame);
			break;
		}
	}
	else
	{
		ReceFrame=g_pCommEcu->SendReceive(SendFrame);
	}

	
	if(!ParseReveFrame(ReceFrame,vecReceFrm,SendFrame.GetFrameNumber()))
	{
		return Rece_Error;
	}

	bCommStatus=Comm_Status_General;

	return Rece_Positive;
}

short CCommLayer::SendExitSysCmd()
{
	CBinary binIndex=NULL;
	CBinary binCMD_ID=NULL;
	CBinaryGroup vecItemInfo;
	CBinaryGroup vecReceFrm;
	CSendFrame SendFrame;
	short iPos=0;

	binIndex.Add(bBrand_ID);
	binIndex+=binCommECU_ID;
	binIndex.Add(0x00);
	iPos=binIndex.GetSize()-1;

	while(1)
	{
		binIndex.SetAt(iPos,binIndex[iPos]+1);
		if(!CommonTools->ReadDBFile(vecItemInfo,"DIAMODE.DB",binIndex))
			break;
		if(vecItemInfo[2][0]==Comm_Status_NA)
		{
			binCMD_ID=vecItemInfo[0];
			if(MakeSendFrame(SendFrame,binCMD_ID))
			{
				SendCommand(SendFrame,vecReceFrm);
			}
		}
	}

	bCommStatus=Comm_Status_NA;

	return Rece_Positive;
}

short CCommLayer::MakeSendFrame(CSendFrame &SendFrame,CBinary binCmdID)
{
	CBinaryGroup vecItemInfo;

	if(!GetCommandInfo(binCmdID,vecItemInfo))
		return 0;
	
	return MakeSendFrame(SendFrame,vecItemInfo);
}

short CCommLayer::MakeSendFrame(CSendFrame &SendFrame,CBinaryGroup vecCmdInfo)
{
	CBinary binCmdInfo=NULL;

	if(!GetbinCmdInfo(vecCmdInfo,binCmdInfo))
		return 0;

	return MakeSendFrameCmd(SendFrame,binCmdInfo);
}

short CCommLayer::GetbinCmdInfo(CBinaryGroup vecCmdInfo,CBinary &binCmdInfo)
{
	CBinary binSendCmd=NULL;
	CBinary binCmdType=NULL;
	CBinary binSID=NULL;
	CBinary binPIDs=NULL;
	CBinary binReceNum=NULL;
	short iReceNum=1;
	unsigned char bType=0x00;
	
	binCmdInfo=NULL;
	if(vecCmdInfo.size()<4)
	{
		return 0;
	}
	binCmdType=vecCmdInfo[0];
	binSID=vecCmdInfo[1];
	binPIDs=vecCmdInfo[2];
	binReceNum=vecCmdInfo[3];
	
	if(CommonTools->IsCBinaryAvailable(binCmdType))
	{
		bType=binCmdType[0];
	}
	if(CommonTools->IsCBinaryAvailable(binSID))
	{
		binSendCmd+=binSID;
	}
	if(CommonTools->IsCBinaryAvailable(binPIDs))
	{
		binSendCmd+=binPIDs;
	}
	if(CommonTools->IsCBinaryAvailable(binReceNum))
	{
		iReceNum=binReceNum[0];
	}
	
	binCmdInfo.Add(bType);
	binCmdInfo+=binSendCmd;
	binCmdInfo.Add(iReceNum);

	return 1;
}

short CCommLayer::MakeSendFrameCmd(CSendFrame &SendFrame,CBinary binCmdInfo)
{
	short iDataLen=0;
	CBinary binSendCmd=NULL;
	short iReceNum=1;
	unsigned char bType=0x00;
	CSendFrame::COneSendFrame OneSendFrm;
	CBinary binTemp=NULL;
	short i=0;

	if(Demo_Flag==true)
	{
		return 1;
	}

	iDataLen=binCmdInfo.GetSize();
	
	if(iDataLen<3)
		return 0;

	bType=binCmdInfo[0];
	iReceNum=binCmdInfo[iDataLen-1];
	binSendCmd=NULL;
	for(i=0;i<(iDataLen-2);i++)
	{
		binSendCmd.Add(binCmdInfo[1+i]);
	}
	
	if(bType==0x00) // ��֡
	{
		if(!CombineSendFrame(SendFrame,binSendCmd,iReceNum))
			return 0;
	}
	else
	{
		VerifyCommand(binSendCmd);
		switch(bCommBUS_ID)
		{
		case P_KWP2000_ON_CAN:
		case P_UDS_ON_CAN:
			binTemp=binSendCmd;
			binSendCmd=NULL;
			binSendCmd.Add(binTemp.GetAt(2));
			for(i=0;i<binTemp.GetSize();i++)
			{
				if(i!=2)
				{
					binSendCmd.Add(binTemp.GetAt(i));
				}
			}
			break;
		default:
			break;
		}
		OneSendFrm.m_binSendCmd=binSendCmd;
		OneSendFrm.m_iReceiveFrameNumber=iReceNum;
		SendFrame+=OneSendFrm;
	}
	
	return 1;
}

short CCommLayer::CombineSendFrame(CSendFrame &SendFrame,CBinary binSendData,short iReceNum)
{
	CSendFrame::COneSendFrame OneSendFrm;
	short i=0;
	short iDataLen=0;
	CBinary binTempData=NULL;
	CBinary binPackHeader=NULL;
	short iPackHeader=0;
	CBinary binFinalCmd=NULL;
	unsigned char bToolAddr=0x00;
	unsigned char bECUAddr=0x00;
	CBinary binToolCANID=NULL;
	CBinary binCANDefaultByte=NULL;
	short iPos=0;
	short iFrameNum=0;
	unsigned char bCtrlIndex=0x00;

	SendFrame.Clear();
	iDataLen=binSendData.GetSize();
	if(!iDataLen)
		return 0;
	switch(bCommBUS_ID)
	{
	case P_KWP2000_ON_K_LINE:
		binPackHeader=Link_Info[Pos_PackageHeader];
		iPackHeader=PackageHeader_KWP2000_8X;
		if(CommonTools->IsCBinaryAvailable(binPackHeader))
		{
			iPackHeader=binPackHeader[0];
		}
		bToolAddr=Link_Info[Pos_ToolAddress][0];
		bECUAddr=Link_Info[Pos_ECUAddress][0];
		switch(iPackHeader)
		{
		case PackageHeader_KWP2000_0X:
			binFinalCmd=NULL;
			binFinalCmd.Add(iDataLen);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			break;
		case PackageHeader_KWP2000_00:
			binFinalCmd=NULL;
			binFinalCmd.Add(0x00);
			binFinalCmd.Add(iDataLen);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			break;
		case PackageHeader_KWP2000_8X:
			binFinalCmd=NULL;
			binFinalCmd.Add(0x80+iDataLen);
			binFinalCmd.Add(bECUAddr);
			binFinalCmd.Add(bToolAddr);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			break;
		case PackageHeader_KWP2000_80:
			binFinalCmd=NULL;
			binFinalCmd.Add(0x80);
			binFinalCmd.Add(bECUAddr);
			binFinalCmd.Add(bToolAddr);
			binFinalCmd.Add(iDataLen);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			break;
		case PackageHeader_KWP2000_CX:
			binFinalCmd=NULL;
			binFinalCmd.Add(0xC0+iDataLen);
			binFinalCmd.Add(bECUAddr);
			binFinalCmd.Add(bToolAddr);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			break;
		default:
			break;
		}
		binFinalCmd.Add(0x00);
		VerifyCommand(binFinalCmd);
		OneSendFrm.m_binSendCmd=binFinalCmd;
		OneSendFrm.m_iReceiveFrameNumber=iReceNum;
		SendFrame+=OneSendFrm;
		break;
	case P_ED_ISO_9141_K_LINE:
		bToolAddr=Link_Info[Pos_ToolAddress][0];
		binFinalCmd=NULL;
		binFinalCmd.Add(0x68);
		binFinalCmd.Add(0x6A);
		binFinalCmd.Add(bToolAddr);
		for(i=0;i<iDataLen;i++)
		{
			binFinalCmd.Add(binSendData[i]);
		}
		binFinalCmd.Add(0x00);
		VerifyCommand(binFinalCmd);
		OneSendFrm.m_binSendCmd=binFinalCmd;
		OneSendFrm.m_iReceiveFrameNumber=iReceNum;
		SendFrame+=OneSendFrm;
		break;
	case P_UDS_ON_K_LINE:
		bToolAddr=Link_Info[Pos_ToolAddress][0];
		bECUAddr=Link_Info[Pos_ECUAddress][0];
		binFinalCmd=NULL;
		binFinalCmd.Add((iDataLen+3)*16+0x04);
		binFinalCmd.Add(bECUAddr);
		binFinalCmd.Add(bToolAddr);
		for(i=0;i<iDataLen;i++)
		{
			binFinalCmd.Add(binSendData[i]);
		}
		binFinalCmd.Add(0x00);
		VerifyCommand(binFinalCmd);
		OneSendFrm.m_binSendCmd=binFinalCmd;
		OneSendFrm.m_iReceiveFrameNumber=iReceNum;
		SendFrame+=OneSendFrm;
		break;
	case P_KWP2000_ON_CAN:
	case P_UDS_ON_CAN:
	case P_ED_ON_CAN:
		binToolCANID=Link_Info[Pos_ToolCANID];
		binCANDefaultByte=Link_Info[Pos_CAN_Defualt_BYTE];
		if(!CommonTools->IsCBinaryAvailable(binToolCANID))
			return 0;
		if(binToolCANID.GetSize()<2)
			return 0;
		if(iDataLen<8)
		{
			binFinalCmd=NULL;
			binFinalCmd.Add(0x00);
			binFinalCmd+=binToolCANID;
			binFinalCmd.Add(iDataLen);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			if(!CommonTools->IsCBinaryAvailable(binCANDefaultByte))
			{
				if(binToolCANID.GetSize()>2)
					binFinalCmd.SetAt(0,0x80+iDataLen+1); //û������ֽڣ���ʾ�䳤
				else
					binFinalCmd.SetAt(0,iDataLen+1); //û������ֽڣ���ʾ�䳤
			}
			else
			{
				if(binToolCANID.GetSize()>2)
				{
					binFinalCmd.SetAt(0,0x88); //������ֽڣ���ʾ����
				}
				else
				{
					binFinalCmd.SetAt(0,0x08);
				}
				for(;i<7;i++)
				{
					binFinalCmd.Add(binCANDefaultByte[0]);
				}
			}
			OneSendFrm.m_binSendCmd=binFinalCmd;
			OneSendFrm.m_iReceiveFrameNumber=iReceNum;
			SendFrame+=OneSendFrm;
		}
		else
		{
			binFinalCmd=NULL;
			binFinalCmd.Add(0x08);
			binFinalCmd+=binToolCANID;
			binFinalCmd.Add((0x10+(iDataLen>>8))&0x1f);
			binFinalCmd.Add(iDataLen&0xff);
			for(i=0;i<6;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}			
			iPos+=6;
			OneSendFrm.m_binSendCmd=binFinalCmd;
			OneSendFrm.m_iReceiveFrameNumber=1;
			SendFrame+=OneSendFrm;
			
			iDataLen=iDataLen-6;
			iFrameNum=iDataLen/7;
			if(iDataLen%7)
				iFrameNum++;
			for(i=0;i<iFrameNum-1;i++)
			{
				binFinalCmd=NULL;
				binFinalCmd.Add(0x08);
				binFinalCmd+=binToolCANID;
				bCtrlIndex=0x21+i;
				if(bCtrlIndex>0x2f)
					bCtrlIndex=0x21;
				binFinalCmd.Add(bCtrlIndex);
				for(i=0;i<7;i++)
				{
					binFinalCmd.Add(binSendData[iPos+i]);
				}
				iPos+=7;
				OneSendFrm.m_binSendCmd=binFinalCmd;
				OneSendFrm.m_iReceiveFrameNumber=0;
				SendFrame+=OneSendFrm;
			}
			
			iDataLen=iDataLen-i*7;
			
			binFinalCmd=NULL;
			binFinalCmd.Add(0x00);
			binFinalCmd+=binToolCANID;
			bCtrlIndex=0x21+i;
			if(bCtrlIndex>0x2f)
				bCtrlIndex=0x21;
			binFinalCmd.Add(bCtrlIndex);
			
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[iPos+i]);
			}
			
			if(!CommonTools->IsCBinaryAvailable(binCANDefaultByte))
			{
				if(binToolCANID.GetSize()>2)
					binFinalCmd.SetAt(0,0x80+iDataLen+1); //û������ֽڣ���ʾ�䳤
				else
					binFinalCmd.SetAt(0,iDataLen+1); //û������ֽڣ���ʾ�䳤
			}
			else
			{
				if(binToolCANID.GetSize()>2)
					binFinalCmd.SetAt(0,0x88); //������ֽڣ���ʾ����
				else
					binFinalCmd.SetAt(0,0x08); //������ֽڣ���ʾ����
				for(;i<7;i++)
				{
					binFinalCmd.Add(binCANDefaultByte[0]);
				}
			}
			OneSendFrm.m_binSendCmd=binFinalCmd;
			OneSendFrm.m_iReceiveFrameNumber=iReceNum;
			SendFrame+=OneSendFrm;
		}
		break;
	case P_ED_VPW:
		if(bBrand_ID==0x01) // OBDII
		{
			bToolAddr=Link_Info[Pos_ToolAddress][0];
			binFinalCmd=NULL;
			binFinalCmd.Add(0x68);
			binFinalCmd.Add(0x6A);
			binFinalCmd.Add(bToolAddr);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			binFinalCmd.Add(0x00);
			VerifyCommand(binFinalCmd);
			OneSendFrm.m_binSendCmd=binFinalCmd;
			OneSendFrm.m_iReceiveFrameNumber=iReceNum;
			SendFrame+=OneSendFrm;
		}
		else
		{
			bToolAddr=Link_Info[Pos_ToolAddress][0];
			bECUAddr=Link_Info[Pos_ECUAddress][0];
			binFinalCmd=NULL;
			binFinalCmd.Add(0xC4);
			binFinalCmd.Add(bECUAddr);
			binFinalCmd.Add(bToolAddr);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			binFinalCmd.Add(0x00);
			VerifyCommand(binFinalCmd);
			OneSendFrm.m_binSendCmd=binFinalCmd;
			OneSendFrm.m_iReceiveFrameNumber=iReceNum;
			SendFrame+=OneSendFrm;
		}
		break;
	case P_ED_PWM:
		if(bBrand_ID==0x01) // OBDII
		{
			bToolAddr=Link_Info[Pos_ToolAddress][0];
			binFinalCmd=NULL;
			binFinalCmd.Add(0x61);
			binFinalCmd.Add(0x6A);
			binFinalCmd.Add(bToolAddr);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			binFinalCmd.Add(0x00);
			VerifyCommand(binFinalCmd);
			OneSendFrm.m_binSendCmd=binFinalCmd;
			OneSendFrm.m_iReceiveFrameNumber=iReceNum;
			SendFrame+=OneSendFrm;
		}
		else
		{
			bToolAddr=Link_Info[Pos_ToolAddress][0];
			bECUAddr=Link_Info[Pos_ECUAddress][0];
			binFinalCmd=NULL;
			binFinalCmd.Add(0xC4);
			binFinalCmd.Add(bECUAddr);
			binFinalCmd.Add(bToolAddr);
			for(i=0;i<iDataLen;i++)
			{
				binFinalCmd.Add(binSendData[i]);
			}
			binFinalCmd.Add(0x00);
			VerifyCommand(binFinalCmd);
			OneSendFrm.m_binSendCmd=binFinalCmd;
			OneSendFrm.m_iReceiveFrameNumber=iReceNum;
			SendFrame+=OneSendFrm;
		}
		break;
	default:
		break;
	}
	
	return 1;
}


short CCommLayer::VerifyCommand(CBinary &Command)
{
	unsigned int Sum=0;
	unsigned char BYTE_Temp=0;
	short i=0;
	short iDataLen=0;
	short iVerifyMethod=0;
	CBinary binVeryfy=NULL;
	
	binVeryfy=Link_Info[Pos_VerifyMethod];
	if(CommonTools->IsCBinaryAvailable(binVeryfy))
	{
		iVerifyMethod=binVeryfy[0];
	}
	else
	{
		switch(bCommBUS_ID)
		{
		case P_KWP2000_ON_K_LINE:
		case P_ED_ISO_14230_K_LINE:
		case P_ED_ISO_9141_K_LINE:
		case P_UDS_ON_K_LINE:
			iVerifyMethod=Verify_CheckSum;
			break;
		case P_ED_VPW:
		case P_ED_PWM:
			iVerifyMethod=Verify_CRC;
			break;
		default:
			break;
		}
	}
	iDataLen=Command.GetSize();
	if(iDataLen<2)
		return 1;
	switch(iVerifyMethod)
	{
	case Verify_CheckSum:
		iDataLen=iDataLen-1;
		for(i=0;i<iDataLen;i++)
		{
			Sum+=Command.GetAt(i);
		}
		BYTE_Temp=Sum&0xff;
		Command.SetAt(iDataLen,BYTE_Temp);
		break;
	case Verify_CheckSum_Reverse:
		iDataLen=iDataLen-1;
		for(i=0;i<iDataLen;i++)
		{
			Sum+=Command.GetAt(i);
		}
		BYTE_Temp=Sum&0xff;
		BYTE_Temp=~BYTE_Temp;
		Command.SetAt(iDataLen,BYTE_Temp);
		break;
	case Verify_CheckSum_Reverse_P1:
		iDataLen=iDataLen-1;
		for(i=0;i<iDataLen;i++)
		{
			Sum+=Command.GetAt(i);
		}
		BYTE_Temp=Sum&0xff;
		BYTE_Temp=~BYTE_Temp+1;
		Command.SetAt(iDataLen,BYTE_Temp);
		break;
	case Verify_Sum_TowBytes_Low:
		iDataLen=iDataLen-2;
		for(i=0;i<iDataLen;i++)
		{
			Sum+=Command.GetAt(i);
		}
		BYTE_Temp=Sum&0xff;
		Command.SetAt(iDataLen,BYTE_Temp);
		BYTE_Temp=(Sum>>8)&0xff;
		Command.SetAt(iDataLen+1,BYTE_Temp);
		break;
	case Verify_Sum_TowBytes_High:
		iDataLen=iDataLen-2;
		for(i=0;i<iDataLen;i++)
		{
			Sum+=Command.GetAt(i);
		}
		BYTE_Temp=(Sum>>8)&0xff;
		Command.SetAt(iDataLen,BYTE_Temp);
		BYTE_Temp=Sum&0xff;
		Command.SetAt(iDataLen+1,BYTE_Temp);
		break;
	case Verify_XOR:
		break;
	case Verify_XOR_9Bits:
		break;
	case Verify_CRC:
		VerifyCMD_CRC(Command);
		break;
	case Verify_CheckSum_Reverse_M1:
		iDataLen=iDataLen-1;
		for(i=0;i<iDataLen;i++)
		{
			Sum+=Command.GetAt(i);
		}
		BYTE_Temp=Sum&0xff;
		BYTE_Temp=~BYTE_Temp-1;
		Command.SetAt(iDataLen,BYTE_Temp);
		break;
	default:
		break;
	}
	return 1;
}

short CCommLayer::VerifyCMD_CRC(CBinary &Command)
{
	unsigned char crc,ch[8],ch1;
	short i,j,k;
	short iLen=0;

	iLen=Command.GetSize();

	if(iLen<4)
		return 0;
	
	crc = 0xff;
	for (i=0;i<iLen-1;i++)
	{
		ch1 = Command.GetAt(i);
		for (j=0;j<8;j++)
		{
			ch[j] = ch1 & 0x01;
			ch1 >>= 1;
		}
		for (k=0;k<8;k++)
		{
			ch[7-k] <<=7;
			if ((crc ^ ch[7-k]) & 0x80)
				crc = (crc << 1)^0x1d ;
			else
				crc <<= 1;
		}
	}
	crc &= 0xff;
	crc ^= 0xff;

	Command.SetAt(iLen-1,crc);

	return  1;
}

short CCommLayer::GetCommandInfo(CBinary binCmdID,CBinaryGroup &vecCmdInfo)
{
	CBinary binIndex=NULL;
	
	binIndex.Add(bBrand_ID);
	binIndex+=binCmdID;
	binIndex.Add(0x00);

	if(!CommonTools->ReadDBFile(vecCmdInfo,"COMMAND.DB",binIndex))
		return 0;

	return 1;
}

short CCommLayer::SendReceive(CBinary binCmdID,CBinaryGroup &vecReceFrm)
{
	CBinaryGroup vecItemInfo;
	CSendFrame SendFrame;
	short i=0;
	short iRet=0;

	if(Demo_Flag==true)
	{
		CBinary binDemoData=NULL;
		binDemoData.Add(0x61);
		for(i=0;i<200;i++)
		{
			binDemoData.Add(rand()&0xFF);
		}
		vecReceFrm.push_back(binDemoData);
		return Rece_Positive;
	}

	if(!GetCommandInfo(binCmdID,vecItemInfo))
		return Rece_Error;
	if(!MatchCommMode(vecItemInfo))
		return Rece_Error;
	if(!MakeSendFrame(SendFrame,vecItemInfo))
		return Rece_Error;

	for(i=0;i<2;i++)
	{
		iRet=SendCommand(SendFrame,vecReceFrm);
		if(iRet!=Rece_Error)
			break;
	}

	return iRet;
}

short CCommLayer::SendReceiveCmd(CBinary binCmdInfo,CBinaryGroup &vecReceFrm)
{
	CSendFrame SendFrame;
	short i=0;
	short iRet=0;
	
	if(Demo_Flag==true)
	{
		CBinary binDemoData=NULL;
		binDemoData.Add(0x61);
		for(i=0;i<200;i++)
		{
			binDemoData.Add(rand()&0xFF);
		}
		vecReceFrm.push_back(binDemoData);
		return Rece_Positive;
	}

	if(!MakeSendFrameCmd(SendFrame,binCmdInfo))
		return Rece_Error;
	
	for(i=0;i<2;i++)
	{
		iRet=SendCommand(SendFrame,vecReceFrm);
		if(iRet!=Rece_Error)
			break;
	}
	
	return iRet;
}

short CCommLayer::SendCommand(CSendFrame SendFrame,CBinaryGroup &vecReceFrm)
{
	CReceiveFrame ReceFrame;
	bool Rece_Flag=true;
	short iRece_Result=0;
	short iRetryTime=0;
	short iTryMaxTime=100;

	ReceFrame=g_pCommEcu->SendReceive(SendFrame);
	
	while(Rece_Flag)
	{
		if(!ParseReveFrame(ReceFrame,vecReceFrm,SendFrame.GetFrameNumber()))
		{
			iRece_Result=Rece_Error;
			break;
		}
		iRece_Result=CheckReceFrm(vecReceFrm);
		switch(iRece_Result)
		{
		case Rece_Negative_KeepRece:
			iRece_Result=Rece_Negative;
			Rece_Flag=false;
			break;
		case Rece_Negative_KeepSend:
			iTryMaxTime=150;
			if(iRetryTime>iTryMaxTime)
			{
				iRece_Result=Rece_Negative;
				Rece_Flag=false;
				break;
			}
			ReceFrame=g_pCommEcu->SendReceive(SendFrame);
			if(!ReceFrame.size())
			{
				iRece_Result=Rece_Negative;
				Rece_Flag=false;
				break;
			}
			break;
		case Rece_Negative_ReEnter:
			if(iRetryTime>0)
			{
				iRece_Result=Rece_Negative;
				Rece_Flag=false;
				break;
			}
			if(SendEnterSysCmd()==Rece_Positive)
			{
				ReceFrame=g_pCommEcu->SendReceive(SendFrame);
			}
			iRetryTime++;
			break;
		case Rece_Positive:
		case Rece_Negative:
			Rece_Flag=false;
			break;
		default:
			break;
		}
	}

	return iRece_Result;
}

short CCommLayer::MatchCommMode(CBinaryGroup vecCmdInfo)
{
	unsigned char bCurStatus=Comm_Status_General;
	short iRet=0;

	if(CommonTools->IsCBinaryAvailable(vecCmdInfo[4]))
	{
		binCurECU_ID=vecCmdInfo[4];
	}
	if(CommonTools->IsCBinaryAvailable(vecCmdInfo[5]))
	{
		bCurBUS_ID=vecCmdInfo[5][0];
	}
	if(CommonTools->IsCBinaryAvailable(vecCmdInfo[6]))
	{
		bCurStatus=vecCmdInfo[6][0];
	}
	
	if((!CommonTools->CBinaryCompare(binCurECU_ID,binCommECU_ID))||(bCurBUS_ID!=bCommBUS_ID))
	{
		ChangeCommStatus(bCurStatus,Comm_Status_NA); // ExitSys
		binCommECU_ID=binCurECU_ID;
		bCommBUS_ID=bCurBUS_ID;
		if(!GetEnterCmdID())
		{
			return Rece_Error;
		}
		iRet=ChangeCommStatus(Comm_Status_NA,bCurStatus);
	}
	else
	{
		iRet=ChangeCommStatus(bCommStatus,bCurStatus);
	}

	return iRet;
}

short CCommLayer::ParseReveFrame(CReceiveFrame ReceFrame,vector<CBinary> &vecReceFrm,short iIndex)
{
	vector<CBinary> vecFrame;
	CBinary binReceCmd=NULL;
	CBinary binBuf=NULL;
	short iRece_Len=0;
	short iDataLen=0;
	unsigned char rFrame[300];
	unsigned char bPos=0;
	short i=0;
	unsigned char bTypeByte=0;

	if(!ReceFrame.size())
		return 0;
	
	vecReceFrm.clear();

	switch(bCommBUS_ID)
	{
	case P_KWP2000_ON_K_LINE:
		vecFrame=ReceFrame.GetOneFrameAnswer(0);
		for(i=0;i<vecFrame.size();i++)
		{
			binReceCmd=vecFrame[i];
			iRece_Len=binReceCmd.GetSize();
			binReceCmd.ReadBuffer(rFrame,0,iRece_Len);
			if(rFrame[0]==0x80)
			{
				bPos=4;
				iDataLen=rFrame[3];
				binBuf=NULL;
				binBuf.WriteBuffer(rFrame+4,iDataLen);
				vecReceFrm.push_back(binBuf);
			}
			else if(rFrame[0]>0x80)
			{
				bPos=3;
				iDataLen=rFrame[0]-0x80;
				binBuf=NULL;
				binBuf.WriteBuffer(rFrame+3,iDataLen);
				vecReceFrm.push_back(binBuf);
			}
			else if(rFrame[0]==0x00)
			{
				bPos=2;
				iDataLen=rFrame[1];
				binBuf=NULL;
				binBuf.WriteBuffer(rFrame+2,iDataLen);
				vecReceFrm.push_back(binBuf);
			}
			else
			{
				bPos=1;
				iDataLen=rFrame[0];
				binBuf=NULL;
				binBuf.WriteBuffer(rFrame+1,iDataLen);
				vecReceFrm.push_back(binBuf);
			}
		}
		break;
	case P_UDS_ON_K_LINE:
		vecFrame=ReceFrame.GetOneFrameAnswer(0);
		for(i=0;i<vecFrame.size();i++)
		{
			binReceCmd=vecFrame[i];
			iRece_Len=binReceCmd.GetSize();
			binReceCmd.ReadBuffer(rFrame,0,iRece_Len);
			binBuf=NULL;
			binBuf.WriteBuffer(rFrame+3,iRece_Len-4);
			vecReceFrm.push_back(binBuf);
		}
		break;
	case P_KWP2000_ON_CAN:
	case P_UDS_ON_CAN:
		vecFrame=ReceFrame.GetOneFrameAnswer(iIndex-1);
		binBuf=NULL;
		for(i=0;i<vecFrame.size();i++)
		{
			binReceCmd=vecFrame[i];
			iRece_Len=binReceCmd.GetSize();
			binReceCmd.ReadBuffer(rFrame,0,iRece_Len);
			if(iRece_Len!=(rFrame[0]+3))
				continue;
			bTypeByte=rFrame[3];
			switch(bTypeByte&0x30)
			{
			case 0x00:
				iDataLen=bTypeByte;
				binBuf.WriteBuffer(rFrame+4,iDataLen);
				vecReceFrm.push_back(binBuf);
				binBuf=NULL;
				break;
			case 0x10:
				if(iRece_Len!=11)
					break;
				iDataLen=(bTypeByte&0x0f)*256+rFrame[4];
				if(iDataLen<8)
					break;
				binBuf=NULL;
				binBuf.WriteBuffer(rFrame+5,6);
				iDataLen=iDataLen-6;
				break;
			case 0x20:
				if(binBuf==NULL)
					break;
				if(iDataLen<=7)
				{
					for(short j=0;j<iDataLen;j++)
					{
						binBuf.Add(rFrame[j+4]);
					}
					vecReceFrm.push_back(binBuf);
					binBuf=NULL;
				}
				else
				{
					for(short j=0;j<7;j++)
					{
						binBuf.Add(rFrame[j+4]);
					}
					iDataLen=iDataLen-7;
				}
				break;
			case 0x30:
				break;
			}
		}
		break;
	case P_ED_ISO_14230_K_LINE:
		break;
	case P_ED_ISO_9141_K_LINE:
		vecFrame=ReceFrame.GetOneFrameAnswer(0);
		for(i=0;i<vecFrame.size();i++)
		{
			binReceCmd=vecFrame[i];
			iRece_Len=binReceCmd.GetSize();
			binReceCmd.ReadBuffer(rFrame,0,iRece_Len);
			binBuf=NULL;
			binBuf.WriteBuffer(rFrame+3,iRece_Len-4);
			vecReceFrm.push_back(binBuf);
		}
		break;
	case P_ED_ON_CAN:
		vecFrame=ReceFrame.GetOneFrameAnswer(iIndex-1);
		binBuf=NULL;
		for(i=0;i<vecFrame.size();i++)
		{
			binReceCmd=vecFrame[i];
			iRece_Len=binReceCmd.GetSize();
			binReceCmd.ReadBuffer(rFrame,0,iRece_Len);
			if(iRece_Len!=(rFrame[0]+5))
				continue;
			bTypeByte=rFrame[5];
			switch(bTypeByte&0x30)
			{
			case 0x00:
				iDataLen=bTypeByte;
				binBuf.WriteBuffer(rFrame+6,iDataLen);
				vecReceFrm.push_back(binBuf);
				binBuf=NULL;
				break;
			case 0x10:
				if(iRece_Len!=13)
					break;
				iDataLen=(bTypeByte&0x0f)*256+rFrame[6];
				if(iDataLen<8)
					break;
				binBuf=NULL;
				binBuf.WriteBuffer(rFrame+7,6);
				iDataLen=iDataLen-6;
				break;
			case 0x20:
				if(binBuf==NULL)
					break;
				if(iDataLen<=7)
				{
					for(short j=0;j<iDataLen;j++)
					{
						binBuf.Add(rFrame[j+6]);
					}
					vecReceFrm.push_back(binBuf);
					binBuf=NULL;
				}
				else
				{
					for(short j=0;j<7;j++)
					{
						binBuf.Add(rFrame[j+6]);
					}
					iDataLen=iDataLen-7;
				}
				break;
			case 0x30:
				break;
			}
		}
		break;
	case P_ED_VPW:
	case P_ED_PWM:
		vecFrame=ReceFrame.GetOneFrameAnswer(0);
		for(i=0;i<vecFrame.size();i++)
		{
			binReceCmd=vecFrame[i];
			iRece_Len=binReceCmd.GetSize();
			if(iRece_Len<5)
				continue;
			binReceCmd.ReadBuffer(rFrame,0,iRece_Len);
			binBuf=NULL;
			binBuf.WriteBuffer(rFrame+3,iRece_Len-4);
			vecReceFrm.push_back(binBuf);
		}
		break;
	default:
		break;
	}

	return vecReceFrm.size();
}

short CCommLayer::CheckReceFrm(CBinaryGroup &vecReceFrm)
{
	vector<CBinary> vecReceTemp;
	CBinary binReceData=NULL;
	short iFrmNum=0;
	short iReceLen=0;
	short i=0;
	
	iFrmNum=vecReceFrm.size();
	vecReceTemp.clear();
	iRece_Result=Rece_Positive;
	
	switch(bCommBUS_ID)
	{
	case P_KWP2000_ON_K_LINE:
	case P_ED_ISO_14230_K_LINE:
	case P_KWP2000_ON_CAN:
	case P_UDS_ON_CAN:
	case P_ED_ON_CAN:
	case P_ED_ISO_9141_K_LINE:
	case P_UDS_ON_K_LINE:
	case P_ED_VPW:
	case P_ED_PWM:
		for(i=0;i<iFrmNum;i++)
		{
			binReceData=vecReceFrm[i];
			iReceLen=binReceData.GetSize();
			if((iReceLen>0)&&(binReceData[0]==0x7F))
			{
				iRece_Result=Rece_Negative;
				if(iReceLen>2)
				{
					switch(binReceData[2])
					{
					case 0x78:
						iRece_Result=Rece_Negative_KeepRece;
						break;
					case 0x21:
					case 0x23:
						iRece_Result=Rece_Negative_KeepSend;
						break;
					case 0x80:
						iRece_Result=Rece_Negative_ReEnter;
						break;
					default:
						iRece_Result=Rece_Negative;
						break;
					}
				}
			}
			else
			{
				iRece_Result=Rece_Positive;
				vecReceTemp.push_back(binReceData);
			}
		}
		if(vecReceTemp.size())
		{
			iRece_Result=Rece_Positive;
		}
		else
		{
			vecReceTemp.push_back(vecReceFrm[iFrmNum-1]);
		}
		vecReceFrm.clear();
		for(i=0;i<vecReceTemp.size();i++)
		{
			vecReceFrm.push_back(vecReceTemp[i]);
		}
		break;
	default:
		break;
	}

	return iRece_Result;
}

short CCommLayer::GetEnterCmdID()
{
	CBinary binIndex=NULL;
	CBinaryGroup vecItemInfo;
	short iPos=0;

	binIndex.Add(bBrand_ID);
	binIndex+=binCommECU_ID;
	binIndex.Add(0x00);
	iPos=binIndex.GetSize()-1;

	while(1)
	{
		binIndex.SetAt(iPos,binIndex[iPos]+1);
		if(!CommonTools->ReadDBFile(vecItemInfo,"DIAMODE.DB",binIndex))
			break;
		if((vecItemInfo[1][0]==Comm_Status_NA)&&(vecItemInfo[2][0]==Comm_Status_General))
		{
			binEnterCmdID=vecItemInfo[0];
			return 1;
		}
	}

	return 0;
}

short CCommLayer::ChangeCommStatus(unsigned char bCurStatus,unsigned char bObjStatus)
{
	short iRet=0;

	if(bCurStatus==bObjStatus)
	{
		return Rece_Positive;
	}

	if((bCurStatus==Comm_Status_NA)&&(bObjStatus==Comm_Status_General)) // EnterSystem
	{
		iRet=SetupComm();
		if(iRet>0)
		{
			OpenIdle();
		}
	}
	else if((bCurStatus==Comm_Status_General)&&(bObjStatus==Comm_Status_NA)) // ExitSystem
	{
		iRet=SendExitSysCmd();
		CloseIdle();
	}

	if(iRet==Rece_Positive)
	{
		bCurStatus=bObjStatus;
	}

	return iRet;
}

short CCommLayer::OpenIdle()
{
	CBinary CBTemp;
	CSendFrame IdleCmd;
	short iIdleKeepTime=0;
	
	CBTemp=Link_Info[Pos_IdleCommand];
	if(!CommonTools->IsCBinaryAvailable(CBTemp))
		return 1;
	if(!MakeSendFrame(IdleCmd,CBTemp))
		return 1;
	CBTemp=Link_Info[Pos_IdleKeepTime];
	if(!CommonTools->IsCBinaryAvailable(CBTemp))
		return 1;
	iIdleKeepTime=CommonTools->StrBinToINT16(CBTemp);
	
	g_pCommEcu->KeepLink(iIdleKeepTime,&IdleCmd);
	
	return 1;
}

short CCommLayer::CloseIdle()
{
	g_pCommEcu->KeepLink();
	
	return 1;
}

short CCommLayer::InitHisData()
{
	vecHisReceData.clear();
	vecHisReceDataCmd.clear();
	return 1;
}

short CCommLayer::ReadAndUpdateHisData(CBinary binCmdID,CBinaryGroup &vecReceCmd)
{
	CMD_SESULT OneHisData;
	CBinary binIndex=NULL;
	short iRet=0;
	short i=0;
	short iNum=0;
	bool Flag_Find=false;

	iNum=vecHisReceData.size();
	for(i=0;i<iNum;i++)
	{
		OneHisData=vecHisReceData[i];
		binIndex=OneHisData.binCmdID;
		if(binIndex==binCmdID)
		{
			iRet=OneHisData.iSesult;
			vecReceCmd=OneHisData.vecReceCmd;
			Flag_Find=true;
			break;
		}
	}

	if(Flag_Find==false)
	{
		iRet=SendReceive(binCmdID,vecReceCmd);
		OneHisData.binCmdID=binCmdID;
		OneHisData.iSesult=iRet;
		OneHisData.vecReceCmd=vecReceCmd;
		vecHisReceData.push_back(OneHisData);
	}

	return iRet;
}

short CCommLayer::ReadAndUpdateHisDataCmd(CBinary binCmdInfo,CBinaryGroup &vecReceCmd)
{
	CMD_SESULT1 OneHisData;
	CBinary binIndex=NULL;
	short iRet=0;
	short i=0;
	short iNum=0;
	bool Flag_Find=false;
	
	iNum=vecHisReceDataCmd.size();
	for(i=0;i<iNum;i++)
	{
		OneHisData=vecHisReceDataCmd[i];
		binIndex=OneHisData.binCmdInfo;
		if(binIndex==binCmdInfo)
		{
			iRet=OneHisData.iSesult;
			vecReceCmd=OneHisData.vecReceCmd;
			Flag_Find=true;
			break;
		}
	}
	
	if(Flag_Find==false)
	{
		iRet=SendReceiveCmd(binCmdInfo,vecReceCmd);
		OneHisData.binCmdInfo=binCmdInfo;
		OneHisData.iSesult=iRet;
		OneHisData.vecReceCmd=vecReceCmd;
		vecHisReceDataCmd.push_back(OneHisData);
	}
	
	return iRet;
}

short CCommLayer::UpdateHisReceDataCmd(CBinary binCmdInfo,short iRet,CBinaryGroup vecReceCmd)
{
	CMD_SESULT1 OneHisData;

	OneHisData.binCmdInfo=binCmdInfo;
	OneHisData.iSesult=iRet;
	OneHisData.vecReceCmd=vecReceCmd;
	vecHisReceDataCmd.push_back(OneHisData);

	return 1;
}

short CCommLayer::OBDIICommSet_15765(CBinary &binECUIDs)
{
	CBinary binLinkID=NULL;
	short iRet=0;
	CBinary binCmd=NULL;
	CBinary binReceData=NULL;
	CSendFrame sendFrm;
	CReceiveFrame receFrm;
	short i=0,j=0;
	unsigned char bECUID=0x00;
	CBinary binBPS=NULL;

	g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x05",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);

	if(Demo_Flag==true)
	{
		bCurBUS_ID=P_KWP2000_ON_CAN;
		bCommBUS_ID=bCurBUS_ID;
		binECUIDs.Add(0xE8);
		binECUIDs.Add(0xE9);
		return 1;
	}

	binECUIDs=NULL;
	
	bCurBUS_ID=P_KWP2000_ON_CAN;
	bCommBUS_ID=bCurBUS_ID;
	binLinkID.Add(bBrand_ID);
	binLinkID+=binCommECU_ID;
	binLinkID.Add(bCommBUS_ID);
	binLinkID.Add(0x01);
	binLinkID.Add(0x00);

	if(!CommonTools->ReadDBFile(Link_Info,"LINKLAYER.DB",binLinkID))
	{
		return 0;
	}
	if(!InitLinkLayer())
	{
		return 0;
	}
// 	if(!InitLinkLayer()) //������������CANBUSЭ���л���CANBUS��ʱ���ظ����������Ժ�CANBUS����ſ��Է���ȥ
// 	{
// 		return 0;
// 	}

	binCmd=CBinary("\x08\x07\xDF\x02\x01\x00\xFF\xFF\xFF\xFF\xFF",11);
	sendFrm.SetDefaultReceiveFrameNumber(0xFF);
	sendFrm=binCmd;
	for(i=0;i<2;i++)
	{
		receFrm=g_pCommEcu->SendReceive(sendFrm);
		if(receFrm.size())
		{
			for(j=0;j<receFrm[0].size();j++)
			{
				binReceData=receFrm[0][j];
				if(binReceData.GetSize()>6)
				{
					if((binReceData[4]==0x41)&&(binReceData[5]==0x00))
					{
						bECUID=binReceData[2];
						if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
							binECUIDs.Add(bECUID);
					}
				}
			}
			break;
		}
	}
	iRet=binReceData.GetSize();
	if(iRet>0)
	{
		return iRet;
	}

	CommonTools->StringToCBinary("250000",binBPS);
	Link_Info[Pos_Baudrate]=binBPS;
	g_pCommEcu->SetBps(250000);
	for(i=0;i<2;i++)
	{
		receFrm=g_pCommEcu->SendReceive(sendFrm);
		if(receFrm.size())
		{
			for(j=0;j<receFrm[0].size();j++)
			{
				binReceData=receFrm[0][j];
				if(binReceData.GetSize()>6)
				{
					if((binReceData[4]==0x41)&&(binReceData[5]==0x00))
					{
						bECUID=binReceData[2];
						if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
							binECUIDs.Add(bECUID);
					}
				}
			}
			break;
		}
	}
	iRet=binReceData.GetSize();
	if(iRet>0)
	{
		return iRet;
	}
	
	bCurBUS_ID=P_ED_ON_CAN;
	bCommBUS_ID=bCurBUS_ID;
	binLinkID.SetAt(5,bCommBUS_ID);
	
	if(!CommonTools->ReadDBFile(Link_Info,"LINKLAYER.DB",binLinkID))
	{
		return 0;
	}
	if(!InitLinkLayer())
	{
		return 0;
	}
	sendFrm.Clear();
	binCmd=CBinary("\x88\x18\xDB\x33\xF1\x02\x01\x00\xFF\xFF\xFF\xFF\xFF",13);
	sendFrm.SetDefaultReceiveFrameNumber(0xFF);
	sendFrm=binCmd;
	for(i=0;i<2;i++)
	{
		receFrm=g_pCommEcu->SendReceive(sendFrm);
		if(receFrm.size())
		{
			for(j=0;j<receFrm[0].size();j++)
			{
				binReceData=receFrm[0][j];
				if(binReceData.GetSize()>8)
				{
					if((binReceData[6]==0x41)&&(binReceData[7]==0x00))
					{
						bECUID=binReceData[4];
						if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
							binECUIDs.Add(bECUID);
					}
				}
			}
			break;
		}
	}
	iRet=binReceData.GetSize();
	if(iRet>0)
	{
		return iRet;
	}
	CommonTools->StringToCBinary("250000",binBPS);
	Link_Info[Pos_Baudrate]=binBPS;
	g_pCommEcu->SetBps(250000);
	for(i=0;i<2;i++)
	{
		receFrm=g_pCommEcu->SendReceive(sendFrm);
		if(receFrm.size())
		{
			for(j=0;j<receFrm[0].size();j++)
			{
				binReceData=receFrm[0][j];
				if(binReceData.GetSize()>8)
				{
					if((binReceData[6]==0x41)&&(binReceData[7]==0x00))
					{
						bECUID=binReceData[4];
						if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
							binECUIDs.Add(bECUID);
					}
				}
			}
			break;
		}
	}
	iRet=binReceData.GetSize();

	return iRet;
}

#if 1
short CCommLayer::OBDIICommSet_KLine(CBinary &binECUIDs)
{
	CBinary binLinkID;
	short iRet=0;
	CBinary binCmd;
	CBinary binReceData;
	CSendFrame sendFrm;
	CReceiveFrame receFrm;
	short i=0,j=0;
	unsigned char bECUID=0x00;

	g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x04",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);

	
	bCurBUS_ID=P_ED_ISO_9141_K_LINE;
	bCommBUS_ID=bCurBUS_ID;
	binLinkID.Add(bBrand_ID);
	binLinkID+=binCommECU_ID;
	binLinkID.Add(bCommBUS_ID);
	binLinkID.Add(0x01);
	binLinkID.Add(0x00);

	if(!CommonTools->ReadDBFile(Link_Info,"LINKLAYER.DB",binLinkID))
	{
		return 0;
	}
	if(!InitLinkLayer())
	{
		return 0;
	}


	if(!binECUIDs.GetSize())
	{
		receFrm=g_pCommEcu->AddressCodeEnter(0x33);
		if(receFrm.size()>0)
		{
			binReceData=receFrm.GetFirstAnswerFrame();
			if(binReceData.GetSize()==2)
			{
				switch(binReceData[0]*256+binReceData[1])
				{
				case 0x0808:
				case 0x9494:
					bCurBUS_ID=P_ED_ISO_9141_K_LINE;
					binCmd=CBinary("\x68\x6A\xF1\x01\x00\xC4",6);
					sendFrm.SetDefaultReceiveFrameNumber(0xFF);
					sendFrm=binCmd;
					break;
				case 0xE98F:
				case 0x6B8F:
				case 0x6D8F:
				case 0xEF8F:
					g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x03",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);
					bCurBUS_ID=P_KWP2000_ON_K_LINE;
					binCmd=CBinary("\xC2\x33\xF1\x01\x00\xE7",6);
					sendFrm.SetDefaultReceiveFrameNumber(0xFF);
					sendFrm=binCmd;
					break;
				default:
					bCurBUS_ID=0x00;
					break;
				}
			}
			if(bCurBUS_ID>0x00)
			{
				for(i=0;i<2;i++)
				{
					receFrm=g_pCommEcu->SendReceive(sendFrm);
					if(receFrm.size())
					{
						for(j=0;j<receFrm[0].size();j++)
						{
							binReceData=receFrm[0][j];
							if(binReceData.GetSize()>6)
							{
								if((binReceData[3]==0x41)&&(binReceData[4]==0x00))
								{
									bECUID=binReceData[2];
									if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
										binECUIDs.Add(bECUID);
								}
							}
						}
						break;
					}
				}
			}
		}
	}

	if(!binECUIDs.GetSize())
	{
		g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x03",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);
		
		bCurBUS_ID=P_KWP2000_ON_K_LINE;
		binCmd=CBinary("\xC1\x33\xF1\x81\x66",5);
		sendFrm.SetDefaultReceiveFrameNumber(0xFF);
		sendFrm=binCmd;
		for(i=0;i<2;i++)
		{
			g_pCommEcu->Begin();
			g_pCommEcu->VoltageHighLowTime(3,300,25,25);	//����300ms,����25ms,������25ms
			g_pCommEcu->SendReceive(sendFrm);
			receFrm=g_pCommEcu->End();
			if(receFrm.size())
			{
				for(j=0;j<receFrm[0].size();j++)
				{
					binReceData=receFrm[0][j];
					if(binReceData.GetSize()>6)
					{
						if((binReceData[1]==0xF1)&&(binReceData[3]==0xC1)&&(binReceData[5]==0x8F))
						{
							if((binReceData[4]==0xE9)||(binReceData[4]==0x6B)||(binReceData[4]==0x6D)||(binReceData[4]==0xEF))
							{
								bECUID=binReceData[2];
								if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
									binECUIDs.Add(bECUID);
							}
						}
					}
				}
				break;
			}
		}
	}


	iRet=binECUIDs.GetSize();

	if(!iRet)
	{
		g_pCommEcu->KeepLink();
	}
	return iRet;
}
#else
short CCommLayer::OBDIICommSet_KLine(CBinary &binECUIDs)
{
	CBinary binLinkID;
	short iRet=0;
	CBinary binCmd;
	CBinary binReceData;
	CSendFrame sendFrm;
	CReceiveFrame receFrm;
	short i=0,j=0;
	unsigned char bECUID=0x00;

//	g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x04",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);
	
	binECUIDs=NULL;
	
	bCurBUS_ID=P_ED_ISO_9141_K_LINE;
	bCommBUS_ID=bCurBUS_ID;
	binLinkID.Add(bBrand_ID);
	binLinkID+=binCommECU_ID;
	binLinkID.Add(bCommBUS_ID);
	binLinkID.Add(0x01);
	binLinkID.Add(0x00);

	if(!CommonTools->ReadDBFile(Link_Info,"LINKLAYER.DB",binLinkID))
	{
		return 0;
	}
	if(!InitLinkLayer())
	{
		return 0;
	}

	if(!binECUIDs.GetSize())
	{
		g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x03",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);
		
		bCurBUS_ID=P_KWP2000_ON_K_LINE;
		binCmd=CBinary("\xC1\x33\xF1\x81\x66",5);
		sendFrm.SetDefaultReceiveFrameNumber(0xFF);
		sendFrm=binCmd;
		for(i=0;i<2;i++)
		{
			g_pCommEcu->Begin();
			g_pCommEcu->VoltageHighLowTime(3,300,25,25);	//����300ms,����25ms,������25ms
			g_pCommEcu->SendReceive(sendFrm);
			receFrm=g_pCommEcu->End();
			if(receFrm.size())
			{
				for(j=0;j<receFrm[0].size();j++)
				{
					binReceData=receFrm[0][j];
					if(binReceData.GetSize()>6)
					{
						if((binReceData[1]==0xF1)&&(binReceData[3]==0xC1)&&(binReceData[5]==0x8F))
						{
							if((binReceData[4]==0xE9)||(binReceData[4]==0x6B)||(binReceData[4]==0x6D)||(binReceData[4]==0xEF))
							{
								bECUID=binReceData[2];
								if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
									binECUIDs.Add(bECUID);
							}
						}
					}
				}
				break;
			}
		}
	}

	if(!binECUIDs.GetSize())
	{
		g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x04",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);
		receFrm=g_pCommEcu->AddressCodeEnter(0x33);
		if(receFrm.size()>0)
		{
			binReceData=receFrm.GetFirstAnswerFrame();
			if(binReceData.GetSize()==2)
			{
				switch(binReceData[0]*256+binReceData[1])
				{
				case 0x0808:
				case 0x9494:
					bCurBUS_ID=P_ED_ISO_9141_K_LINE;
					binCmd=CBinary("\x68\x6A\xF1\x01\x00\xC4",6);
					sendFrm.SetDefaultReceiveFrameNumber(0xFF);
					sendFrm=binCmd;
					break;
				case 0xE98F:
				case 0x6B8F:
				case 0x6D8F:
				case 0xEF8F:
					g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x03",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);
					bCurBUS_ID=P_KWP2000_ON_K_LINE;
					binCmd=CBinary("\xC2\x33\xF1\x01\x00\xE7",6);
					sendFrm.SetDefaultReceiveFrameNumber(0xFF);
					sendFrm=binCmd;
					break;
				default:
					bCurBUS_ID=0x00;
					break;
				}
			}
			if(bCurBUS_ID>0x00)
			{
				for(i=0;i<2;i++)
				{
					receFrm=g_pCommEcu->SendReceive(sendFrm);
					if(receFrm.size())
					{
						for(j=0;j<receFrm[0].size();j++)
						{
							binReceData=receFrm[0][j];
							if(binReceData.GetSize()>6)
							{
								if((binReceData[3]==0x41)&&(binReceData[4]==0x00))
								{
									bECUID=binReceData[2];
									if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
										binECUIDs.Add(bECUID);
								}
							}
						}
						break;
					}
				}
			}
		}
	}

	iRet=binECUIDs.GetSize();

	if(!iRet)
	{
		g_pCommEcu->KeepLink();
	}

	return iRet;
}
#endif

short CCommLayer::OBDIICommSet_J1850_VPW(CBinary &binECUIDs)
{
	CBinary binLinkID;
	short iRet=0;
	CBinary binCmd;
	CBinary binReceData;
	CSendFrame sendFrm;
	CReceiveFrame receFrm;
	short i=0,j=0;
	unsigned char bECUID=0x00;

	g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x02",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);
	
	//binECUIDs;
	
	bCurBUS_ID=P_ED_VPW;
	bCommBUS_ID=bCurBUS_ID;
	binLinkID.Add(bBrand_ID);
	binLinkID+=binCommECU_ID;
	binLinkID.Add(bCommBUS_ID);
	binLinkID.Add(0x01);
	binLinkID.Add(0x00);

	LOG_I(TAG, "%s[line %d] ReadDBFile [LINKLAYER.DB]", __FUNCTION__, __LINE__);
	if(!CommonTools->ReadDBFile(Link_Info,"LINKLAYER.DB",binLinkID))
	{
		return 0;
	}

	LOG_I(TAG, "%s[line %d] InitLinkLayer", __FUNCTION__, __LINE__);
	if(!InitLinkLayer())
	{
		return 0;
	}

	adsSleep(500);

	binCmd=CBinary("\x68\x6A\xF1\x01\x00\xE7",6);
	VerifyCMD_CRC(binCmd);
	sendFrm.SetDefaultReceiveFrameNumber(0xFF);
	sendFrm=binCmd;
	for(i=0;i<2;i++)
	{
		receFrm=g_pCommEcu->SendReceive(sendFrm);
		LOG_I(TAG, "%s[line %d] receFrm.size() = %d", __FUNCTION__, __LINE__, receFrm.size());
		if(receFrm.size())
		{
			for(j=0;j<receFrm[0].size();j++)
			{
				LOG_I(TAG, "%s[%d] receFrm[0].size() = %d", __FUNCTION__, __LINE__, receFrm[0].size());
				binReceData=receFrm[0][j];
				if(binReceData.GetSize()>6)
				{
					if((binReceData[3]==0x41)&&(binReceData[4]==0x00))
					{
						bECUID=binReceData[2];
						if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
							binECUIDs.Add(bECUID);
					}
				}
			}
			break;
		}
	}

	iRet=binReceData.GetSize();
	LOG_I(TAG, "%s[%d] return code %d", __FUNCTION__, __LINE__, iRet);
	return iRet;
}

short CCommLayer::OBDIICommSet_J1850_PWM(CBinary &binECUIDs)
{
	CBinary binLinkID;
	short iRet=0;
	CBinary binCmd;
	CBinary binReceData;
	CSendFrame sendFrm;
	CReceiveFrame receFrm;
	short i=0,j=0;
	unsigned char bECUID=0x00;
	
	g_pDisplay->MessageBox(CBinary("\x00\xFF\x05\x00\x00\x01",6),CBinary("\x00\xFF\x00\x00\x00\x09",6),adsMB_NoButton);
	
	bCurBUS_ID=P_ED_PWM;
	bCommBUS_ID=bCurBUS_ID;
	binLinkID.Add(bBrand_ID);
	binLinkID+=binCommECU_ID;
	binLinkID.Add(bCommBUS_ID);
	binLinkID.Add(0x01);
	binLinkID.Add(0x00);
	
	if(!CommonTools->ReadDBFile(Link_Info,"LINKLAYER.DB",binLinkID))
	{
		return 0;
	}
	if(!InitLinkLayer())
	{
		return 0;
	}
	
	binCmd=CBinary("\x61\x6A\xF1\x01\x00\xE7",6);
	VerifyCMD_CRC(binCmd);
	sendFrm.SetDefaultReceiveFrameNumber(0xFF);
	sendFrm=binCmd;
	for(i=0;i<2;i++)
	{
		receFrm=g_pCommEcu->SendReceive(sendFrm);
		if(receFrm.size())
		{
			for(j=0;j<receFrm[0].size();j++)
			{
				binReceData=receFrm[0][j];
				if(binReceData.GetSize()>6)
				{
					if((binReceData[3]==0x41)&&(binReceData[4]==0x00))
					{
						bECUID=binReceData[2];
						if(CommonTools->GetBytePosInCBinary(binECUIDs,bECUID)<0)
							binECUIDs.Add(bECUID);
					}
				}
			}
			break;
		}
	}
	
	iRet=binReceData.GetSize();
	
	return iRet;
}

short CCommLayer::OBDIISetIdle()
{
	CSendFrame sfTxf;
	CBinary binLinkCmd;
	LOG_I(TAG, "OBDIISetIdle[line %d] busID--->%d", __LINE__,bCommBUS_ID);
	switch(bCommBUS_ID)
	{
	case P_KWP2000_ON_K_LINE:
		binLinkCmd=CBinary("\xc2\x33\xf1\x3e\x01\x25",6);
		sfTxf.SetDefaultReceiveFrameNumber(1);
		sfTxf=binLinkCmd;
		g_pCommEcu->KeepLink(2000,&sfTxf);
		break;
	case P_ED_ISO_9141_K_LINE:
		binLinkCmd=CBinary("\x68\x6A\xf1\x01\x00\xc4",6);
		sfTxf.SetDefaultReceiveFrameNumber(1);
		sfTxf=binLinkCmd;
		g_pCommEcu->KeepLink(2000,&sfTxf);
		break;
	default:
		break;
	}

	return 1;
}

short CCommLayer::OBDIIResetFilter(unsigned char bECUID)
{
	char strFilter[3];
	CCommWithEcu::CFilterOrLengthRecognise FilterCond;
	CBinary binCANFilterID;
	CBinary binFlowCtrlCmd;
	CSendFrame sf;

	FilterCond.Empty();
	switch(bCommBUS_ID)
	{
	case P_KWP2000_ON_K_LINE:
		sprintf(strFilter,"%c%c",0xF1,bECUID);
		FilterCond.SetNormalFilterMode(1,2,strFilter);
		g_pCommEcu->ProtocolSet (PROTOCOL_KWP,&FilterCond);
		break;
	case P_ED_ISO_9141_K_LINE:
		g_pCommEcu->TimeIntervalOver(50,60,3); // ���°����ĵ���������һ��ʱ�䣬��ΪISO�Ǹ���ʱ�������յ�
		sprintf(strFilter,"%c%c",0x6B,bECUID);
		FilterCond.SetNormalFilterMode(1,2,strFilter);
		g_pCommEcu->ProtocolSet (PROTOCOL_ISO,&FilterCond);
		break;
	case P_ED_VPW:
		sprintf(strFilter,"%c%c",0x6B,bECUID);
		FilterCond.SetNormalFilterMode(1,2,strFilter);
		g_pCommEcu->ProtocolSet (PROTOCOL_VPW,&FilterCond);
		break;
	case P_ED_PWM:
		sprintf(strFilter,"%c%c",0x6B,bECUID);
		FilterCond.SetNormalFilterMode(1,2,strFilter);
		g_pCommEcu->ProtocolSet (PROTOCOL_PWM,&FilterCond);
		break;
	case P_KWP2000_ON_CAN:
		binCANFilterID=CBinary("\x07\xE8",2);
		binCANFilterID.SetAt(1,bECUID);
		FilterCond.AddCanFilterId(binCANFilterID);
		g_pCommEcu->ProtocolSet(PROTOCOL_CANBUS,&FilterCond);
		FilterCond.Empty();
		FilterCond.SetCanFilterMode(3,0x10,4);
		binFlowCtrlCmd=CBinary("\x08\x07\xdf\x30\x00\x04\x00\x00\x00\x00\x00",11);
		binFlowCtrlCmd.SetAt(2,bECUID-0x08);
		sf = binFlowCtrlCmd;
		g_pCommEcu->SetAutoSendContinuedFrame(FilterCond,sf);
		g_pCommEcu->SetNegativeHandleMethod(4,0x7f,6,0x78,1);
		break;
	case P_ED_ON_CAN:
		binCANFilterID=CBinary("\x18\xDA\x11\xF1",4);
		binCANFilterID.SetAt(2,bECUID);
		Link_Info[Pos_ToolCANID]=binCANFilterID;
		binCANFilterID=CBinary("\x18\xDA\xF1\x11",4);
		binCANFilterID.SetAt(3,bECUID);
		FilterCond.AddCanFilterId(binCANFilterID);
		g_pCommEcu->ProtocolSet(PROTOCOL_CANBUS,&FilterCond);
		FilterCond.Empty();
		FilterCond.SetCanFilterMode(5,0x10,6);
		binFlowCtrlCmd=CBinary("\x88\x18\xDA\x11\xf1\x30\x00\x04\x00\x00\x00\x00\x00",13);
		binFlowCtrlCmd.SetAt(3,bECUID);
		sf = binFlowCtrlCmd;
		g_pCommEcu->SetAutoSendContinuedFrame(FilterCond,sf);
		g_pCommEcu->SetNegativeHandleMethod(6,0x7f,8,0x78,1);
		break;
	default:
		break;
	}

	return 1;
}
