package com.TD.Controller;

import java.io.Serializable;


public class GUIParam implements Serializable{
	
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	private int GUIType;					//界面类型
	private int nCounts;					//显示条目数量,可以是按钮数量,可以是list条目数
	private int allItems;					//数据内容总量
	private int topItemId;					//界面显示第一项ID
	private String strTitle;				//界面标题栏
	private String[] GUIContents;			//显示内容
	
	
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
