package com.TD.Assist;

public class ParseData {
	/**
	 * 
	 *  Class Name: ParseData.java
	 *  Function:原生方法解析诊断数据
	 *  
	 *  Modifications: ver 1.0 第一次创建   
	 *  
	 *  @author Andy  DateTime 2015-11-27 上午10:09:11    
	 *  @version 1.0
	 */
	public final class GUIParam{
		public String title;				//菜单显示title
		public int itemCounts;				//菜单项数量
		public int itemDefault;				//菜单项缺省值
		public int itemTop;					//当前屏显示的第一行
		public int keyFlag;						//按钮标识
	}
	
	public native static GUIParam parse(byte[] param);
}
