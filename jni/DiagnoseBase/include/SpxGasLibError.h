/***************************************************************************//**
\file		SpxGasLibError.h
\brief
	defines the error code returned by SglGetLastError(). If it is not defined
	here, please check the WinError.h for system definitions.
	Another, these error codes are "reused" to be user-defined Windows messages 
	which will be sent in non-blocked working style. For more info, please refer 
	to SpxGasLib.h.
\warning
	to avoid conflicting with other user defined error no. or private Windows
	messages' no., please try modifying tag SGL_SPC or SGL_OFS.
\author		james.zhang@spx.com
*******************************************************************************/
#ifndef SPXGASLIBERROR_H
#define SPXGASLIBERROR_H

/*
 *	Windows messages number info. grabbed from MSDN
 */
// #define WM_USER       0x0400
// 
// Remarks
// There are five ranges of message numbers: 
// 
// Range Meaning 
// 0 through WM_USER ¨C 1 Messages reserved for use by the system. 
// WM_USER through 0x7FFF Integer messages for use by private window classes. 
// WM_APP through 0xBFFF Messages available for use by applications. 
// 0xC000 through 0xFFFF String messages for use by applications. 
// Greater than 0xFFFF Reserved by the system for future use. 


/*
 *	Windows error number info. copied from WinError.h
 */
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//

#define SGL_SEV		0xC0000000L		// severity code: 11 - Error, not used yet
#define SGL_C		0x20000000L		// enable Customer code flag for SpxGasLib
#define SGL_FCL		(FACILITY_WINDOWS_CE<<16)		// not used yet

// SpxGasLib error number's specific offset, designed to use not more than 1024
// error number ( currently [F000,F3FF], should be enough ), this tag can be
// modified to avoid conflicting with existed user defined error no.
#define SGL_SPC		0x00007000L		

// final offset for SpxGasLib error number
/*
 *	to follow Windows definition: "WM_USER through 0x7FFF" "Integer messages for
 *	use by private window classes"
 */
#define SGL_OFS		(0x0L/*|SGL_SEV*//*|SGL_C*//*|SGL_FCL*/|SGL_SPC) 

#define SGL_HEAD							(0x0L|SGL_OFS)		// head errno
#define SGL_TAIL							(SGL_HEAD+0x3FFL)	// tail errno

#define SGL_SUCCESS							ERROR_SUCCESS		// 0

#define SGL_ERRNO_SEGMENT_COUNT				0x100


////////////////////////////////////
//                                //
// SEGMENT -- Sgl FATAL           //
// [0x000, 0x0FF]                 //
////////////////////////////////////
#define SGL_FTL_OFS							SGL_HEAD
#define SGL_FTL_HEAD						SGL_FTL_OFS
#define SGL_FTL_TAIL						(SGL_FTL_HEAD+0x0FFL)

// Sgl undefined fatal error.
#define SGL_FTL_UNDEFINED					(0x0L|SGL_FTL_OFS)

// Sgl gas module fatal error:		hardware fault
#define SGL_FTL_HW_FAULT					(0x1L|SGL_FTL_OFS)

// Sgl gas module fatal error:		EEPROM out of its range
#define SGL_FTL_EEPROM_OUT_RANGE			(0x2L|SGL_FTL_OFS)

// Sgl gas module fatal error:		temperature out of its range
#define SGL_FTL_TEMP_OUT_SPEC				(0x3L|SGL_FTL_OFS)

// Sgl gas module fatal error:		pressure out of its range
#define SGL_FTL_PRES_OUT_SPEC				(0x4L|SGL_FTL_OFS)

// Sgl gas module fatal error:		tile orientation out of its range
#define SGL_FTL_TILT_OUT_SPEC				(0x5L|SGL_FTL_OFS)

// Sgl gas module fatal error:		filter fatal error
#define SGL_FTL_FILTER						(0x6L|SGL_FTL_OFS)

// Sgl zero request status on after doing zero bench
#define SGL_FTL_ZEROREQ_IN_ZEROBENCH		(0x7L|SGL_FTL_OFS)

// Sgl gas module fatal error:		hardware revision not match, need update
#define SGL_FTL_HW_REV						(0x8L|SGL_FTL_OFS)

// Sgl gas module fatal error:		tile self test failed
#define SGL_FTL_TILT_SELFTEST				(0x9L|SGL_FTL_OFS)

// Sgl gas module fatal error:		tilt record sign error
#define SGL_FTL_TILT_RECORD_SIGN			(0xAL|SGL_FTL_OFS)

// Sgl gas module fatal error:		read ROM out of range
#define SGL_FTL_ROM_OUT_RANGE				(0xBL|SGL_FTL_OFS)


////////////////////////////////////
//                                //
// SEGMENT -- Sgl Error           //
// [0x100, 0x1FF]                 //
////////////////////////////////////
#define SGL_ERR_OFS							(SGL_FTL_TAIL+0x1L)
#define SGL_ERR_HEAD						SGL_ERR_OFS
#define SGL_ERR_TAIL						(SGL_ERR_HEAD+0x0FFL)

// Sgl undefined error.
#define SGL_ERR_UNDEFINED					(0x0L|SGL_ERR_OFS)

// Sgl Not enough storage is available to process this command.
#define SGL_ERR_NOT_ENOUGH_MEMORY			(0x1L|SGL_ERR_OFS)

// Sgl device communication time out
#define SGL_ERR_COMM_TIMEOUT				(0x2L|SGL_ERR_OFS)

// Sgl serial communication sent length less than that desired to send
#define SGL_ERR_SERIAL_SEND_LACK			(0x3L|SGL_ERR_OFS)

// Sgl serial communication received length less than that desired to receive
#define SGL_ERR_SERIAL_RECV_LACK			(0x4L|SGL_ERR_OFS)

// Sgl last work hasn't done as last work was performed in non-blocked style
#define SGL_ERR_LAST_WORK_UNDONE			(0x5L|SGL_ERR_OFS)

// Sgl time out when waiting for work done
#define SGL_ERR_WAIT_WORK_DONE_TIMEOUT		(0x6L|SGL_ERR_OFS)

// Sgl tilt calibration error, tilt check will be disabled
#define SGL_ERR_TILT_CHECK					(0x7L|SGL_ERR_OFS)

// Sgl failed to read specified ini file
#define SGL_ERR_READ_INI					(0x8L|SGL_ERR_OFS)

// Sgl this is not the task that you just performed 
#define SGL_ERR_NO_SUCH_TASK_TO_RESUME		(0x9L|SGL_ERR_OFS)

// Sgl no task to resume, or task already resumed
#define SGL_ERR_NO_TASK_TO_RESUME			(0xAL|SGL_ERR_OFS)

// Sgl invalid input parameter
#define SGL_ERR_INVALID_INPUT_PRM			(0xBL|SGL_ERR_OFS)

// Sgl need perform Zero with AutoMode
#define SGL_ERR_NEED_ZERO_AUTOMODE			(0xCL|SGL_ERR_OFS)

// Sgl need perform Zero with FilterMode
#define SGL_ERR_NEED_ZERO_FILTERMODE		(0xDL|SGL_ERR_OFS)

// Sgl need perform Zero with UserMode
#define SGL_ERR_NEED_ZERO_USERMODE			(0xEL|SGL_ERR_OFS)

// Sgl time out when waiting for work done
#define SGL_ERR_TEST_CAL_GAS_TIMEOUT		(0xFL|SGL_ERR_OFS)

// Sgl failed to write specified ini file
#define SGL_ERR_WRITE_INI					(0x10L|SGL_ERR_OFS)

// Sgl device communication no such command while packing request
#define SGL_ERR_COMM_BAD_CMD				(0x11L|SGL_ERR_OFS)

// Sgl device communication send request error 
#define SGL_ERR_COMM_TX_REQ					(0x12L|SGL_ERR_OFS)

// Sgl device communication checksum error
#define SGL_ERR_COMM_CHKSUM					(0x13L|SGL_ERR_OFS)

// Sgl device communication transmit error
#define SGL_ERR_COMM_TRANSMIT				(0x14L|SGL_ERR_OFS)

// Sgl device communication receive non-acknowledgment
#define SGL_ERR_COMM_RX_NACK				(0x15L|SGL_ERR_OFS)

// Sgl device communication is in progress
#define SGL_ERR_COMM_IN_PROGRESS			(0x16L|SGL_ERR_OFS)

// Sgl device communication unknown error
#define SGL_ERR_COMM_UNKNOWN				(0x17L|SGL_ERR_OFS)

// Sgl time out when suspending work function
#define SGL_ERR_SUSPEND_WRK_FUNC_TIMEOUT	(0x18L|SGL_ERR_OFS)


////////////////////////////////////
//                                //
// SEGMENT -- Sgl Information     //
// [0x200, 0x2FF]                 //
////////////////////////////////////
#define SGL_INF_OFS							(SGL_ERR_TAIL+0x1L)
#define SGL_INF_HEAD						SGL_INF_OFS
#define SGL_INF_TAIL						(SGL_INF_HEAD+0x0FFL)

// Sgl undefined information
#define SGL_INF_UNDEFINED					(0x0L|SGL_INF_OFS)

// Sgl gas module needs 10s to be ready accept next command after reset
#define SGL_INF_AFTER_RESET_WAIT			(0x1L|SGL_INF_OFS)

// Sgl every task should be normally (or "naturally") ended with this info.
#define SGL_INF_TASK_SUCCEEDED				(0x2L|SGL_INF_OFS)

// Sgl after pump solenoid on, wait water purge
#define SGL_INF_AFTER_WATER_PURGE_WAIT		(0x3L|SGL_INF_OFS)

// Sgl after purge solenoid off, wait zero gas purge
#define SGL_INF_AFTER_ZERO_PURGE_WAIT		(0x4L|SGL_INF_OFS)

// Sgl after zero solenoid off, delay some time
#define SGL_INF_AFTER_ZERO_SOLENOID_WAIT	(0x5L|SGL_INF_OFS)

// Sgl in non-blocked style, we got chance to modify calibration gas values
#define SGL_INF_CAL_DATA_GOTTEN				(0x6L|SGL_INF_OFS)

// Sgl unused yet
#define SGL_INF_BEFORE_CALIBRATION_WAIT		(0x7L|SGL_INF_OFS)

// Sgl data of calibration gas (under testing) has been gotten
#define SGL_INF_TST_CAL_DATA_GOTTEN			(0x8L|SGL_INF_OFS)

// Sgl in non-blocked style, we got chance to modify vehicle information
#define SGL_INF_VEH_INF_GOTTEN				(0x9L|SGL_INF_OFS)

// Sgl Iridium won't accept any command in 10s immediately after power on
#define SGL_INF_AFTER_POWER_WAIT			(0xAL|SGL_INF_OFS)


////////////////////////////////////
//                                //
// SEGMENT -- Sgl Internal Error  //
// [0x300, 0x3FF]                 //
////////////////////////////////////
// Sgl library internal errors, please kindly report to author(s) if the 
// following one(s) appear(s)
#define SGL_ITN_OFS							(SGL_INF_TAIL+0x1L)
#define SGL_ITN_HEAD						SGL_ITN_OFS
#define SGL_ITN_TAIL						(SGL_ITN_HEAD+0x0FFL)

#define SGL_ITN_UNDEFINED					(0x0L|SGL_ITN_OFS)
	
#define SGL_ITN_UNKNOWN_GAS_DRV				(0x1L|SGL_ITN_OFS)

#define SGL_ITN_COMM_LOGIC					(0x2L|SGL_ITN_OFS)

#define SGL_ITN_NULL_DSP					(0x3L|SGL_ITN_OFS)

#define SGL_ITN_WT_PW						(0x4L|SGL_ITN_OFS)

#define SGL_ITN_NULL_PRM					(0x5L|SGL_ITN_OFS)

#define SGL_ITN_NOT_INITED					(0x6L|SGL_ITN_OFS)

#define SGL_ITN_WT_RWF						(0x7L|SGL_ITN_OFS)

#define SGL_ITN_UNKNOWN_MODULE_ID			(0x8L|SGL_ITN_OFS)

#define SGL_ITN_DBG_SUSPEND_RESUME			(0x9L|SGL_ITN_OFS)

#define SGL_ITN_DBG_POST_MSG				(0xAL|SGL_ITN_OFS)


////////////////////////////////////
//                                //
// Repeat Macro                   //
//                                //
////////////////////////////////////
# define SGL_RPT0(m, b, f)
# define SGL_RPT1(m, b, f) m(b, 0, f)
# define SGL_RPT2(m, b, f) SGL_RPT1(m, b, f) m(b, 1, f)
# define SGL_RPT3(m, b, f) SGL_RPT2(m, b, f) m(b, 2, f)
# define SGL_RPT4(m, b, f) SGL_RPT3(m, b, f) m(b, 3, f)
# define SGL_RPT5(m, b, f) SGL_RPT4(m, b, f) m(b, 4, f)
# define SGL_RPT6(m, b, f) SGL_RPT5(m, b, f) m(b, 5, f)
# define SGL_RPT7(m, b, f) SGL_RPT6(m, b, f) m(b, 6, f)
# define SGL_RPT8(m, b, f) SGL_RPT7(m, b, f) m(b, 7, f)
# define SGL_RPT9(m, b, f) SGL_RPT8(m, b, f) m(b, 8, f)
# define SGL_RPT10(m, b, f) SGL_RPT9(m, b, f) m(b, 9, f)
# define SGL_RPT11(m, b, f) SGL_RPT10(m, b, f) m(b, 10, f)
# define SGL_RPT12(m, b, f) SGL_RPT11(m, b, f) m(b, 11, f)
# define SGL_RPT13(m, b, f) SGL_RPT12(m, b, f) m(b, 12, f)
# define SGL_RPT14(m, b, f) SGL_RPT13(m, b, f) m(b, 13, f)
# define SGL_RPT15(m, b, f) SGL_RPT14(m, b, f) m(b, 14, f)
# define SGL_RPT16(m, b, f) SGL_RPT15(m, b, f) m(b, 15, f)
# define SGL_RPT17(m, b, f) SGL_RPT16(m, b, f) m(b, 16, f)
# define SGL_RPT18(m, b, f) SGL_RPT17(m, b, f) m(b, 17, f)
# define SGL_RPT19(m, b, f) SGL_RPT18(m, b, f) m(b, 18, f)
# define SGL_RPT20(m, b, f) SGL_RPT19(m, b, f) m(b, 19, f)
# define SGL_RPT21(m, b, f) SGL_RPT20(m, b, f) m(b, 20, f)
# define SGL_RPT22(m, b, f) SGL_RPT21(m, b, f) m(b, 21, f)
# define SGL_RPT23(m, b, f) SGL_RPT22(m, b, f) m(b, 22, f)
# define SGL_RPT24(m, b, f) SGL_RPT23(m, b, f) m(b, 23, f)
# define SGL_RPT25(m, b, f) SGL_RPT24(m, b, f) m(b, 24, f)
# define SGL_RPT26(m, b, f) SGL_RPT25(m, b, f) m(b, 25, f)
# define SGL_RPT27(m, b, f) SGL_RPT26(m, b, f) m(b, 26, f)
# define SGL_RPT28(m, b, f) SGL_RPT27(m, b, f) m(b, 27, f)
# define SGL_RPT29(m, b, f) SGL_RPT28(m, b, f) m(b, 28, f)
# define SGL_RPT30(m, b, f) SGL_RPT29(m, b, f) m(b, 29, f)
# define SGL_RPT31(m, b, f) SGL_RPT30(m, b, f) m(b, 30, f)
# define SGL_RPT32(m, b, f) SGL_RPT31(m, b, f) m(b, 31, f)
# define SGL_RPT33(m, b, f) SGL_RPT32(m, b, f) m(b, 32, f)
# define SGL_RPT34(m, b, f) SGL_RPT33(m, b, f) m(b, 33, f)
# define SGL_RPT35(m, b, f) SGL_RPT34(m, b, f) m(b, 34, f)
# define SGL_RPT36(m, b, f) SGL_RPT35(m, b, f) m(b, 35, f)
# define SGL_RPT37(m, b, f) SGL_RPT36(m, b, f) m(b, 36, f)
# define SGL_RPT38(m, b, f) SGL_RPT37(m, b, f) m(b, 37, f)
# define SGL_RPT39(m, b, f) SGL_RPT38(m, b, f) m(b, 38, f)
# define SGL_RPT40(m, b, f) SGL_RPT39(m, b, f) m(b, 39, f)
# define SGL_RPT41(m, b, f) SGL_RPT40(m, b, f) m(b, 40, f)
# define SGL_RPT42(m, b, f) SGL_RPT41(m, b, f) m(b, 41, f)
# define SGL_RPT43(m, b, f) SGL_RPT42(m, b, f) m(b, 42, f)
# define SGL_RPT44(m, b, f) SGL_RPT43(m, b, f) m(b, 43, f)
# define SGL_RPT45(m, b, f) SGL_RPT44(m, b, f) m(b, 44, f)
# define SGL_RPT46(m, b, f) SGL_RPT45(m, b, f) m(b, 45, f)
# define SGL_RPT47(m, b, f) SGL_RPT46(m, b, f) m(b, 46, f)
# define SGL_RPT48(m, b, f) SGL_RPT47(m, b, f) m(b, 47, f)
# define SGL_RPT49(m, b, f) SGL_RPT48(m, b, f) m(b, 48, f)
# define SGL_RPT50(m, b, f) SGL_RPT49(m, b, f) m(b, 49, f)
# define SGL_RPT51(m, b, f) SGL_RPT50(m, b, f) m(b, 50, f)
# define SGL_RPT52(m, b, f) SGL_RPT51(m, b, f) m(b, 51, f)
# define SGL_RPT53(m, b, f) SGL_RPT52(m, b, f) m(b, 52, f)
# define SGL_RPT54(m, b, f) SGL_RPT53(m, b, f) m(b, 53, f)
# define SGL_RPT55(m, b, f) SGL_RPT54(m, b, f) m(b, 54, f)
# define SGL_RPT56(m, b, f) SGL_RPT55(m, b, f) m(b, 55, f)
# define SGL_RPT57(m, b, f) SGL_RPT56(m, b, f) m(b, 56, f)
# define SGL_RPT58(m, b, f) SGL_RPT57(m, b, f) m(b, 57, f)
# define SGL_RPT59(m, b, f) SGL_RPT58(m, b, f) m(b, 58, f)
# define SGL_RPT60(m, b, f) SGL_RPT59(m, b, f) m(b, 59, f)
# define SGL_RPT61(m, b, f) SGL_RPT60(m, b, f) m(b, 60, f)
# define SGL_RPT62(m, b, f) SGL_RPT61(m, b, f) m(b, 61, f)
# define SGL_RPT63(m, b, f) SGL_RPT62(m, b, f) m(b, 62, f)
# define SGL_RPT64(m, b, f) SGL_RPT63(m, b, f) m(b, 63, f)
# define SGL_RPT65(m, b, f) SGL_RPT64(m, b, f) m(b, 64, f)
# define SGL_RPT66(m, b, f) SGL_RPT65(m, b, f) m(b, 65, f)
# define SGL_RPT67(m, b, f) SGL_RPT66(m, b, f) m(b, 66, f)
# define SGL_RPT68(m, b, f) SGL_RPT67(m, b, f) m(b, 67, f)
# define SGL_RPT69(m, b, f) SGL_RPT68(m, b, f) m(b, 68, f)
# define SGL_RPT70(m, b, f) SGL_RPT69(m, b, f) m(b, 69, f)
# define SGL_RPT71(m, b, f) SGL_RPT70(m, b, f) m(b, 70, f)
# define SGL_RPT72(m, b, f) SGL_RPT71(m, b, f) m(b, 71, f)
# define SGL_RPT73(m, b, f) SGL_RPT72(m, b, f) m(b, 72, f)
# define SGL_RPT74(m, b, f) SGL_RPT73(m, b, f) m(b, 73, f)
# define SGL_RPT75(m, b, f) SGL_RPT74(m, b, f) m(b, 74, f)
# define SGL_RPT76(m, b, f) SGL_RPT75(m, b, f) m(b, 75, f)
# define SGL_RPT77(m, b, f) SGL_RPT76(m, b, f) m(b, 76, f)
# define SGL_RPT78(m, b, f) SGL_RPT77(m, b, f) m(b, 77, f)
# define SGL_RPT79(m, b, f) SGL_RPT78(m, b, f) m(b, 78, f)
# define SGL_RPT80(m, b, f) SGL_RPT79(m, b, f) m(b, 79, f)
# define SGL_RPT81(m, b, f) SGL_RPT80(m, b, f) m(b, 80, f)
# define SGL_RPT82(m, b, f) SGL_RPT81(m, b, f) m(b, 81, f)
# define SGL_RPT83(m, b, f) SGL_RPT82(m, b, f) m(b, 82, f)
# define SGL_RPT84(m, b, f) SGL_RPT83(m, b, f) m(b, 83, f)
# define SGL_RPT85(m, b, f) SGL_RPT84(m, b, f) m(b, 84, f)
# define SGL_RPT86(m, b, f) SGL_RPT85(m, b, f) m(b, 85, f)
# define SGL_RPT87(m, b, f) SGL_RPT86(m, b, f) m(b, 86, f)
# define SGL_RPT88(m, b, f) SGL_RPT87(m, b, f) m(b, 87, f)
# define SGL_RPT89(m, b, f) SGL_RPT88(m, b, f) m(b, 88, f)
# define SGL_RPT90(m, b, f) SGL_RPT89(m, b, f) m(b, 89, f)
# define SGL_RPT91(m, b, f) SGL_RPT90(m, b, f) m(b, 90, f)
# define SGL_RPT92(m, b, f) SGL_RPT91(m, b, f) m(b, 91, f)
# define SGL_RPT93(m, b, f) SGL_RPT92(m, b, f) m(b, 92, f)
# define SGL_RPT94(m, b, f) SGL_RPT93(m, b, f) m(b, 93, f)
# define SGL_RPT95(m, b, f) SGL_RPT94(m, b, f) m(b, 94, f)
# define SGL_RPT96(m, b, f) SGL_RPT95(m, b, f) m(b, 95, f)
# define SGL_RPT97(m, b, f) SGL_RPT96(m, b, f) m(b, 96, f)
# define SGL_RPT98(m, b, f) SGL_RPT97(m, b, f) m(b, 97, f)
# define SGL_RPT99(m, b, f) SGL_RPT98(m, b, f) m(b, 98, f)
# define SGL_RPT100(m, b, f) SGL_RPT99(m, b, f) m(b, 99, f)
# define SGL_RPT101(m, b, f) SGL_RPT100(m, b, f) m(b, 100, f)
# define SGL_RPT102(m, b, f) SGL_RPT101(m, b, f) m(b, 101, f)
# define SGL_RPT103(m, b, f) SGL_RPT102(m, b, f) m(b, 102, f)
# define SGL_RPT104(m, b, f) SGL_RPT103(m, b, f) m(b, 103, f)
# define SGL_RPT105(m, b, f) SGL_RPT104(m, b, f) m(b, 104, f)
# define SGL_RPT106(m, b, f) SGL_RPT105(m, b, f) m(b, 105, f)
# define SGL_RPT107(m, b, f) SGL_RPT106(m, b, f) m(b, 106, f)
# define SGL_RPT108(m, b, f) SGL_RPT107(m, b, f) m(b, 107, f)
# define SGL_RPT109(m, b, f) SGL_RPT108(m, b, f) m(b, 108, f)
# define SGL_RPT110(m, b, f) SGL_RPT109(m, b, f) m(b, 109, f)
# define SGL_RPT111(m, b, f) SGL_RPT110(m, b, f) m(b, 110, f)
# define SGL_RPT112(m, b, f) SGL_RPT111(m, b, f) m(b, 111, f)
# define SGL_RPT113(m, b, f) SGL_RPT112(m, b, f) m(b, 112, f)
# define SGL_RPT114(m, b, f) SGL_RPT113(m, b, f) m(b, 113, f)
# define SGL_RPT115(m, b, f) SGL_RPT114(m, b, f) m(b, 114, f)
# define SGL_RPT116(m, b, f) SGL_RPT115(m, b, f) m(b, 115, f)
# define SGL_RPT117(m, b, f) SGL_RPT116(m, b, f) m(b, 116, f)
# define SGL_RPT118(m, b, f) SGL_RPT117(m, b, f) m(b, 117, f)
# define SGL_RPT119(m, b, f) SGL_RPT118(m, b, f) m(b, 118, f)
# define SGL_RPT120(m, b, f) SGL_RPT119(m, b, f) m(b, 119, f)
# define SGL_RPT121(m, b, f) SGL_RPT120(m, b, f) m(b, 120, f)
# define SGL_RPT122(m, b, f) SGL_RPT121(m, b, f) m(b, 121, f)
# define SGL_RPT123(m, b, f) SGL_RPT122(m, b, f) m(b, 122, f)
# define SGL_RPT124(m, b, f) SGL_RPT123(m, b, f) m(b, 123, f)
# define SGL_RPT125(m, b, f) SGL_RPT124(m, b, f) m(b, 124, f)
# define SGL_RPT126(m, b, f) SGL_RPT125(m, b, f) m(b, 125, f)
# define SGL_RPT127(m, b, f) SGL_RPT126(m, b, f) m(b, 126, f)
# define SGL_RPT128(m, b, f) SGL_RPT127(m, b, f) m(b, 127, f)
# define SGL_RPT129(m, b, f) SGL_RPT128(m, b, f) m(b, 128, f)
# define SGL_RPT130(m, b, f) SGL_RPT129(m, b, f) m(b, 129, f)
# define SGL_RPT131(m, b, f) SGL_RPT130(m, b, f) m(b, 130, f)
# define SGL_RPT132(m, b, f) SGL_RPT131(m, b, f) m(b, 131, f)
# define SGL_RPT133(m, b, f) SGL_RPT132(m, b, f) m(b, 132, f)
# define SGL_RPT134(m, b, f) SGL_RPT133(m, b, f) m(b, 133, f)
# define SGL_RPT135(m, b, f) SGL_RPT134(m, b, f) m(b, 134, f)
# define SGL_RPT136(m, b, f) SGL_RPT135(m, b, f) m(b, 135, f)
# define SGL_RPT137(m, b, f) SGL_RPT136(m, b, f) m(b, 136, f)
# define SGL_RPT138(m, b, f) SGL_RPT137(m, b, f) m(b, 137, f)
# define SGL_RPT139(m, b, f) SGL_RPT138(m, b, f) m(b, 138, f)
# define SGL_RPT140(m, b, f) SGL_RPT139(m, b, f) m(b, 139, f)
# define SGL_RPT141(m, b, f) SGL_RPT140(m, b, f) m(b, 140, f)
# define SGL_RPT142(m, b, f) SGL_RPT141(m, b, f) m(b, 141, f)
# define SGL_RPT143(m, b, f) SGL_RPT142(m, b, f) m(b, 142, f)
# define SGL_RPT144(m, b, f) SGL_RPT143(m, b, f) m(b, 143, f)
# define SGL_RPT145(m, b, f) SGL_RPT144(m, b, f) m(b, 144, f)
# define SGL_RPT146(m, b, f) SGL_RPT145(m, b, f) m(b, 145, f)
# define SGL_RPT147(m, b, f) SGL_RPT146(m, b, f) m(b, 146, f)
# define SGL_RPT148(m, b, f) SGL_RPT147(m, b, f) m(b, 147, f)
# define SGL_RPT149(m, b, f) SGL_RPT148(m, b, f) m(b, 148, f)
# define SGL_RPT150(m, b, f) SGL_RPT149(m, b, f) m(b, 149, f)
# define SGL_RPT151(m, b, f) SGL_RPT150(m, b, f) m(b, 150, f)
# define SGL_RPT152(m, b, f) SGL_RPT151(m, b, f) m(b, 151, f)
# define SGL_RPT153(m, b, f) SGL_RPT152(m, b, f) m(b, 152, f)
# define SGL_RPT154(m, b, f) SGL_RPT153(m, b, f) m(b, 153, f)
# define SGL_RPT155(m, b, f) SGL_RPT154(m, b, f) m(b, 154, f)
# define SGL_RPT156(m, b, f) SGL_RPT155(m, b, f) m(b, 155, f)
# define SGL_RPT157(m, b, f) SGL_RPT156(m, b, f) m(b, 156, f)
# define SGL_RPT158(m, b, f) SGL_RPT157(m, b, f) m(b, 157, f)
# define SGL_RPT159(m, b, f) SGL_RPT158(m, b, f) m(b, 158, f)
# define SGL_RPT160(m, b, f) SGL_RPT159(m, b, f) m(b, 159, f)
# define SGL_RPT161(m, b, f) SGL_RPT160(m, b, f) m(b, 160, f)
# define SGL_RPT162(m, b, f) SGL_RPT161(m, b, f) m(b, 161, f)
# define SGL_RPT163(m, b, f) SGL_RPT162(m, b, f) m(b, 162, f)
# define SGL_RPT164(m, b, f) SGL_RPT163(m, b, f) m(b, 163, f)
# define SGL_RPT165(m, b, f) SGL_RPT164(m, b, f) m(b, 164, f)
# define SGL_RPT166(m, b, f) SGL_RPT165(m, b, f) m(b, 165, f)
# define SGL_RPT167(m, b, f) SGL_RPT166(m, b, f) m(b, 166, f)
# define SGL_RPT168(m, b, f) SGL_RPT167(m, b, f) m(b, 167, f)
# define SGL_RPT169(m, b, f) SGL_RPT168(m, b, f) m(b, 168, f)
# define SGL_RPT170(m, b, f) SGL_RPT169(m, b, f) m(b, 169, f)
# define SGL_RPT171(m, b, f) SGL_RPT170(m, b, f) m(b, 170, f)
# define SGL_RPT172(m, b, f) SGL_RPT171(m, b, f) m(b, 171, f)
# define SGL_RPT173(m, b, f) SGL_RPT172(m, b, f) m(b, 172, f)
# define SGL_RPT174(m, b, f) SGL_RPT173(m, b, f) m(b, 173, f)
# define SGL_RPT175(m, b, f) SGL_RPT174(m, b, f) m(b, 174, f)
# define SGL_RPT176(m, b, f) SGL_RPT175(m, b, f) m(b, 175, f)
# define SGL_RPT177(m, b, f) SGL_RPT176(m, b, f) m(b, 176, f)
# define SGL_RPT178(m, b, f) SGL_RPT177(m, b, f) m(b, 177, f)
# define SGL_RPT179(m, b, f) SGL_RPT178(m, b, f) m(b, 178, f)
# define SGL_RPT180(m, b, f) SGL_RPT179(m, b, f) m(b, 179, f)
# define SGL_RPT181(m, b, f) SGL_RPT180(m, b, f) m(b, 180, f)
# define SGL_RPT182(m, b, f) SGL_RPT181(m, b, f) m(b, 181, f)
# define SGL_RPT183(m, b, f) SGL_RPT182(m, b, f) m(b, 182, f)
# define SGL_RPT184(m, b, f) SGL_RPT183(m, b, f) m(b, 183, f)
# define SGL_RPT185(m, b, f) SGL_RPT184(m, b, f) m(b, 184, f)
# define SGL_RPT186(m, b, f) SGL_RPT185(m, b, f) m(b, 185, f)
# define SGL_RPT187(m, b, f) SGL_RPT186(m, b, f) m(b, 186, f)
# define SGL_RPT188(m, b, f) SGL_RPT187(m, b, f) m(b, 187, f)
# define SGL_RPT189(m, b, f) SGL_RPT188(m, b, f) m(b, 188, f)
# define SGL_RPT190(m, b, f) SGL_RPT189(m, b, f) m(b, 189, f)
# define SGL_RPT191(m, b, f) SGL_RPT190(m, b, f) m(b, 190, f)
# define SGL_RPT192(m, b, f) SGL_RPT191(m, b, f) m(b, 191, f)
# define SGL_RPT193(m, b, f) SGL_RPT192(m, b, f) m(b, 192, f)
# define SGL_RPT194(m, b, f) SGL_RPT193(m, b, f) m(b, 193, f)
# define SGL_RPT195(m, b, f) SGL_RPT194(m, b, f) m(b, 194, f)
# define SGL_RPT196(m, b, f) SGL_RPT195(m, b, f) m(b, 195, f)
# define SGL_RPT197(m, b, f) SGL_RPT196(m, b, f) m(b, 196, f)
# define SGL_RPT198(m, b, f) SGL_RPT197(m, b, f) m(b, 197, f)
# define SGL_RPT199(m, b, f) SGL_RPT198(m, b, f) m(b, 198, f)
# define SGL_RPT200(m, b, f) SGL_RPT199(m, b, f) m(b, 199, f)
# define SGL_RPT201(m, b, f) SGL_RPT200(m, b, f) m(b, 200, f)
# define SGL_RPT202(m, b, f) SGL_RPT201(m, b, f) m(b, 201, f)
# define SGL_RPT203(m, b, f) SGL_RPT202(m, b, f) m(b, 202, f)
# define SGL_RPT204(m, b, f) SGL_RPT203(m, b, f) m(b, 203, f)
# define SGL_RPT205(m, b, f) SGL_RPT204(m, b, f) m(b, 204, f)
# define SGL_RPT206(m, b, f) SGL_RPT205(m, b, f) m(b, 205, f)
# define SGL_RPT207(m, b, f) SGL_RPT206(m, b, f) m(b, 206, f)
# define SGL_RPT208(m, b, f) SGL_RPT207(m, b, f) m(b, 207, f)
# define SGL_RPT209(m, b, f) SGL_RPT208(m, b, f) m(b, 208, f)
# define SGL_RPT210(m, b, f) SGL_RPT209(m, b, f) m(b, 209, f)
# define SGL_RPT211(m, b, f) SGL_RPT210(m, b, f) m(b, 210, f)
# define SGL_RPT212(m, b, f) SGL_RPT211(m, b, f) m(b, 211, f)
# define SGL_RPT213(m, b, f) SGL_RPT212(m, b, f) m(b, 212, f)
# define SGL_RPT214(m, b, f) SGL_RPT213(m, b, f) m(b, 213, f)
# define SGL_RPT215(m, b, f) SGL_RPT214(m, b, f) m(b, 214, f)
# define SGL_RPT216(m, b, f) SGL_RPT215(m, b, f) m(b, 215, f)
# define SGL_RPT217(m, b, f) SGL_RPT216(m, b, f) m(b, 216, f)
# define SGL_RPT218(m, b, f) SGL_RPT217(m, b, f) m(b, 217, f)
# define SGL_RPT219(m, b, f) SGL_RPT218(m, b, f) m(b, 218, f)
# define SGL_RPT220(m, b, f) SGL_RPT219(m, b, f) m(b, 219, f)
# define SGL_RPT221(m, b, f) SGL_RPT220(m, b, f) m(b, 220, f)
# define SGL_RPT222(m, b, f) SGL_RPT221(m, b, f) m(b, 221, f)
# define SGL_RPT223(m, b, f) SGL_RPT222(m, b, f) m(b, 222, f)
# define SGL_RPT224(m, b, f) SGL_RPT223(m, b, f) m(b, 223, f)
# define SGL_RPT225(m, b, f) SGL_RPT224(m, b, f) m(b, 224, f)
# define SGL_RPT226(m, b, f) SGL_RPT225(m, b, f) m(b, 225, f)
# define SGL_RPT227(m, b, f) SGL_RPT226(m, b, f) m(b, 226, f)
# define SGL_RPT228(m, b, f) SGL_RPT227(m, b, f) m(b, 227, f)
# define SGL_RPT229(m, b, f) SGL_RPT228(m, b, f) m(b, 228, f)
# define SGL_RPT230(m, b, f) SGL_RPT229(m, b, f) m(b, 229, f)
# define SGL_RPT231(m, b, f) SGL_RPT230(m, b, f) m(b, 230, f)
# define SGL_RPT232(m, b, f) SGL_RPT231(m, b, f) m(b, 231, f)
# define SGL_RPT233(m, b, f) SGL_RPT232(m, b, f) m(b, 232, f)
# define SGL_RPT234(m, b, f) SGL_RPT233(m, b, f) m(b, 233, f)
# define SGL_RPT235(m, b, f) SGL_RPT234(m, b, f) m(b, 234, f)
# define SGL_RPT236(m, b, f) SGL_RPT235(m, b, f) m(b, 235, f)
# define SGL_RPT237(m, b, f) SGL_RPT236(m, b, f) m(b, 236, f)
# define SGL_RPT238(m, b, f) SGL_RPT237(m, b, f) m(b, 237, f)
# define SGL_RPT239(m, b, f) SGL_RPT238(m, b, f) m(b, 238, f)
# define SGL_RPT240(m, b, f) SGL_RPT239(m, b, f) m(b, 239, f)
# define SGL_RPT241(m, b, f) SGL_RPT240(m, b, f) m(b, 240, f)
# define SGL_RPT242(m, b, f) SGL_RPT241(m, b, f) m(b, 241, f)
# define SGL_RPT243(m, b, f) SGL_RPT242(m, b, f) m(b, 242, f)
# define SGL_RPT244(m, b, f) SGL_RPT243(m, b, f) m(b, 243, f)
# define SGL_RPT245(m, b, f) SGL_RPT244(m, b, f) m(b, 244, f)
# define SGL_RPT246(m, b, f) SGL_RPT245(m, b, f) m(b, 245, f)
# define SGL_RPT247(m, b, f) SGL_RPT246(m, b, f) m(b, 246, f)
# define SGL_RPT248(m, b, f) SGL_RPT247(m, b, f) m(b, 247, f)
# define SGL_RPT249(m, b, f) SGL_RPT248(m, b, f) m(b, 248, f)
# define SGL_RPT250(m, b, f) SGL_RPT249(m, b, f) m(b, 249, f)
# define SGL_RPT251(m, b, f) SGL_RPT250(m, b, f) m(b, 250, f)
# define SGL_RPT252(m, b, f) SGL_RPT251(m, b, f) m(b, 251, f)
# define SGL_RPT253(m, b, f) SGL_RPT252(m, b, f) m(b, 252, f)
# define SGL_RPT254(m, b, f) SGL_RPT253(m, b, f) m(b, 253, f)
# define SGL_RPT255(m, b, f) SGL_RPT254(m, b, f) m(b, 254, f)
# define SGL_RPT256(m, b, f) SGL_RPT255(m, b, f) m(b, 255, f)
/*
 *	The compiler has a limit of 256 levels of nested macros, here 252 levels 
 *	are left which should be enough for using.
 */

#define SNGL_MSG(bgn_id, offset, f)	ON_MESSAGE((bgn_id+offset), f)

/*
 *	this macro is some like "ON_COMMAND_RANGE". To keep things simple, we use
 *	successive message counts for its second parameter.
 */
#define SGL_ON_MESSAGE_RANGE(bgn_id,count,f) SGL_RPT##count(SNGL_MSG, bgn_id, f)		


#endif	// #ifndef SPXGASLIBERROR_H

