#ifndef __DRIVEN_STD__
#define  __DRIVEN_STD__

#include "CommLayer.h"
#include "DataListFunc.h"
#include "DTCFunc.h"

#define Pos_MaxCommTime                0
#define Pos_ReceTimeOut                1
#define Pos_SendWaitMinTime            2
#define Pos_SendIntervalTime           3
#define Pos_Baudrate                   4
#define Pos_ParityBit                  5
#define Pos_DataBit                    6
#define Pos_Init_Mode                  7
#define Pos_Parameter                  8
#define Pos_AddressType                9
#define Pos_Send_Pin                   10
#define Pos_Rece_Pin                   11
#define Pos_Other_Pin                  12
#define Pos_Control_Pin                13
#define Pos_VoltageValue               14
#define Pos_IdleCommand                15
#define Pos_IdleKeepTime               16
#define Pos_PackageHeader              17
#define Pos_VerifyMethod               18
#define Pos_ToolAddress                19
#define Pos_ECUAddress                 20
#define Pos_ToolCANID                  21
#define Pos_ECUCANID                   22
#define Pos_FlowControlPosition        23
#define Pos_FlowControlMask            24
#define Pos_FlowControlFrame           25
#define Pos_FrameLengthPosition        26
#define Pos_Mark                       27
#define Pos_Offset                     28
#define Pos_FilterPositions            29
#define Pos_FilterContain              30
#define Pos_CAN_Defualt_BYTE           31

#define P_KWP2000_ON_K_LINE 1
#define P_KWP2000_ON_CAN 2
#define P_UDS_ON_CAN 3
#define P_ED_ISO_14230_K_LINE 4
#define P_ED_ISO_9141_K_LINE 5
#define P_ED_ON_CAN 6
#define P_ED_VPW 7
#define P_ED_PWM 8
#define P_UDS_ON_K_LINE 13

#define Comm_Status_NA 1
#define Comm_Status_General 2
#define Comm_Status_Advance 3

#define PackageHeader_KWP2000_0X  1
#define PackageHeader_KWP2000_00  2
#define PackageHeader_KWP2000_8X  3
#define PackageHeader_KWP2000_80  4
#define PackageHeader_KWP2000_CX  5

#define Verify_CheckSum  1
#define Verify_CheckSum_Reverse  2
#define Verify_CheckSum_Reverse_P1  3
#define Verify_Sum_TowBytes_Low  4
#define Verify_Sum_TowBytes_High  5
#define Verify_XOR  6
#define Verify_XOR_9Bits  7
#define Verify_CRC  8
#define Verify_CheckSum_Reverse_M1  9

#define ReadCode_001 1

#define Data_Type_ECUInfo 1
#define Data_Type_DataList 2
#define Data_Type_Other 3

#define CONV_LINEARITY 1
#define CONV_STATUS 2
#define CONV_STATUS_BIT_1 3
#define CONV_STATUS_BIT_N 4
#define CONV_DISPLAY 5
#define CONV_SPE1 6
#define CONV_SPE2 7
#define CONV_SPE3 8
#define CONV_SPE4 9
#define CONV_SPE5 10

#define Return_Comm_Fail -1
#define Return_Fail 0
#define Return_OK 1
#define Return_Break 2
#define Return_End 3

#define Spec_Step_Break -1
#define Spec_Step_End -2

#define Rece_Positive 1
#define Rece_Error 0
#define Rece_Negative -1
#define Rece_Negative_KeepRece -2
#define Rece_Negative_KeepSend -3
#define Rece_Negative_ReEnter -4

#define INFORMATION					CBinary("\x00\xFF\x00\x00\x00\x09", 6)
#define CANNOT_OPEN_FILE			CBinary("\x00\xFF\x00\x00\x00\x10", 6)

extern bool Demo_Flag;
extern unsigned char bBrand_ID;

extern CBinary binVehicleID;
extern CBinary binECUID;
extern CBinary binCommECU_ID;
extern unsigned char bCommBUS_ID;
extern CBinary binCurECU_ID;
extern unsigned char bCurBUS_ID;
extern CCommLayer *CommLayer;
extern CDataListFunc *DataListFunc;
extern CDTCFunc *DTCFunc;
extern CCommonTools *CommonTools;
extern vector<CBinary> vecReceData;

#endif


