package com.TD.Model;

import com.TD.Assist.CommData;

import android.R.integer;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import com.TD.Controller.ControllerProtocol;
/**
 * 
 *  Class Name: Diagnose.java
 *  Function: ����߳�,��Controller���б�����
 *  
 *  Modifications:   
 *  
 *  @author Andy  DateTime 2015-11-11 ����8:47:57    
 *  @version 1.0
 */
public class Diagnose extends Thread implements ControllerProtocol{
	
	private final static String TAG = Diagnose.class.getSimpleName();
	public final static String SOCKET_ADDRES = "com.TD.Model.Diagnose.LOCALSOCKET";
	//������Ϣ��UI��Handler����
	private static Handler outboxHandler;
	
	private static CommData data = null;
	public Diagnose()
	{
	
	}
	
	public Diagnose(Handler handler, CommData data)
	{
		outboxHandler = handler;
		setData(data);
	}
	
	public Diagnose(Handler handler, Runnable runnable)
	{
		outboxHandler = handler;
		//this.runnable = runnable;
	}
	
	@Override
	public void interrupt() {
		Log.i(TAG, "interrupt()");
		super.interrupt();
	}

	@Override
	public void run() {
		Log.i(TAG, "run()");
		RunDiagnose();
		super.run();
	}

	@Override
	public void start() {
		super.start();
	}
	
	

	//���ᱻJNI����
	//����Ҫ�ȴ�UI�������������ִ�����������
	public static boolean guiCallbackImmediate(byte[] param)
	{
		Log.i(TAG, "guiCallbackImmediate()");
		try {
			Message msg = outboxHandler.obtainMessage();
			msg.obj = param;
			msg.sendToTarget();
		}
		catch (NullPointerException e)
		{
			Log.e(TAG, "NullPointerException", e);
			return false;
		}
		Log.i(TAG, "return from guiCallbackImmediate()");
		return true;
	}
	
	//���ᱻJNI����
	//��Ҫ�ȴ�UI�Ľ�������ŷ���
	public static byte[] guiCallbackWaitForResponse(byte[] param)
	{
		
		byte[] cmd = null;
		try {
			String strData = new String();
			for (byte e : param)
			{
				strData += String.format("0x%02x ", e);
			}
			Log.i(TAG, "send data to UI:" + strData);
			Message msg = outboxHandler.obtainMessage();
			msg.obj = param;
			msg.sendToTarget();
		}
		catch (NullPointerException e)
		{
			Log.e(TAG, "NullPointerException", e);
		}
		Log.i(TAG, "wait for command from UI");
		cmd = recvData();
		String strCmd = new String();
		for (byte e:cmd)
		{
			strCmd += String.format("0x%02x ", e);
			//Log.i(TAG, "get data from UI--->" + e);
		}
		Log.i(TAG, "get command from UI: " + strCmd);
		return cmd;
	}
	
	private static byte[] recvData()
	{
		byte[] bRes = null;
		
		synchronized(getData())
		{
			try {
				getData().wait();
				
			} catch (InterruptedException e) {
				e.printStackTrace();
				getData().notify();
			}
		}
		bRes = getData().getData();
		return bRes;
	}
	

	public native void RunDiagnose();

	public static CommData getData() {
		return data;
	}

	public static void setData(CommData data) {
		Diagnose.data = data;
	}
}
