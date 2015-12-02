package com.TD.Display;

import com.TD.Controller.ControllerProtocol;
import com.example.display.R;
import android.app.Activity;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity implements ActivityIndentify, ControllerProtocol{
	private final String TAG = MainActivity.class.getSimpleName();
	private Button btnOBDIIButton;
	private Button btnTest;
	private Intent itService = new Intent(SERVICE_DIAGNOSE);
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		btnClickListener clickListener = new btnClickListener();

		btnOBDIIButton = (Button) findViewById(R.id.btnOBDII);
		btnOBDIIButton.setOnClickListener(clickListener);

		btnTest = (Button) findViewById(R.id.btn);
		btnTest.setOnClickListener(clickListener);
		Log.i(TAG, "before startService");
		

		Log.i(TAG, "after startService");
		//注册广播消息接收器
		registBroadcastReceiver();
		
		
		// controller = new Controller();
		// controller.addOutboxHandler(new Handler(this));
	}

	@Override
	protected void onDestroy() {
		//stopService(itService);
		unregisterReceiver(mainReceiver);
		super.onDestroy();
	}

	// 按钮响应函数
	class btnClickListener implements OnClickListener {

		@Override
		public void onClick(View v) {
			int iBtnId = v.getId();
			switch (iBtnId) {
			case R.id.btnOBDII:
				Log.i(TAG, "btnClickListener");
				//启动诊断服务
				startService(itService);
				/*
				Intent intent = new Intent(RECEIVER_SERVER);
				Bundle bundle = new Bundle();
				bundle.putInt(GUI_REQ, V_REQUEST_START_DIAGNOSE);
				intent.putExtras(bundle);
				sendBroadcast(intent);*/
				break;
			case R.id.btn:
			}
		}

	}

	// 注册广播消息接收器
	private void registBroadcastReceiver() {
		Log.i(TAG, "registBroadcast");
		IntentFilter iFilter = new IntentFilter();
		iFilter.addAction(RECEIVER_MAIN);
		registerReceiver(mainReceiver, iFilter);
	}

	private BroadcastReceiver mainReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			/*if (RECEIVER_MAIN.equals(intent.getAction())) {
				Bundle bundle = intent.getExtras();
				int nType = bundle.getInt(CTL_ANS);
				Log.i(TAG, TAG + " accept a broadcast, result " + nType);
				switch (nType) {
				case C_SHOW_MESSAGEBOX:
				{
					String[] content = (String[])bundle.getSerializable(CTL_GUI_PARAM);
					Toast toast = Toast.makeText(getApplicationContext(), content[0], Toast.LENGTH_LONG);
					toast.setGravity(Gravity.CENTER, 0, 0);
					toast.show();
					break;
				}
				default:
					break;
				}
			}*/
		}
	};
	
}
