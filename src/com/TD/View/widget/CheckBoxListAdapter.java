package com.TD.View.widget;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.example.display.R;

import android.R.integer;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.TextView;

public class CheckBoxListAdapter extends BaseAdapter{
	private static final String TAG = CheckBoxListAdapter.class.getSimpleName();
	
	private ArrayList<String> mData = null;
	private HashMap<Integer, Boolean> mIsSelected = null;
	private LayoutInflater mInflater = null;
	private int mCheckBoxId = -1;
	private int mItemId = -1;
	private int mSourceId = -1;
	public CheckBoxListAdapter(Context context, List<String> data, int source, int checkBoxId, int itemId)
	{
		mData = (ArrayList<String>)data;
		//mContext = context;
		mInflater = LayoutInflater.from(context);
		mIsSelected = new HashMap<Integer, Boolean>();
		mSourceId = source;
		mCheckBoxId = checkBoxId;
		mItemId = itemId;
		initSelected();
	}
	
	//CheckBox ��ʼ״̬��ʼ��Ϊδѡ��
	private void initSelected()
	{
		for (int i = 0; i < mData.size(); ++i)
		{
			mIsSelected.put(i, false);
		}
	}
	@Override
	public int getCount() {
		return mData.size();
	}

	@Override
	public Object getItem(int position) {
		return mData.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		try {
			ViewHolder vh = null;
			if (null == convertView){
				//���벼���ļ������Ƹ�converView
				convertView = mInflater.inflate(mSourceId, null);
				vh = new ViewHolder();
				vh.cb = (CheckBox)convertView.findViewById(mCheckBoxId);
				vh.tv = (TextView)convertView.findViewById(mItemId);
				//��Itemѡ��浽converView��
				//�����б���½���ʱ������Ҫ�����µ�Item�����Ƶ�������
				//����ListView��Ч��
				convertView.setTag(vh);
			}
			else {
				//ȡ��֮ǰ�������ЧItem��
				vh = (ViewHolder)convertView.getTag();
			}
			//����Item��ֵ
			vh.tv.setText(mData.get(position).toString());
			//����IsSelected������CheckBox��״̬
			vh.cb.setChecked(mIsSelected.get(position));
			//mVHList.add(vh);
		} catch (NullPointerException e) {
			Log.i(TAG, e.getMessage());
		}
		
		return convertView;
	}
	/*
	//���ѡ����
	public void clickItem(int position)
	{
		Log.i(TAG, "Click item [" + position + "],the state about the item has been selected--->" + mVHList.get(position).cb.isChecked());
		mVHList.get(position).cb.toggle();
		mIsSelected.put(position, mVHList.get(position).cb.isChecked());
	}*/
	public void setCheck(int position)
	{
		
	}
	
	//ѡ��ָ����
	public void checkItem(int position)
	{
		Log.i(TAG, "check item [" + position + "]");
		mIsSelected.put(position, true);
	}
	
	//��ѡָ����
	public void uncheckItem(int position)
	{
		Log.i(TAG, "uncheck item [" + position + "]");
		mIsSelected.put(position, false);
	}
	
	//�ж�ĳһ���Ƿ�ѡ��
	public boolean isChecked(int position)
	{
		return mIsSelected.get(position);
	}
	/*public static HashMap<Integer, Boolean> getIsSelected()
	{
		return mIsSelected;
	}
	
	public void setIsSelected(HashMap<Integer, Boolean> selected)
	{
		mIsSelected = selected;
	}*/
	
	public final class ViewHolder{
		public TextView tv = null;
		public CheckBox cb = null;;
	}
}
