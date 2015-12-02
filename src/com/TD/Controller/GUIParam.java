package com.TD.Controller;

import java.io.Serializable;


public class GUIParam implements Serializable{
	
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	private int GUIType;					//��������
	private int nCounts;					//��ʾ��Ŀ����,�����ǰ�ť����,������list��Ŀ��
	private int allItems;					//������������
	private int topItemId;					//������ʾ��һ��ID
	private String strTitle;				//���������
	private String[] GUIContents;			//��ʾ����
	
	
	public void setType(int nType)
	{
		this.GUIType = nType;
	}
	
	public int getType()
	{
		return this.GUIType;
	}
	
	public void setTitle(String strTitle)
	{
		this.strTitle = strTitle;
	}
	
	public String getTitle()
	{
		return strTitle;
	}
	
	public void setCounts(int nCounts)
	{
		this.nCounts = nCounts;
	}
	
	public int getCounts()
	{
		return nCounts;
	}
	
	public void setContents(String[] contents)
	{
		this.GUIContents = contents;
	}
	
	public String[] getContents()
	{
		return this.GUIContents;
	}

	public void setTopItem(int topItem)
	{
		this.topItemId = topItem;
	}
	
	public int getTopItem()
	{
		return this.topItemId;
	}
	
	public void setAllItems(int allItems)
	{
		this.allItems = allItems;
	}
	
	public int getAllItems()
	{
		return this.allItems;
	}
	
	/*@Override
	public int describeContents() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public void writeToParcel(Parcel dest, int flags) {
		// TODO Auto-generated method stub
		
	}*/
}
