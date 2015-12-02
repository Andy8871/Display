/**
 * 
 */
package com.TD.Controller;

import java.util.StringTokenizer;

import com.TD.Display.ActivityIndentify;
import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

/**
 * @author andy
 * 
 */
public class OriginalState implements ControllerState, ControllerProtocol, ActivityIndentify {
	private final static String TAG = OriginalState.class.getSimpleName();
	private final Controller controller;
	private final static String STATE = RECEIVER_DEFAULT;

	public OriginalState(Controller ctler) {
		this.controller = ctler;
	}

	@Override
	public boolean handleMessage(Message msg) {
		return parse((byte[]) msg.obj);
	}

	/**
	 * 处理来自于UI的请求命令
	 */
	@Override
	public boolean receiveBroadcast(Intent intent) {
		Bundle bundle = intent.getExtras();
		int nType = bundle.getInt(GUI_REQ, 0);
		switch (nType) {
		case V_REQUEST_START_DIAGNOSE: {
			//controller.startDiagnose();
			break;
		}
		case V_REQUEST_EXIT_DIAGNOSE: {
			break;
		}
		}
		return false;
	}

	/**
	 * 
	 *  Function:解析来自于诊断的数据
	 *	Params:
	 *  Returns:boolean 
	 *  @author Andy  DateTime 2015-11-11 下午9:00:24
	 *  @param param
	 *  @return
	 */
	@SuppressLint("SdCardPath") private boolean parse(byte[] param) {
		boolean bRes = false;
		
		//无效参数
		if (param.length < 10)
			return false;
		//诊断传上来的内容为空
		int nLen = (((int)(param[0] & 0xFF) << 8) & 0xFF) + (int)(param[1] & 0xFF);
		if (nLen < 0)
			return false;
		
		int nType = (int) ((param[2]) & 0xff);
		switch (nType) {
		case GUI_DIAGNOSE_RUN_SUCCESS:
		{
			/**
			 * 需要添加代码来处理诊断模块启动成功以后的请求信息
			 */
			String strDiagnosePath = "/sdcard/Vpecker/scan/OBDII/OBDII/EN";
			String strLanguage = "EN";
			int nCmdLen = strDiagnosePath.length() + strLanguage.length() + 1;
			byte[] cmd = new byte[nCmdLen];
			Log.i(TAG, "cmd byte len = " + cmd.length);
			System.arraycopy(strDiagnosePath.getBytes(), 0, cmd, 0, strDiagnosePath.length());
			cmd[strDiagnosePath.length()] = 0;
			
			System.arraycopy(strLanguage.getBytes(), 0, cmd, strDiagnosePath.length() + 1, strLanguage.length());
			//cmd += strLanguage.getBytes();
			controller.resultToDiagnose(cmd);
			break;
		}
		case GUI_MENU:
		{
			//解析菜单显示内容
			int nOffset = 9;
			GUIParam guiParam = new GUIParam();
			
			//菜单项数量
			int nCount = (((int)(param[nOffset] & 0xFF) << 8) & 0xFF) + (int)(param[nOffset + 1] & 0xFF);
			nOffset += 2;
			guiParam.setCounts(nCount);
			byte[] temp = new byte[param.length - nOffset];
			System.arraycopy(param, nOffset, temp, 0, param.length - nOffset);
			String strContent = new String(temp);
			//字符串解析分隔符“0”
			byte[] delimiters = new byte[1];
			delimiters[0] = 0x00;
			String strDelimiter = new String(delimiters);
			StringTokenizer tokenizer = new StringTokenizer(strContent, strDelimiter);
			
			String[] arrContents = new String[tokenizer.countTokens() - 1];
			int nPos = 0;
			boolean bInitTitle = false;
			String strTitle = new String();
			while (tokenizer.hasMoreElements())
			{
				//获取title信息
				if (false == bInitTitle)
				{
					strTitle = tokenizer.nextToken();
					bInitTitle = true;
				}
				else {
					//获取菜单显示内容
					arrContents[nPos] = new String();
					arrContents[nPos] = tokenizer.nextToken();
					Log.i(TAG, arrContents[nPos]);
					++nPos;
				}	
			}
			guiParam.setTitle(strTitle);
			guiParam.setContents(arrContents);
			controller.changeActivity(ACTIVITY_MENU, guiParam);
			controller.changeState(new MenuState(controller));
			break;
		}
		default:
			break;
		}
		
		
		return bRes;
	}

	@Override
	public String getCurrentState() {
		return STATE;
	}

}
