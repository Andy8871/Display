package com.TD.Display;

import java.util.ArrayList;

import com.TD.Controller.ControllerProtocol;
import com.TD.View.widget.DTCFragment.ItemClikListener;
import com.TD.View.widget.DTCFragment;
import com.example.display.R;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;

public class DTCActivity extends Activity implements ItemClikListener, ActivityIndentify, ControllerProtocol {
	private final String TAG = DTCActivity.class.getSimpleName();
	private ArrayList<String> dtcList;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_dtc);
		Log.i(TAG, "crate dtc activity");
		
		registBroadcastReceiver();
		// 获取故障码信息并显示
		Bundle bundle = getIntent().getBundleExtra("SHOW");
		dtcList = bundle.getStringArrayList("CONTENT");
		for (int i = 0; i < dtcList.size(); ++i)
		{
			Log.i(TAG, dtcList.get(i));
		}
		Fragment fg = new DTCFragment();
		FragmentManager fmFragmentManager = getFragmentManager();
		FragmentTransaction fragmentTransaction = fmFragmentManager
				.beginTransaction();
		fragmentTransaction.replace(R.id.dtcFragment, fg);
		fragmentTransaction.commit();
	}
	
	
	@Override
	protected void onDestroy() {
		Bundle bundle = new Bundle();
		bundle.putInt(GUI_REQ, V_REQUEST_EXIT_DTC);
		sendDataToDiagnose(bundle);
		unregisterReceiver(dtcReceiver);
		super.onDestroy();
	}

	@Override
	public void OnItemClickListener(int position) {
		// TODO Auto-generated method stub
		
	}
	
	public ArrayList<String> getDTCList()
	{
		return dtcList;
	}

	
	// 注册广播消息接收器
	private void registBroadcastReceiver() {
		Log.i(TAG, "registBroadcast");
		IntentFilter iFilter = new IntentFilter();
		iFilter.addAction(RECEIVER_DTC);
		registerReceiver(dtcReceiver, iFilter);
	}

	private BroadcastReceiver dtcReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			// TODO Auto-generated method stub

		}

	};
	
	private void sendDataToDiagnose(Bundle bundle) {
		Intent intent = new Intent(RECEIVER_SERVER);
		intent.putExtras(bundle);
		sendBroadcast(intent);
	}

	
}
