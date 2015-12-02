package com.TD.Controller;

import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

public class ECUInfoState implements ControllerState, ControllerProtocol{

	private final static String TAG = ECUInfoState.class.getSimpleName();
	private final static String STATE = RECEIVER_ECUINFO;
	private final Controller controller;
	
	public ECUInfoState(Controller controller)
	{
		this.controller = controller;
	}
	
	@Override
	public boolean handleMessage(Message msg) {
		/*switch (msg.what) {
		case M_REQUEST_SHOWMENU:
		{
			break;
		}
		case M_REQUEST_SHOWDTC:
		{
			break;
		}
		case M_REQUEST_SHOWDS:
			Log.i(TAG, "M_REQUEST_SHOWDS");
			break;
		case M_REQUEST_SHOWECUINFO:
			Log.i(TAG, "M_REQUEST_SHOWECUINFO");
			break;

		default:
			break;
		}*/
		return false;
	}
	@Override
	public boolean receiveBroadcast(Intent intent) {
		Log.i(TAG, TAG + " accept a broadcast from UI");
		Bundle bundle = intent.getExtras();
		int nType = bundle.getInt(GUI_REQ, 0);
		switch (nType) {
		case V_REQUEST_EXIT_ECUINFO:
		{
			byte[] cmd = new byte[8];
			cmd[0] = 0;
			cmd[1] = 6;
			cmd[4] = -1;
			cmd[5] = -1;
			controller.changeState(new MenuState(controller));
			controller.resultToDiagnose(cmd);
			break;
		}
		case V_REQUEST_SELECT_ITEM:
		{
			Log.i(TAG, "V_REQUEST_SELECT_ITEM");
			//ResultToDiagnose();
			break;
		}
		default:
			break;
		}
		return false;
	}

	@Override
	public String getCurrentState() {
		return STATE;
	}
}
