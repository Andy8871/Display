package com.TD.View.widget;

import java.util.ArrayList;

import com.TD.Display.DTCActivity;
import com.TD.Display.MenuActivity;
import com.example.display.R;

import android.R.string;
import android.app.Activity;
import android.app.ListFragment;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;


 /**
 *  Class Name: DTCListFragment.java
 *  Function:
 *  
 *     Modifications:   
 *  
 *  @author Andy  DateTime 2015-10-26 ÏÂÎç7:48:21    
 *  @version 1.0
 */
public class DTCFragment extends ListFragment {
	private final String TAG = DTCFragment.class.getSimpleName();
	private ArrayList<String> dtcList = null;
	private DTCActivity dtcActivity = null;
	private ItemClikListener onItemClickClikListener = null;
	@Override
	public void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		Log.i(TAG, "crate list fragment");
		dtcActivity = (DTCActivity)getActivity();
		dtcList = dtcActivity.getDTCList();
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(getActivity(), R.layout.item_dtclist, R.id.dtcitem, dtcList);
		setListAdapter(adapter);
	}
	@Override
	public void onListItemClick(ListView l, View v, int position, long id) {
		// TODO Auto-generated method stub
		String strLog = "Select item[" + position + "]";
		Log.i(TAG, strLog);
		onItemClickClikListener.OnItemClickListener(position);
		super.onListItemClick(l, v, position, id);
	}
	
	
	
	@Override
	public void onAttach(Activity activity) {
		// TODO Auto-generated method stub
		super.onAttach(activity);
		Log.i(TAG, "onAttach");
		onItemClickClikListener = (ItemClikListener)activity;
	}



	public interface ItemClikListener
	{
		public void OnItemClickListener(int position);
	}
}
