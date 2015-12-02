package com.TD.View.widget;

import java.util.HashMap;
import java.util.List;

import android.R.integer;
import android.app.Activity;
import android.app.ListFragment;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.ListView;
import android.widget.SimpleAdapter;

import com.TD.Controller.Controller;
import com.TD.Controller.ControllerProtocol;

public class ReportListView  extends ListFragment implements ControllerProtocol{
	private List<HashMap<String, Object>> mData = null;
	private SimpleAdapter mAdapter = null;
	private String[] mFrom = null;
	private int[] mTo = null;
	private int mSourceID = -1;
	private Context mContext = null;
	private OnBackListener backListener;
	private OnClickListener clickListener;
	
	/*public ReportListView(List<HashMap<String, Object>> dataList)
	{
		data = dataList;
	}*/
	public ReportListView(Context context, List<HashMap<String, Object>> data, int source, String[]from, int[] to)
	{
		/*mData = data;
		mContext = context;
		mSourceID = source;
		mFrom = from;
		mTo = to;*/
		mAdapter = new SimpleAdapter(context, data, mSourceID, from, to);
	}
	@Override
	public void onAttach(Activity activity) {
		super.onAttach(activity);
	}
	
	/**
	 * 
	 */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		backListener = (OnBackListener) getActivity();
		clickListener = (OnClickListener)getActivity();
		/*if ((data != null) && (from != null) && (to != null) && (layoutID != -1))
		{
			adapter = new SimpleAdapter(getActivity(), data, layoutID, from, to);
			setListAdapter(adapter);
		}*/
		//Bundle args = getArguments();
		//data = (ArrayList<HashMap<String,Object>>)args.getSerializable(CTL_GUI_PARAM);
		super.onCreate(savedInstanceState);
	}
	@Override
	public void onDestroy() {
		backListener.isBack();
		super.onDestroy();
	}
	
	
	@Override
	public void onListItemClick(ListView l, View v, int position, long id) {
		clickListener.onClickItem(position);
		super.onListItemClick(l, v, position, id);
	}
	@Override
	public void onDetach() {
		super.onDetach();
	}
	@Override
	public void onResume() {
		super.onResume();
	}
	
	public void setFrom(String[] from)
	{
		mFrom = from;
	}
	
	public void setTo(int[] to)
	{
		mTo = to;
	}
	
	public void setLayoutID(int id)
	{
		mSourceID = id;
	}
	
	public void setData(List<HashMap<String, Object>> data)
	{
		mData = data;
	}
	
	public void updateView(List<HashMap<String, Object>> data)
	{
		data.clear();
		mData = data;
		mAdapter.notifyDataSetChanged();
	}
	
	public interface OnBackListener{
		public boolean isBack();
	}
	
	public interface OnClickListener{
		public boolean onClickItem(int position);
	}
	
}
