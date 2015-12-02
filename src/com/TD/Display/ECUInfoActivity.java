package com.TD.Display;

import java.util.ArrayList;
import java.util.HashMap;

import com.TD.Controller.ControllerProtocol;
import com.TD.Controller.GUIParam;
import com.example.display.R;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.ListView;
import android.widget.SimpleAdapter;

public class ECUInfoActivity extends Activity implements ActivityIndentify, ControllerProtocol{

	private final String TAG = ECUInfoActivity.class.getSimpleName();
	private SimpleAdapter adapter = null;
	private final String[] from = {"item", "value"};
	private final int[] to = {R.id.item, R.id.value};
	private ListView lView = null;
	private ArrayList<String> infoList;

	public ArrayList<String> getInfoList() {
		return infoList;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {

		Log.i(TAG, "Show ECU information");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_ecuinfo);
		
		lView = (ListView)findViewById(R.id.listView);
		// 获取版本信息并显示
		Bundle args = getIntent().getExtras();
		GUIParam param = (GUIParam) args.getSerializable(CTL_GUI_PARAM);
		setTitle(param.getTitle());
		ArrayList<HashMap<String, Object>> data = getData(param);
		adapter = new SimpleAdapter(this, data, R.layout.item_ecuinfolist, from, to);
		lView.setAdapter(adapter);
	}

	@Override
	protected void onDestroy() {
		Log.i(TAG, "onDestroy");
		Bundle bundle = new Bundle();
		bundle.putInt(GUI_REQ, V_REQUEST_EXIT_ECUINFO);
		sendDataToDiagnose(bundle);
		super.onDestroy();
	}
	private void sendDataToDiagnose(Bundle bundle) {
		Intent intent = new Intent(RECEIVER_SERVER);
		intent.putExtras(bundle);
		sendBroadcast(intent);
	}
	
	private ArrayList<HashMap<String, Object>> getData(GUIParam param)
	{
		int itemCounts = param.getCounts();
		ArrayList<HashMap<String, Object>> data = new ArrayList<HashMap<String,Object>>(itemCounts);
		String[] contents = param.getContents();
		if (itemCounts > 0)
		{
			
			for (int i = 0; i < itemCounts * 2; i += 2)
			{
				HashMap<String, Object> item = new HashMap<String, Object>();
				item.put(from[0], contents[i]);
				item.put(from[1], contents[i + 1]);
				data.add(item);
			}
		}
		return data;
	}
}
