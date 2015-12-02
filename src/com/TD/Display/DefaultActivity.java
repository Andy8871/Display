package com.TD.Display;

import java.util.ArrayList;

import com.TD.Controller.ControllerProtocol;
import com.TD.View.widget.DefaultListFragment;
import com.TD.View.widget.DefaultListFragment.ItemClikListener;
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

public class DefaultActivity extends Activity implements ItemClikListener,
ActivityIndentify, ControllerProtocol {

	private final String TAG = DefaultActivity.class.getSimpleName();

	private ArrayList<String> dataList;

	public ArrayList<String> getDataList() {
		return dataList;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {

		Log.i(TAG, "Show menu");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_default);

		// 注册菜单广播消息接收器
		registBroadcastReceiver();

		// 获取菜单信息并显示
		Bundle bundle = getIntent().getBundleExtra("SHOW");
		dataList = bundle.getStringArrayList("CONTENT");
		Fragment fg = new DefaultListFragment();
		FragmentManager fmFragmentManager = getFragmentManager();
		FragmentTransaction fragmentTransaction = fmFragmentManager
				.beginTransaction();
		fragmentTransaction.replace(R.id.defaultFragment, fg);
		fragmentTransaction.commit();
	}

	@Override
	protected void onDestroy() {
		Log.i(TAG, "onDestroy");
		Bundle bundle = new Bundle();
		bundle.putInt(GUI_REQ, V_REQUEST_EXIT_DEFAULT);
		sendDataToDiagnose(bundle);
		unregisterReceiver(menuReceiver);
		super.onDestroy();
	}

	@Override
	public void OnItemClickListener(int position) {
		// TODO Auto-generated method stub
		String strLog = "Select item[" + position + "]";
		Log.i(TAG, strLog);
		Bundle bundle = new Bundle();
		bundle.putInt(GUI_REQ, V_REQUEST_SELECT_ITEM);
		sendDataToDiagnose(bundle);
	}

	// 注册广播消息接收器
	private void registBroadcastReceiver() {
		Log.i(TAG, "registBroadcast");
		IntentFilter iFilter = new IntentFilter();
		iFilter.addAction(RECEIVER_DEFAULT);
		registerReceiver(menuReceiver, iFilter);
	}

	private BroadcastReceiver menuReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			/*if (RECEIVER_DEFAULT.equals(intent.getAction())) {
				Bundle bundle = intent.getBundleExtra("SHOW");
				int nType = bundle.getInt("ANSTYPE");
				Log.i(TAG, TAG + " accept a broadcast, result " + nType);
				switch (nType) {
				case C_SHOW_MENU: {
					break;
				}
				case C_SHOW_DTC: {
					Log.i(TAG, "S_SHOW_DTC");
					ArrayList<String> dtcList = bundle.getStringArrayList("ANSCONTENT");
					for (int i = 0; i < dtcList.size(); ++i)
					{
						Log.i(TAG, dtcList.get(i));
					}
					//ArrayList<String> dtcList = new ArrayList<String>();
					Bundle bd = new Bundle();
					bd.putStringArrayList("DTCLIST", dtcList);
					Intent it = new Intent(ACTIVITY_DTC);
					it.putExtra("SHOWDTC", bd);
					startActivity(it);
					break;
				}
				}
			}*/
		}

	};

	private void sendDataToDiagnose(Bundle bundle) {
		Intent intent = new Intent(RECEIVER_SERVER);
		intent.putExtras(bundle);
		sendBroadcast(intent);
	}

}
