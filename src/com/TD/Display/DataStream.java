package com.TD.Display;


import com.TD.Controller.GUIParam;

public class DataStream extends GUIParam{
	private int itemsInScreen;			/* 当前屏显示的数据流条目 */
	private int topItemId;				/* 当前屏显示的第一行数据流ID*/
	private int allItems;				/* 所有数据流条目*/
	
	/**
	 * 	Function:根据ListView显示区域高度及ListView中Item的高度计算当前屏可现实的数据流条目
	 *	Params:@param height		ListView显示区域高度
	 *	Params:@param itemHeight	Item的高度
	 *  Returns:void 
	 *  @author Andy  DateTime 2015-12-1 上午11:40:08
	 */
	public int getItemsInScreen(int height, int itemHeight)
	{
		/* 区域高度及item高度都必须大于0 */
		if (height <= 0 || itemHeight <= 0)
			return -1;
		/* 当区域高度小于item高度时当前屏可以显示的数据流条目为0 */
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
