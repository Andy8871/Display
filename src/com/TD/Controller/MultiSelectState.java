package com.TD.Controller;

import com.TD.Display.ActivityIndentify;
import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

public class MultiSelectState implements ControllerState, ControllerProtocol,ActivityIndentify {

	private final static String TAG = MultiSelectState.class.getSimpleName();
	private final static String STATE = RECEIVER_ECUINFO;
	private final Controller controller;

	public MultiSelectState(Controller controller) {
		this.controller = controller;
	}

	@Override
	public boolean handleMessage(Message msg) {
		Log.i(TAG, "Get data from diagnose");
		byte[] param = (byte[]) msg.obj;
		// ��Ч����
		if (param.length < 10)
			return false;
		// ��ϴ�����������Ϊ��
		int nLen = (((int) (param[0] & 0xFF) << 8) & 0xFF)
				+ (int) (param[1] & 0xFF);
		if (nLen < 0)
			return false;

		int nType = (int) ((param[2]) & 0xff);
		switch (nType) {
		case GUI_MULTI_SELECT: {
			break;
		}
		case GUI_DATA_STREAM:
			Log.i(TAG, "Change activity to data stream");
			int offset = 6;
			byte[] data = new byte[param.length - 12];
			System.arraycopy(param, offset, data, 0, data.length);
			
			GUIParam guiParam = new GUIParam();
			/* ��ǰ��Ҫ��ʾ�Ĳ˵������� */
			int nCount = (((int) (param[offset] & 0xFF) << 8) & 0xFF)
					+ (int) (param[offset + 1] & 0xFF);
			offset += 2;
			guiParam.setCounts(nCount);
			
			/* ���в˵���Ŀ���� */
			int allItems = (((int) (param[offset] & 0xFF) << 8) & 0xFF)
					+ (int) (param[offset + 1] & 0xFF);
			offset += 2;
			
			/* ��������ʾ��һ�� */
			int topItem = (((int) (param[offset] & 0xFF) << 8) & 0xFF)
					+ (int) (param[offset + 1] & 0xFF);
			offset += 2;
			
			/* ��ϴ��ݵ��������ݣ�����+�˵������� */
			byte[] temp = new byte[param.length - offset];
			System.arraycopy(param, offset + 1, temp, 0, temp.length - 1);
			String strContent = new String(temp);
			int start = 0;
			int end = start;
			int counts = 0;
			String[] content = new String[0];
			int capacity = 0;
			int pos = 0;
			while(end < temp.length)
			{
				if (0 == temp[end])
				{
					String str = null;
					if (start != end)
					{
						str = strContent.substring(start, end);
					}
					else {
						str = "";
					}
					start = end + 1;
					if (0 == counts)
					{
						guiParam.setTitle(str);
					}
					else {
						capacity += 1;
						String[] strTemp = new String[capacity];
						System.arraycopy(content, 0, strTemp, 0, content.length);
						strTemp[pos] = str;
						content = strTemp;
						pos++;
					}
					++counts;
				}
				end++;
			}
			guiParam.setContents(content);
			controller.changeActivity(ACTIVITY_DATASTREAM, guiParam);
			controller.changeState(new DatastreamState(controller));
			/*byte[] cmd = new byte[16];
			cmd[0] = 0;
			cmd[1] = 5;
			cmd[4] = (byte)0xFF;
			cmd[5] = 0;									 ��ǰ����ʾ��һ��ID����0��ʼ�������ֽ� 
			cmd[6] = 0;									 ��ǰ����ʾ��һ��ID����0��ʼ�������ֽ� 
			cmd[7] = (byte)((19 >> 8) & 0xFF);			 ��ǰ����ʾ��Ŀ���������ֽ� 
			cmd[8] = (byte)(19 & 0xFF);					 ��ǰ����ʾ��Ŀ���������ֽ� 
			controller.resultToDiagnose(cmd);*/
			break;
		default:
			break;
		}
		return false;
	}

	@Override
	public boolean receiveBroadcast(Intent intent) {
		Log.i(TAG, TAG + " accept a broadcast from UI");
		Bundle extras = intent.getExtras();
		int nType = extras.getInt(GUI_REQ, 0);
		switch (nType) {
		case V_REQUEST_EXIT_MULTISELECT: {
			byte[] cmd = new byte[8];
			cmd[0] = 0;
			cmd[1] = 6;
			cmd[4] = -1;
			cmd[5] = -1;
			controller.changeState(new MenuState(controller));
			controller.resultToDiagnose(cmd);
			break;
		}
		case V_REQUEST_SELECT_ITEM: {
			Log.i(TAG, "V_REQUEST_SELECT_ITEM");
			byte[] data = extras.getByteArray(GUI_SEL_ITEM);
			byte btnID = (byte)(extras.getInt(GUI_BTN_ID) & 0xFF);
			int nLen = data.length;
			byte[] cmd = new byte[nLen + 7];
			cmd[0] = (byte) ((nLen >> 8) & 0xFF);
			cmd[1] = (byte) (nLen & 0xFF);
			cmd[4] = btnID;
			System.arraycopy(data, 0, cmd, 7, data.length);
			controller.resultToDiagnose(cmd);
			break;
		}
		default:
			break;
		}
		return false;
	}

	@Override
	public String getCurrentState() {
		return STATE;
	}
}
