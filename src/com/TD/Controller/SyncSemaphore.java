package com.TD.Controller;

import java.util.concurrent.Semaphore;

import android.util.Log;

public class SyncSemaphore {
	private Semaphore semaphore = new Semaphore(1);
	
	/**
	*Brief
	*@title:initial
	*@descripetion:��ʼ���ź���
	*@params:@param bAvavilable acquire��ǰ�ź����Ƿ������������
	*@return:void
	*@throws:
	*@author:Andy Huang
	*@date:����12:21:05 2015
	*
	**/
	
	public void initial(boolean bAvavilable)
	{
		if(!bAvavilable)
			try {
				semaphore.acquire();
				} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	}
	
	public void waitForUI() throws InterruptedException
	{
		Log.i("SyncSemaphore", "������Դ-->>��ǰ������Դ��:" + semaphore.drainPermits());
		semaphore.acquire();
	}
	
	public void resultToDiagnose()
	{
		Log.i("SyncSemaphore", "�ͷ���Դ-->>��ǰ������Դ��:" + semaphore.drainPermits());
		semaphore.release();
	}
	
}
