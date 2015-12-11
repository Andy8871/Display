package com.TD.Controller;

import java.io.Serializable;
import java.util.StringTokenizer;

import com.TD.Assist.CommData;
import com.TD.Assist.MessageBox;
import com.TD.Display.ActivityIndentify;
import com.TD.Model.Diagnose;

import android.app.AlertDialog;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Binder;
import android.os.Bundle;
import android.os.Handler;
import android.os.Handler.Callback;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

public class Controller extends Service implements ControllerProtocol,
		ActivityIndentify {
	static {
		System.loadLibrary("OBDII");
	}

	private final static String TAG = Controller.class.getSimpleName();
	
	private ControllerState controllerState;

	private Diagnose diagnose = null;
	// 给诊断发送消息
	private Handler outboxHander = null;
	// 接收诊断发送的消息
	private Handler inboxHandler = null;
	
	private MessageBox msgBox = null;
	
	private CommData data = new CommData();
	
	private dialogListener dlgListener = new dialogListener() {
		
		@Override
		public int callback(int btn) {
			int btnID = BTN_OTHER;
			if (AlertDialog.BUTTON_POSITIVE == btn)
				btnID = BTN_POSITIVE;
			else if (AlertDialog.BUTTON_NEGATIVE == btn)
				btnID= BTN_NIGATIVE;
			return btnID;
		}
	};
	
	public class ControllerBinder extends Binder{
		public Controller getController()
		{
			return Controller.this;
		}
	}
	
	private final IBinder mBinder = new ControllerBinder();
	@Override
	public IBinder onBind(Intent intent) {
		return mBinder;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {

		Log.i(TAG, "onStartCommand");
		// 初始化工作
		// 设置控制状态为初始状态
		controllerState = new OriginalState(this);

		// 注册接收UI控制指令的广播消息接收器
		registBroadcastReceiver();

		inboxHandler = new Handler(inboxHandlerMsg);

		diagnose = new Diagnose(inboxHandler, data);
		
		msgBox = new MessageBox(dlgListener);
		startDiagnose();
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		Log.i(TAG, "onDestroy");
		unregisterReceiver(diagReceiver);
		
		//while (inboxHandler.hasMessages(TRIM_MEMORY_RUNNING_CRITICAL))
		// sm = null;
		super.onDestroy();
	}

	@Override
	public boolean onUnbind(Intent intent) {
		Log.i(TAG, "onUnbind");
		return super.onUnbind(intent);
	}

	// 在ControllerSate中被调用
	// 启动诊断
	public void startDiagnose() {
		diagnose.start();
	}

	// 在ControllerSate中被调用
	// 退出诊断
	public void exitDiagnose() {
		// freeDll("OBDII");
		// diagnose.stop();
		stopSelf();
	}

	// 在ControllerSate中被调用
	// 切换当前到当前的UI状态
	public final void changeState(ControllerState controllerState) {
		this.controllerState = controllerState;
	}

	// 在ControllerSate中被调用
	// 更新UI状态
	public final void notifyToUI(Intent intent) {
		if (intent != null) {
			sendBroadcast(intent);
		}
	}
	
	public final void clearMessageHandler()
	{
		if (inboxHandler.hasMessages(0xFF))
		{
			Log.i(TAG, "need to clear the message");
			inboxHandler.removeMessages(0xFF);
		}
	}
	
	public final void notifyToUI(String action, int type, Serializable value)
	{
		Intent intent = new Intent(action);
		Bundle bundle = new Bundle();
		bundle.putInt(CTL_ANS, type);
		bundle.putSerializable(CTL_GUI_PARAM, value);
		// bundle.putStringArray("MENULIST", arrContents);
		intent.putExtras(bundle);
		notifyToUI(intent);
	}

	// 在ControllerState中被调用
	// 切换UI界面
	public final void changeActivity(String action, Object obj) {
		Intent intent = new Intent(action);
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		intent.setAction(action);
		Bundle bundler = new Bundle();
		bundler.putSerializable(CTL_GUI_PARAM, (GUIParam) obj);
		intent.putExtras(bundler);
		startActivity(intent);
	}

	// 在ControllerSate中被调用
	// 发送UI的控制命令到诊断
	public final void sendCommandToModel(Message msg) {
		outboxHander.sendMessage(msg);
	}

	// 在ControllerSate中被调用
	// 向诊断模块发送请求数据
	public final void resultToDiagnose(byte[] content) {
		synchronized (data) {
			data.sendData(content);
			data.notify();
		}
	}

	// 处理来自于诊断的请求消息
	private Callback inboxHandlerMsg = new Callback() {

		@Override
		public boolean handleMessage(Message msg) {
			boolean bRes = false;
			byte[] param = (byte[])msg.obj;
			//无效参数
			if (param.length < 10)
				return false;
			//诊断传上来的内容为空
			int nLen = (((int)(param[0] & 0xFF) << 8) & 0xFF) + (int)(param[1] & 0xFF);
			if (nLen < 0)
				return false;
			
			int nType = (int) ((param[2]) & 0xff);
			/**
			 * 弹出消息对话框统一处理
			 * 在诊断服务中直接弹出消息对话框
			 * 避免各个界面处理对话框消息
			 */
			if (GUI_MESSAGE_BOX == nType)
			{
				bRes = parse(param);
			}
			else
			{
				msgBox.Cancel();
				bRes = controllerState.handleMessage(msg);
			}
			
			return bRes;
		}
	};

	// 注册广播消息接收器
	private void registBroadcastReceiver() {
		Log.i(TAG, "registBroadcastReceiver");
		IntentFilter iFilter = new IntentFilter();
		iFilter.addAction(RECEIVER_SERVER);
		registerReceiver(diagReceiver, iFilter);
	}

	// 诊断服务广播接收器
	// 接收来自于UI的操作请求广播消息
	private BroadcastReceiver diagReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			Log.i(TAG, "Accept message from UI: ");
			controllerState.receiveBroadcast(intent);
		}

	};
	
	private boolean parse(byte[] param)
	{

		// 解析提示框显示内容
		int nOffset = 4;

		// 按钮标识
	/*	int nBtnFlag = (((int) (param[nOffset] & 0xFF) << 8) & 0xFF)
				+ (int) (param[nOffset + 1] & 0xFF);*/
		nOffset += 2;
		
		// 提示框数据内容：标题+菜单项内容
		byte[] temp = new byte[param.length - nOffset];
		System.arraycopy(param, nOffset, temp, 0, param.length - nOffset);
		String strContent = new String(temp);

		// 字符串解析分隔符“0”
		byte[] delimiters = new byte[1];
		delimiters[0] = 0x00;
		String strDelimiters = new String(delimiters);
		StringTokenizer tokenizer = new StringTokenizer(strContent,
				strDelimiters);
		String[] arrContents = new String[tokenizer.countTokens()];
		int nPos = 0;
		/**
		 * arrContents[0] 内容提示信息
		 * arrContents[1] 标题文本
		 * arrContents[2] 标题文本
		 */
		while (tokenizer.hasMoreElements()) {
			arrContents[nPos] = new String();
			arrContents[nPos] = tokenizer.nextToken();
			Log.i(TAG, arrContents[nPos]);
			++nPos;
		}
		msgBox.Show(this, arrContents[1], arrContents[0]);
		byte[] cmd = new byte[8];
		cmd[0] = 0;
		cmd[1] = 6;
		resultToDiagnose(cmd);
		
		return true;
	}
	
	public interface dialogListener{
		public final int BTN_OTHER = 0;
		public final int BTN_NIGATIVE = 1;
		public final int BTN_POSITIVE = 2;
		public int callback(int btn);
	}
	
	
}