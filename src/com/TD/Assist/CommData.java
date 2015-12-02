package com.TD.Assist;

public class CommData extends Object{
	private byte[] data = new byte[0];
	public void sendData(byte[] param)
	{
		data = null;
		data = param;
	}
	
	public byte[] getData()
	{
		return data;
	}
}
