package com.TD.Display;


import com.TD.Controller.GUIParam;

public class DataStream extends GUIParam{
	private int itemsInScreen;			/* ��ǰ����ʾ����������Ŀ */
	private int topItemId;				/* ��ǰ����ʾ�ĵ�һ��������ID*/
	private int allItems;				/* ������������Ŀ*/
	
	/**
	 * 	Function:����ListView��ʾ����߶ȼ�ListView��Item�ĸ߶ȼ��㵱ǰ������ʵ����������Ŀ
	 *	Params:@param height		ListView��ʾ����߶�
	 *	Params:@param itemHeight	Item�ĸ߶�
	 *  Returns:void 
	 *  @author Andy  DateTime 2015-12-1 ����11:40:08
	 */
	public int getItemsInScreen(int height, int itemHeight)
	{
		/* ����߶ȼ�item�߶ȶ��������0 */
		if (height <= 0 || itemHeight <= 0)
			return -1;
		/* ������߶�С��item�߶�ʱ��ǰ��������ʾ����������ĿΪ0 */
		if (height < itemHeight)
		{
			itemsInScreen = 0;
			return 0;
		}
		itemsInScreen = height / itemHeight;
		return itemsInScreen;
	}
	
	public int getTopItemId()
	{
		return topItemId;
	}
	
	public int getAllItems()
	{
		return allItems;
	}
}
