package com.TD.Controller;

import java.io.Serializable;

import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

public class DatastreamState implements ControllerState, ControllerProtocol {

	private final static String TAG = DatastreamState.class.getSimpleName();
	private final Controller controller;
	private final static String STATE = RECEIVER_DS;
	private int topItems = 0;
	private int itemsInView = 0;

	public DatastreamState(Controller controller) {
		this.controller = controller;
	}

	@Override
	public boolean handleMessage(Message msg) {
		Log.i(TAG, "handleMessage");
		byte[] param = (byte[]) msg.obj;
		// 无效参数
		if (param.length < 10)
			return false;
		// 诊断传上来的内容为空
		int nLen = (((int) (param[0] & 0xFF) << 8) & 0xFF)
				+ (int) (param[1] & 0xFF);
		if (nLen < 0)
			return false;

		int nType = (int) ((param[2]) & 0xff);
		switch (nType) {
		case GUI_DATA_STREAM: {
			Log.i(TAG, "handleMessage GUI_DATA_STREAM");

			int offset = 6;
			GUIParam guiParam = new GUIParam();
			// 当前需要显示的菜单项数量
			int nCount = (((int) (param[offset] & 0xFF) << 8) & 0xFF) + (int) (param[offset + 1] & 0xFF);
			guiParam.setCounts(nCount);
			offset += 2;

			// 所有菜单项目数量
			int allItems = (((int) (param[offset] & 0xFF) << 8) & 0xFF) + (int) (param[offset + 1] & 0xFF);
			guiParam.setAllItems(allItems);
			offset += 2;

			// 数据流显示第一行
			int topItem = (((int) (param[offset] & 0xFF) << 8) & 0xFF) + (int) (param[offset + 1] & 0xFF);
			guiParam.setTopItem(topItem);
			offset += 2;

			// 诊断传递的数据内容：标题+菜单项内容
			byte[] temp = new byte[param.length - offset];
			System.arraycopy(param, offset + 1, temp, 0, temp.length - 1);
			String strContent = new String(temp);
			int start = 0;
			int end = start;
			int counts = 0;
			String[] content = new String[0];
			int capacity = 0;
			int pos = 0;
			while (end < temp.length) {
				if (0 == temp[end]) {
					String str = null;
					if (start != end) {
						str = strContent.substring(start, end);
					} else {
						str = "";
					}
					start = end + 1;
					if (0 == counts) {
						guiParam.setTitle(str);
					} else {
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
			notifyToUI(C_SHOW_DS, guiParam);
			/*Log.i(TAG, "send command to diagnose");
			byte[] cmd = new byte[16];
			cmd[0] = 0;
			cmd[1] = 5;
			cmd[4] = (byte) 0xFF;
			cmd[5] = (byte) ((topItems >> 8) & 0xFF); // 当前屏显示第一行ID（从0开始） 高字节
			cmd[6] = (byte) (topItems & 0xFF); // 当前屏显示第一行ID（从0开始） 低字节
			cmd[7] = (byte) ((itemsInView >> 8) & 0xFF); // 当前屏显示条目数量 高字节
			cmd[8] = (byte) (itemsInView & 0xFF); // 当前屏显示条目数量 低字节
			controller.resultToDiagnose(cmd);*/
			break;
		}
		default:
			break;
		}
		return true;
	}

	@Override
	public boolean receiveBroadcast(Intent intent) {
		Log.i(TAG, TAG + " accept a broadcast from UI");
		Bundle bundle = intent.getExtras();
		int nType = bundle.getInt(GUI_REQ, 0);
		switch (nType) {
		case V_REQUEST_EXIT_DS: {
			Log.i(TAG, "V_REQUEST_EXIT_DS");
			Log.i(TAG, "changeState to MenuState");
			byte[] cmd = new byte[16];
			cmd[0] = 0;
			cmd[1] = 5;
			cmd[4] = (byte) adsIDBACK;
			/*cmd[5] = (byte) ((topItems >> 8) & 0xFF); // 当前屏显示第一行ID（从0开始） 高字节
			cmd[6] = (byte) (topItems & 0xFF); // 当前屏显示第一行ID（从0开始） 低字节
			cmd[7] = (byte) ((itemsInView >> 8) & 0xFF); // 当前屏显示条目数量 高字节
			cmd[8] = (byte) (itemsInView & 0xFF); // 当前屏显示条目数量 低字节
*/			controller.resultToDiagnose(cmd);
			controller.changeState(new MultiSelectState(controller));
			break;
		}
		case V_REQUEST_SELECT_ITEM: {
			Log.i(TAG, "V_REQUEST_SELECT_ITEM");
			break;
		}
		case V_REQUEST_HAS_UPDATED_VIEW: {
			topItems = bundle.getInt("topItem");
			itemsInView = bundle.getInt("itemsInView");
			byte[] cmd = new byte[16];
			cmd[0] = 0;
			cmd[1] = 5;
			cmd[4] = (byte) 0xFF;
			cmd[5] = (byte) ((topItems >> 8) & 0xFF); // 当前屏显示第一行ID（从0开始） 高字节
			cmd[6] = (byte) (topItems & 0xFF); // 当前屏显示第一行ID（从0开始） 低字节
			cmd[7] = (byte) ((itemsInView >> 8) & 0xFF); // 当前屏显示条目数量 高字节
			cmd[8] = (byte) (itemsInView & 0xFF); // 当前屏显示条目数量 低字节
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

	private void notifyToUI(int type, Serializable value) {
		controller.notifyToUI(STATE, type, value);
	}

}
