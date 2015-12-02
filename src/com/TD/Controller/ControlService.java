package com.TD.Controller;

import com.TD.Model.Diagnose;

import android.app.IntentService;
import android.content.Intent;
import android.util.Log;

public class ControlService extends IntentService{
	private static final String TAG = ControlService.class.getSimpleName();
	private Diagnose diagnose = new Diagnose();
	
	static{
		System.loadLibrary("OBDII");
	}
	
	public ControlService() {
		super(TAG);
	}

	@Override
	protected void onHandleIntent(Intent intent) {
		Log.i(TAG, "onHandleIntent");
		diagnose.RunDiagnose();
	}

	@Override
	public void onDestroy() {
		Log.i(TAG, "onDestroy");
		super.onDestroy();
	}
	
	

}
