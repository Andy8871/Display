package com.TD.Controller;

import android.content.Intent;
import android.os.Message;

public interface ControllerState {
	//解析来自诊断传上来的数据
	public boolean handleMessage(Message msg);
	//解析来自UI的请求
	public boolean receiveBroadcast(Intent intent);
	
	public String getCurrentState();
}
