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
	private final String[] from = new String[]{"item", "value", "unit"};		/* HashMap��ʹ�õ�key */
	private final int[] to = new int[]{R.id.dsitem, R.id.dsvalue, R.id.dsunit};	/* ������������������Ӧ��Ŀ�� */
	private int itemsInView;		/* ��ǰ��������ʾ��Item���� 	*/
	private String title = null;	/* ��ǰ����ʾ��Title */
	private int counts = 0;			/* ��MultiSelect������ѡ�е���������Ŀ */			
	private int topItem = 0;		/* ��ǰ����ʾ�ĵ�һ��Item ID */
	private String[] contents = null;	/* ��������ʾ���� ���������������ƣ�������ֵ����������λ */
	private List<HashMap<String, Object>> data = null;	/* ������ʾ���������������� */
	private SimpleAdapter adapter = null;				/* ��������ʾ���������� */
	private ListView lv;			/* ��������ʾ�б� */
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		Log.i(TAG, "Show Datastream");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_datastream);
		lv = (ListView)findViewById(R.id.ds);
		lv.setOnItemClickListener(new SelectItemListener());
		int lvHeight = 300;
		int itemHeight = 30;
		/* ����ListView�߶Ⱥ�Item�߶ȼ������ǰ����ʾ���������� */
		itemsInView = getItemsInView(lvHeight,itemHeight);
		// ע��˵��㲥��Ϣ������
		registBroadcastReceiver();
		
		/* �ָ��ϴα���Ľ������� */
		if (null != savedInstanceState)
		{
			counts = savedInstanceState.getInt("counts");
			topItem = savedInstanceState.getInt("topItems");
			title = savedInstanceState.getString("title");
			contents = savedInstanceState.getStringArray("data");
		}
		else {/* ��һ�γ�ʼ������ */
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
		
		/* ���������� */
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

	// ע��㲥��Ϣ������
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
	 * 	Function:�������ݸ����ģ��
	 *	Params:@param bundle ��Ҫ���ص�����
	 *  Returns:void 
	 *  @author Andy  DateTime 2015-12-1 ����12:16:17
	 */
	private void sendDataToDiagnose(Bundle bundle) {
		Intent intent = new Intent(RECEIVER_SERVER);
		intent.putExtras(bundle);
		sendBroadcast(intent);
	}
	
	/**
	 * 	Function:����ListView
	 *	Params:@param param����Ҫ���µ���������
	 *  Returns:void 
	 *  @author Andy  DateTime 2015-12-1 ����12:15:44
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
	 * 	Function:��ȡ��ǰ��������ʾ��Items����
	 *	Params:@param viewHeight
	 *	Params:@param itemHeight
	 *	Params:@return
	 *  Returns:int 
	 *  @author Andy  DateTime 2015-12-1 ����11:51:50
	 */
	private int getItemsInView(int viewHeight, int itemHeight)
	{
		/* ��ǰ��List��ʾ�����߶Ⱥ�Item�ĸ߶ȶ��������0 */
		if (viewHeight <= 0 || itemHeight <= 0)
			return -1;
		/* ��ǰ��List��ʾ�����߶�С��Item�ĸ߶�����ʾ����Ϊ0 */
		if (viewHeight < itemHeight)
			return 0;
		
		return (viewHeight / itemHeight);
	}
	
	
	
	/**
	 * 
	 *  Class Name: DatastramActivity.java
	 *  Function: �б����¼�������
	 *  
	 *  Modifications:   
	 *  
	 *  @author Andy  DateTime 2015-11-30 ����2:56:58    
	 *  @version 1.0
	 */
	private final class SelectItemListener implements OnItemClickListener{

		@Override
		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {
		}
		
	}
}
