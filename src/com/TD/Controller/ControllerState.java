package com.TD.Controller;

import android.content.Intent;
import android.os.Message;

public interface ControllerState {
	//����������ϴ�����������
	public boolean handleMessage(Message msg);
	//��������UI������
	public boolean receiveBroadcast(Intent intent);
	
	public String getCurrentState();
}
