package com.TD.Display;

import java.util.ArrayList;
import java.util.HashMap;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import com.TD.Controller.ControllerProtocol;
import com.TD.Controller.GUIParam;
import com.TD.View.widget.CheckBoxListAdapter.ViewHolder;
import com.TD.View.widget.CheckBoxListAdapter;
import com.example.display.R;

@SuppressLint("UseSparseArrays") 
public class MultiSelectActivity  extends Activity implements ActivityIndentify, ControllerProtocol{

	private final String TAG = MultiSelectActivity.class.getSimpleName();
	private TextView tvTitle = null;
	private ArrayList<String> infoList;
	private CheckBoxListAdapter adapter;
	//private HashMap<Integer, Boolean> isSelected = null;
	private ListView lv;
	//private ViewHolder vh;
	
	private byte[] selectItem;
	//private byte selected = 0;
	private Button btnOK = null;
	private Button btnCancel = null;
	private Button btnSelectall = null;
	private Button btnUnselectall = null;
	
	public ArrayList<String> getInfoList() {
		return infoList;
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		Log.i(TAG, "Show MultiSelectActivity");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_multiselect);
		tvTitle = (TextView)findViewById(R.id.path);
		btnOK = (Button)findViewById(R.id.OK);
		btnCancel = (Button)findViewById(R.id.Cancel);
		btnSelectall = (Button)findViewById(R.id.SelectAll);
		btnUnselectall = (Button)findViewById(R.id.UnselectAll);
		lv = (ListView)findViewById(R.id.lv);
		BtnClickListener btnClickListener = new BtnClickListener();
		btnOK.setOnClickListener(btnClickListener);
		btnCancel.setOnClickListener(btnClickListener);
		btnSelectall.setOnClickListener(btnClickListener);
		btnUnselectall.setOnClickListener(btnClickListener);
		// 获取版本信息并显示;
		Bundle args = getIntent().getExtras();
		GUIParam param = (GUIParam) args.getSerializable(CTL_GUI_PARAM);
		tvTitle.setText(param.getTitle());
		lv.setOnItemClickListener(new ItemClickListener());
		
		int counts = param.getCounts();
		if (counts > 0)
		infoList = getData(param);
		if (null != infoList)
		{
			adapter = new CheckBoxListAdapter(this, infoList, R.layout.item_checkboxlist, R.id.checkbox, R.id.item);
			lv.setAdapter(adapter);
			//isSelected = new HashMap<Integer, Boolean>(counts);
			selectItem = new byte[(counts / 8) + 1];
		}
	}

	@Override
	protected void onDestroy() {
		Log.i(TAG, "onDestroy");
		Bundle bundle = new Bundle();
		bundle.putInt(GUI_REQ, V_REQUEST_EXIT_MULTISELECT);
		sendDataToDiagnose(bundle);
		super.onDestroy();
	}
	
	private void sendDataToDiagnose(Bundle bundle) {
		Intent intent = new Intent(RECEIVER_SERVER);
		intent.putExtras(bundle);
		sendBroadcast(intent);
	}
	
	
	
	private class BtnClickListener implements OnClickListener{

		@Override
		public void onClick(View v) {
			Bundle bundle = new Bundle();
			switch (v.getId()) {
			case R.id.OK:
				int index = 0;
				for (int iPos = 0; iPos < selectItem.length; ++iPos)
				{
					for (int iBit = 0; iBit < 8; ++iBit)
					{
						byte b = selectItem[iPos];
						b = (byte)((b >> (7 - iBit)) & 0x01);
						if (0x01 == b)
							Log.i(TAG, "Select Item[" + index + "]");
						++index;
					}
				}
				bundle.putInt(GUI_BTN_ID, GUI_MSG_BTN_OK);
				bundle.putInt(GUI_REQ, V_REQUEST_SELECT_ITEM);
				bundle.putByteArray(GUI_SEL_ITEM, selectItem);
				sendDataToDiagnose(bundle);
				break;
			case R.id.Cancel:
				break;
			case R.id.SelectAll:
				selectAllItem();
				break;
			case R.id.UnselectAll:
				unselectAllItem();
				break;
			default:
				break;
			}
		}

	}
	
	private class ItemClickListener implements OnItemClickListener
	{

		@Override
		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {
			ViewHolder vh = (ViewHolder)view.getTag();
			vh.cb.toggle();
			
			if (vh.cb.isChecked())
			{
				adapter.checkItem(position);
				int index = position / 8;
				int pos = position % 8;
				int value = (1 << (7 - pos)) & 0xFF;
				selectItem[index] |= (byte)(value);
			}
			else
				adapter.uncheckItem(position);
		}
		
	}
	
	private void selectAllItem()
	{
		for (int position = 0; position < selectItem.length; ++position)
		{
			selectItem[position] = (byte)(0xFF);
		}
	}
	
	private void unselectAllItem()
	{
		for (int position = 0; position < selectItem.length; ++position)
		{
			selectItem[position/8] = 0;
		}
	}
	
	private ArrayList<String> getData(GUIParam param)
	{
		ArrayList<String> data = null;
		int counts = param.getCounts();
		if (counts > 0)
		{
			data = new ArrayList<String>();
			String[] arrData = param.getContents();
			for (int i = 0; i < counts - 1; ++i)
				data.add(arrData[i]);
		}
		return data;
	}
}
