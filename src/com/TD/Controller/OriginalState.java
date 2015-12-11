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
	 * ����������UI����������
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
	 *  Function:������������ϵ�����
	 *	Params:
	 *  Returns:boolean 
	 *  @author Andy  DateTime 2015-11-11 ����9:00:24
	 *  @param param
	 *  @return
	 */
	@SuppressLint("SdCardPath") private boolean parse(byte[] param) {
		boolean bRes = false;
		
		//��Ч����
		if (param.length < 10)
			return false;
		//��ϴ�����������Ϊ��
		int nLen = (((int)(param[0] & 0xFF) << 8) & 0xFF) + (int)(param[1] & 0xFF);
		if (nLen < 0)
			return false;
		
		int nType = (int) ((param[2]) & 0xff);
		switch (nType) {
		case GUI_DIAGNOSE_RUN_SUCCESS:
		{
			/**
			 * ��Ҫ��Ӵ������������ģ�������ɹ��Ժ��������Ϣ
			 */
			byte[] cmd = null;
			byte[] temp = new byte[0xFFFF];
			int nOffset = 0;
			/*��ϳ������й���Ŀ¼*/
			String strDiagnosePath = "/sdcard/Vpecker/scan/OBDII/OBDII/EN";
			System.arraycopy(strDiagnosePath.getBytes(), 0, temp, 0, strDiagnosePath.length());
			nOffset = strDiagnosePath.length();
			temp[nOffset] = 0;
			nOffset += 1;
			
			/* ��ǰϵͳ���� */
			String strLanguage = "EN";
			System.arraycopy(strLanguage.getBytes(), 0, temp, nOffset, strLanguage.length());
			nOffset += strLanguage.length();
			temp[nOffset] = 0;
			nOffset += 1;
			
			/* ����ģʽ��1 demo�� 0 run */
			byte bDemo = 0;
			temp[nOffset] = bDemo;
			nOffset += 1;
			
			cmd = new byte[nOffset];
			System.arraycopy(temp, 0, cmd, 0, nOffset);
			controller.resultToDiagnose(cmd);
			break;
		}
		case GUI_MENU:
		{
			//�����˵���ʾ����
			int nOffset = 9;
			GUIParam guiParam = new GUIParam();
			
			//�˵�������
			int nCount = (((int)(param[nOffset] & 0xFF) << 8) & 0xFF) + (int)(param[nOffset + 1] & 0xFF);
			nOffset += 2;
			guiParam.setCounts(nCount);
			byte[] temp = new byte[param.length - nOffset];
			System.arraycopy(param, nOffset, temp, 0, param.length - nOffset);
			String strContent = new String(temp);
			//�ַ��������ָ�����0��
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
				//��ȡtitle��Ϣ
				if (false == bInitTitle)
				{
					strTitle = tokenizer.nextToken();
					bInitTitle = true;
				}
				else {
					//��ȡ�˵���ʾ����
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
