package com.TD.Assist;

public class ParseData {
	/**
	 * 
	 *  Class Name: ParseData.java
	 *  Function:ԭ�����������������
	 *  
	 *  Modifications: ver 1.0 ��һ�δ���   
	 *  
	 *  @author Andy  DateTime 2015-11-27 ����10:09:11    
	 *  @version 1.0
	 */
	public final class GUIParam{
		public String title;				//�˵���ʾtitle
		public int itemCounts;				//�˵�������
		public int itemDefault;				//�˵���ȱʡֵ
		public int itemTop;					//��ǰ����ʾ�ĵ�һ��
		public int keyFlag;						//��ť��ʶ
	}
	
	public native static GUIParam parse(byte[] param);
}
