package com.TD.Controller;

import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;


 /**
 *  Class Name: DTCState.java
 *  Function:
 *  
 *     Modifications:   
 *  
 *  @author Andy  DateTime 2015-10-26 ÏÂÎç5:44:33    
 *  @version 1.0
 */
public class DTCState implements ControllerState, ControllerProtocol{
	private final static String TAG = DTCState.class.getSimpleName();
	private final static String STATE = RECEIVER_DTC;
	private final Controller controller;
	
	public DTCState(Controller controller)
	{
		this.controller = controller;
	}

	@Override
	public boolean handleMessage(Message msg) {
		return false;
	}

	@Override
	public boolean receiveBroadcast(Intent intent) {
		Log.i(TAG, TAG + " accept a broadcast from UI");
		Bundle bundle = intent.getExtras();
		int nType = bundle.getInt(GUI_REQ, 0);
		switch (nType) {
		case V_REQUEST_EXIT_DTC:
		{
			Log.i(TAG,"V_REQUEST_EXIT_DTC");
			//controller.ResultToDiagnose();
			//CToJava.Exit();
			Log.i(TAG, "changeState to MenuState");
			controller.changeState(new MenuState(controller));
			break;
		}
		case V_REQUEST_SELECT_ITEM:
		{
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
