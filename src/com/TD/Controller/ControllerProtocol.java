package com.TD.Controller;
public interface ControllerProtocol {

	// 广播消息
	public final String RECEIVER_MAIN = "com.TD.Display.MainActivity.RECEIVER";
	public final String RECEIVER_MENU = "com.TD.Display.MenuActivity.RECEIVER";
	public final String RECEIVER_DTC = "com.TD.Display.DtcActivity.RECEIVER";
	public final String RECEIVER_DS = "com.TD.Display.DatastreamActivity.RECEIVER";
	public final String RECEIVER_ECUINFO = "com.TD.Display.ECUInfoActivity.RECEIVER";
	public final String RECEIVER_DEFAULT = "com.TD.Display.DefaultActivity.RECEIVER";
	public final String RECEIVER_SERVER = "com.TD.Display.DiagnoseService.RECEIVER";

	// 诊断服务
	public final String SERVICE_DIAGNOSE = "com.TD.Display.DiagnoseService.SERVICE";

	public final String GUI_REQ = "REQUEST";
	public final String CTL_ANS = "ANSWER";
	public final String CTL_GUI_PARAM = "GUIPARAM";
	public final String GUI_SEL_ITEM = "ITEM";
	public final String GUI_BTN_ID = "BUTTONID";
	public final String GUI_SHOW_LINE = "FIRSTLINEID";
	
	public final int V_REQUEST_START_DIAGNOSE = 1001;// empty
	public final int V_REQUEST_SELECT_ITEM = 1002;
	public final int V_REQUEST_EXIT_DIAGNOSE = 1003;
	public final int V_REQUEST_EXIT_MENU = 1004;
	public final int V_REQUEST_EXIT_DTC = 1005;
	public final int V_REQUEST_EXIT_DS = 1006;
	public final int V_REQUEST_EXIT_ECUINFO = 1007;
	public final int V_REQUEST_EXIT_ACTION = 1008;
	public final int V_REQUEST_EXIT_DEFAULT = 1009;
	public final int V_REQUEST_CLICK_BTN = 1010;
	public final int V_REQUEST_EXIT_MULTISELECT = 1011;
	public final int V_REQUEST_HAS_UPDATED_VIEW = 1012;
	
	public final int C_SHOW_MENU = 2001;
	public final int C_SHOW_DTC = 2002;
	public final int C_SHOW_DS = 2003;
	public final int C_SHOW_ECUINFO = 2004;
	public final int C_SHOW_ACTION = 2005;
	public final int C_SHOW_DEFAULT = 2006;
	public final int C_UPDATE_DS = 2007;
	public final int C_CTRL_SELECT_ITEM = 2008;
	public final int C_CTRL_CLICK_BTN = 2009;
	public final int C_EXIT_DIAGNOSE = 2020;
	public final int C_START_DIAGNOSE =2030;
	public final int C_OTHER		= 2040;
	public final int C_SHOW_MESSAGEBOX = 2041;

	/*public final int M_REQUEST_SHOW = 3001;
	public final int M_REQUEST_UPDATE = 3002;
	public final int M_REQUEST_SHOWDS = 3003;
	public final int M_REQUEST_SHOWECUINFO = 3004;
	public final int M_REQUEST_SHOWACTION = 3005;
	public final int M_REQUEST_SHOWDEFAULT = 3006;
	public final int M_REQUEST_SHOWMENU = 3007;
	public final int M_REQUEST_SHOWDTC = 3008;
	*/
	/* 用户界面按钮信息 ，用户自定义添加*/
	public final int GUI_MSG_BTN_NOBUTTON = 0X00;
	public final int GUI_MSG_BTN_OK = 	0X01;
	public final int GUI_MSG_BTN_CANCEL = 0X02;
	public final int GUI_MSG_BTN_OKCANCEL = 0X03;
	public final int GUI_MSG_BTN_YESNO = 0X04;
	public final int GUI_MSG_BTN_YESNOCANCEL = 0X05;
	public final int GUI_MSG_BTN_RETRYCANCEL = 0X06;
	public final int GUI_MSG_BTN_ABORTRETRYIGNORE = 0X07;
	public final int GUI_MSG_BTN_IGNOREABORT = 0X08;
	public final int GUI_MSG_BTN_OK_RETURN = 0X09;
	public final int GUI_MSG_BTN_NEXTCANCEL = 0X16;
	public final int GUI_MSG_BTN_PREVNEXTCANCEL = 0X17;
	public final int GUI_MSG_BTN_PREVFINISH = 0X19;
	public final int GUI_MSG_BTN_PRINT = 	0X40;
	public final int GUI_MSG_BTN_HELP = 	0X80;

	/* 系统按钮信息 */
	public final int adsIDNOCLICK = 0x00;
	public final int adsIDOK = 0x01;
	public final int adsIDCANCEL = 0x02;
	public final int adsIDYES = 0x03;
	public final int adsIDNO = 0x04;
	public final int adsIDRETRY = 0x05;
	public final int adsIDIGNORE = 0x06;
	public final int adsIDABORT = 0x07;
	public final int adsIDPREV = 0x08;
	public final int adsIDNEXT = 0x09;
	public final int adsIDFINISH = 0x0A;
	public final int adsIDBACK = 0x0B;
	public final int adsIDCHANNEL = 0x0C;// AUDI
	public final int adsIDSHORTTESTENTER = 0x0D;// SHORTTESTENTER
	public final int adsIDSTOP = 0x1D;// Datadrv

	/* 诊断与UI通讯数据类型 */
	public final int GUI_MENU = 0;							//菜单显示界面
	public final int GUI_TROUBLE_CODE = 10;					//故障码显示界面
	public final int GUI_DATA_STREAM = 20;					//数据流显示界面
	public final int GUI_ACTIVE_TEST = 30;					//动作测试显示界面
	public final int GUI_MULTI_SELECT = 40;					//多项选择显示界面
	public final int GUI_ECU_INFORMATION = 50;				//车辆基本信息显示界面
	public final int GUI_MESSAGE_BOX = 60;					//弹出对话框显示界面
	public final int GUI_INPUT = 70;						//输入界面
	public final int GUI_PROGRESS_BAR = 80;					//进度条显示控件
	public final int GUI_PICTURE = 90;						//显示图片
	public final int GUI_QUICK_SCAN = 100;					//快速扫描显示界面
	public final int GUI_WARNING_BOX = 101;					//弹出警告提示框
	public final int GUI_SPECIAL_TEST = 110;				//特殊功能显示界面
	public final int GUI_ECUID = 130;						//车辆ECU ID
	public final int GUI_LOADSTRING = 0xFD;					//载入字符串
	public final int GUI_DIAGNOSE_EXIT = 0xFE;				//退出诊断
	public final int GUI_DIAGNOSE_RUN_SUCCESS = 0xFF;		//成功进入诊断
}
