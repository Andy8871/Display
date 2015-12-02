package com.TD.Display;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.TD.Controller.ControllerProtocol;
import com.TD.Controller.GUIParam;
import com.example.display.R;

import android.R.integer;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Messenger;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

public class DatastramActivity extends Activity implements ActivityIndentify, ControllerProtocol {

	private final String TAG = DatastramActivity.class.getSimpleName();
	private final String[] from = new String[]{"item", "value", "unit"};		/* HashMap所使用的key */
	private final int[] to = new int[]{R.id.dsitem, R.id.dsvalue, R.id.dsunit};	/* 适配器和数据容器对应的目标 */
	private int itemsInView;		/* 当前屏可以显示的Item数量 	*/
	private String title = null;	/* 当前屏显示的Title */
	private int counts = 0;			/* 从MultiSelect界面中选中的数据流条目 */			
	private int topItem = 0;		/* 当前屏显示的第一行Item ID */
	private String[] contents = null;	/* 数据流显示内容 ：包括数据流名称，数据流值，数据流单位 */
	private List<HashMap<String, Object>> data = null;	/* 用于显示数据流的数据容器 */
	private SimpleAdapter adapter = null;				/* 数据流显示界面适配器 */
	private ListView lv;			/* 数据流显示列表 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		Log.i(TAG, "Show Datastream");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_datastream);
		lv = (ListView)findViewById(R.id.ds);
		lv.setOnItemClickListener(new SelectItemListener());
		int lvHeight = 300;
		int itemHeight = 30;
		/* 根据ListView高度和Item高度计算出当前屏显示数据流数量 */
		itemsInView = getItemsInView(lvHeight,itemHeight);
		// 注册菜单广播消息接收器
		registBroadcastReceiver();
		
		/* 恢复上次保存的界面数据 */
		if (null != savedInstanceState)
		{
			counts = savedInstanceState.getInt("counts");
			topItem = savedInstanceState.getInt("topItems");
			title = savedInstanceState.getString("title");
			contents = savedInstanceState.getStringArray("data");
		}
		else {/* 第一次初始化界面 */
			Bundle args = getIntent().getExtras();
			GUIParam param = (GUIParam) args.getSerializable(CTL_GUI_PARAM);
			counts = param.getCounts();
			title = param.getTitle();
			contents = param.getContents();
			topItem = param.getTopItem();
			Bundle bundle = new Bundle();
			bundle.putInt(GUI_REQ, V_REQUEST_HAS_UPDATED_VIEW);
			bundle.putInt("topItem", topItem);
			bundle.putInt("itemsInView", itemsInView);
			sendDataToDiagnose(bundle);
		}
		
		/* 设置适配器 */
		data = new ArrayList<HashMap<String,Object>>(counts);
		if (counts > 0 && (counts * 3) <= contents.length)
		{
			for (int i = 0; i < counts * 3; i += 3)
			{
				HashMap<String, Object> item = new HashMap<String, Object>();
				item.put(from[0], contents[i]);			//item name
				item.put(from[1], contents[i + 1]);		//item value
				item.put(from[2], contents[i + 2]);		//item unit
				data.add(item);
			}
		}
		adapter = new SimpleAdapter(this, data, R.layout.item_dslist, from, to);
		lv.setAdapter(adapter);
	}

	@Override
	protected void onDestroy() {
		Log.i(TAG, "onDestroy");
		Bundle bundle = new Bundle();
		bundle.putInt(GUI_REQ, V_REQUEST_EXIT_DS);
		sendDataToDiagnose(bundle);
		unregisterReceiver(dsReceiver);
		super.onDestroy();
	}
	
	@Override
	protected void onSaveInstanceState(Bundle outState) {
		outState.putInt("counts", counts);
		outState.putInt("topItem", topItem);
		outState.putString("title", title);
		outState.putStringArray("data", contents);
		super.onSaveInstanceState(outState);
	}

	// 注册广播消息接收器
	private void registBroadcastReceiver() {
		Log.i(TAG, "registBroadcast");
		IntentFilter iFilter = new IntentFilter();
		iFilter.addAction(RECEIVER_DS);
		registerReceiver(dsReceiver, iFilter);
	}

	private BroadcastReceiver dsReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			if (RECEIVER_DS.equals(intent.getAction())) {
				Bundle args = intent.getExtras();
				int nType = args.getInt(CTL_ANS);
				Log.i(TAG, TAG + " accept a broadcast, result " + nType);
				switch (nType) {
				case C_SHOW_DS: {
					
					GUIParam param = (GUIParam) args.getSerializable(CTL_GUI_PARAM);
					updateView(param);
					/*Bundle bundle = new Bundle();
					bundle.putInt(GUI_REQ, V_REQUEST_HAS_UPDATED_VIEW);
					bundle.putInt("topItem", topItem);
					bundle.putInt("itemsInView", itemsInView);
					sendDataToDiagnose(bundle);*/
					break;
				}
				}
			}
		}
	};

	/**
	 * 	Function:返回数据给诊断模块
	 *	Params:@param bundle 需要返回的数据
	 *  Returns:void 
	 *  @author Andy  DateTime 2015-12-1 下午12:16:17
	 */
	private void sendDataToDiagnose(Bundle bundle) {
		Intent intent = new Intent(RECEIVER_SERVER);
		intent.putExtras(bundle);
		sendBroadcast(intent);
	}
	
	/**
	 * 	Function:更新ListView
	 *	Params:@param param：需要更新的数据内容
	 *  Returns:void 
	 *  @author Andy  DateTime 2015-12-1 下午12:15:44
	 */
	private void updateView(GUIParam param)
	{
		int nCounts = param.getCounts();
		String[] update = param.getContents();
		data.clear();
		if (nCounts > 0 && ((nCounts * 3) <= update.length))
		{
			for (int i = 0; i < nCounts * 3; i += 3)
			{
				HashMap<String, Object> item = new HashMap<String, Object>();
				item.put(from[0], contents[i]);			//item name
				item.put(from[1], contents[i + 1]);		//item value
				item.put(from[2], contents[i + 2]);		//item unit
				data.add(item);
			}
		}
		adapter.notifyDataSetChanged();
	}
	
	
	
	/**
	 * 	Function:获取当前屏可以显示的Items数量
	 *	Params:@param viewHeight
	 *	Params:@param itemHeight
	 *	Params:@return
	 *  Returns:int 
	 *  @author Andy  DateTime 2015-12-1 上午11:51:50
	 */
	private int getItemsInView(int viewHeight, int itemHeight)
	{
		/* 当前屏List显示容器高度和Item的高度都必须大于0 */
		if (viewHeight <= 0 || itemHeight <= 0)
			return -1;
		/* 当前屏List显示容器高度小于Item的高度则显示条数为0 */
		if (viewHeight < itemHeight)
			return 0;
		
		return (viewHeight / itemHeight);
	}
	
	
	
	/**
	 * 
	 *  Class Name: DatastramActivity.java
	 *  Function: 列表点击事件监听器
	 *  
	 *  Modifications:   
	 *  
	 *  @author Andy  DateTime 2015-11-30 下午2:56:58    
	 *  @version 1.0
	 */
	private final class SelectItemListener implements OnItemClickListener{

		@Override
		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {
		}
		
	}
}
