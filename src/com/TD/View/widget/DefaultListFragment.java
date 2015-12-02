package com.TD.View.widget;

import java.util.ArrayList;

import com.TD.Display.DefaultActivity;
import com.TD.Display.ECUInfoActivity;
import com.TD.Display.MainActivity;
import com.TD.Display.MenuActivity;
import com.example.display.R;

import android.app.Activity;
import android.app.ListFragment;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class DefaultListFragment extends ListFragment {

	private ArrayList<String> dataList = null;
	private DefaultActivity defaultActivity = null;
	private ItemClikListener onItemClickClikListener = null;
	@Override
	public void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		Log.i("ECUInfoListFragment", "crate list fragment");
		defaultActivity = (DefaultActivity)getActivity();
		dataList = defaultActivity.getDataList();
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(getActivity(), R.layout.item_defualtlist, R.id.defaultitem, dataList);
		setListAdapter(adapter);
	}
	@Override
	public void onListItemClick(ListView l, View v, int position, long id) {
		// TODO Auto-generated method stub
		String strLog = "Select item[" + position + "]";
		Log.i("DefaultListFragment", strLog);
		onItemClickClikListener.OnItemClickListener(position);
		super.onListItemClick(l, v, position, id);
	}
	
	
	
	@Override
	public void onAttach(Activity activity) {
		// TODO Auto-generated method stub
		super.onAttach(activity);
		Log.i("DefaultListFragment", "onAttach");
		onItemClickClikListener = (ItemClikListener)activity;
	}



	public interface ItemClikListener
	{
		public void OnItemClickListener(int position);
	}
	
	
	
}
