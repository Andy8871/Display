package com.TD.Controller;

import java.util.concurrent.Semaphore;

import android.util.Log;

public class SyncSemaphore {
	private Semaphore semaphore = new Semaphore(1);
	
	/**
	*Brief
	*@title:initial
	*@descripetion:初始化信号量
	*@params:@param bAvavilable acquire当前信号量是否可以立即返回
	*@return:void
	*@throws:
	*@author:Andy Huang
	*@date:下午12:21:05 2015
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
		Log.i("SyncSemaphore", "请求资源-->>当前可用资源数:" + semaphore.drainPermits());
		semaphore.acquire();
	}
	
	public void resultToDiagnose()
	{
		Log.i("SyncSemaphore", "释放资源-->>当前可用资源数:" + semaphore.drainPermits());
		semaphore.release();
	}
	
}
