package com.TD.Display;

import java.util.ArrayList;

import com.TD.Controller.ControllerProtocol;
import com.TD.Controller.GUIParam;
import com.example.display.R;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class MenuActivity extends Activity implements ActivityIndentify, ControllerProtocol {

	private final String TAG = MenuActivity.class.getSimpleName();
	private final static String KEY_LEVEL = "level";
	private final static String KEY_DATA = "data";
	private final static String KEY_TITILE = "title";
	private ArrayList<String> data;
	private ArrayAdapter<String> adapter;
	private ListView lv;
	private String title = null;
	private int level = 0;
	private boolean isBack = false;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		StackTraceElement[] sElements = Thread.currentThread().getStackTrace();
		Log.i(TAG,sElements[2].getMethodName() + "--->Line:" + sElements[2].getLineNumber());
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_menu);
		lv = (ListView)findViewById(R.id.menu);
		// 注册菜单广播消息接收器
		registBroadcastReceiver();
		
		if (null != savedInstanceState)
		{	//从前一次activity中获取信息
			level = savedInstanceState.getInt(KEY_LEVEL);
			title = savedInstanceState.getString(KEY_TITILE);
			data = savedInstanceState.getStringArrayList(KEY_DATA);
		}
		else {// 第一次启动MenuActivity获取菜单信息并显示
			level++;
			Bundle bundle = getIntent().getExtras();
			GUIParam param = (GUIParam) bundle.getSerializable(CTL_GUI_PARAM);
			title = param.getTitle();
			
			String[] arrContents = param.getContents();
			data = new ArrayList<String>(arrContents.length);
			for (String e : arrContents) {
				data.add(e);
			}
		}
		
		setTitle(title);
		adapter = new ArrayAdapter<String>(this, R.layout.item_menu, R.id.item, data);
		lv.setAdapter(adapter);
		lv.setOnItemClickListener(new OnSelectItemListener());
	}
	
	
	@Override
	protected void onSaveInstanceState(Bundle outState) {
		StackTraceElement[] sElements = Thread.currentThread().getStackTrace();
		Log.i(TAG, sElements[2].getMethodName() + "--->Line:" + sElements[2].getLineNumber());
		outState.putInt(KEY_LEVEL, level);
		outState.putStringArrayList(KEY_DATA, data);
		outState.putString(KEY_TITILE, title);
		super.onSaveInstanceState(outState);
	}
	

	
	@Override
	public void onBackPressed() {
		isBack = true;
		level--;
		Bundle bundle = new Bundle();
		bundle.putInt(GUI_REQ, V_REQUEST_EXIT_MENU);
		sendDataToDiagnose(bundle);
		if (level <= 0)
			super.onBackPressed();
	}


	@Override
	protected void onRestart() {
		StackTraceElement[] sElements = Thread.currentThread().getStackTrace();
		Log.i(TAG, sElements[2].getMethodName() + "--->Line:" + sElements[2].getLineNumber());
		super.onRestart();
	}


	@Override
	protected void onResume() {
		StackTraceElement[] sElements = Thread.currentThread().getStackTrace();
		Log.i(TAG, sElements[2].getMethodName() + "--->Line:" + sElements[2].getLineNumber());
		
		super.onResume();
	}


	@Override
	protected void onPause() {
		StackTraceElement[] sElements = Thread.currentThread().getStackTrace();
		Log.i(TAG, sElements[2].getMethodName() + "--->Line:" + sElements[2].getLineNumber());
		
		super.onPause();
	}
	

	@Override
	protected void onDestroy() {
		/*Bundle bundle = new Bundle();
		bundle.putInt(GUI_REQ, V_REQUEST_EXIT_MENU);
		sendDataToDiagnose(bundle);*/
		unregisterReceiver(menuReceiver);
		super.onDestroy();
	}
	
	// 注册广播消息接收器
	private void registBroadcastReceiver() {
		Log.i(TAG, "registBroadcast");
		IntentFilter iFilter = new IntentFilter();
		iFilter.addAction(RECEIVER_MENU);
		registerReceiver(menuReceiver, iFilter);
	}

	private BroadcastReceiver menuReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			if (RECEIVER_MENU.equals(intent.getAction())) {
				Bundle bundle = intent.getExtras();
				int nType = bundle.getInt(CTL_ANS);
				
				switch (nType) {
				case C_SHOW_MENU: {
					if (!isBack)
						level++;
					Log.i(TAG, "C_SHOW_MENU currnet menu level" + level);
					GUIParam param = (GUIParam) bundle.getSerializable(CTL_GUI_PARAM);
					updateView(param.getContents());
					isBack = false;
					break;
				}
				case C_EXIT_DIAGNOSE:
					Bundle args = new Bundle();
					args.putInt(GUI_REQ, V_REQUEST_EXIT_DIAGNOSE);
					sendDataToDiagnose(args);
					break;
				}
			}
		}

	};
	private void sendDataToDiagnose(Bundle bundle) {
		Intent intent = new Intent(RECEIVER_SERVER);
		intent.putExtras(bundle);
		sendBroadcast(intent);
	}
	
	
	private class OnSelectItemListener implements OnItemClickListener
	{

		@Override
		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {
			Log.i(TAG, "Select item[" + position + "]");
			Bundle bundle = new Bundle();
			bundle.putInt(GUI_REQ, V_REQUEST_SELECT_ITEM);
			bundle.putInt(GUI_SEL_ITEM, position);
			sendDataToDiagnose(bundle);
		}
		
	}
	
	private void updateView(String[] content)
	{
		data.clear();
		//ArrayList<String> menuAdapter = new ArrayList<String>(content.length);
		for (String e : content) {
			//menuAdapter.add(e);
			data.add(e);
			Log.i(TAG, e);
		}
		//data = menuAdapter;		//这种方式修改数据源时更新适配器不会生效		
		//data.addAll(menuAdapter);
		adapter.notifyDataSetChanged();
	}
}
