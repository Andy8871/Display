package com.TD.Controller;

import java.io.Serializable;
import java.util.StringTokenizer;

import com.TD.Display.ActivityIndentify;

import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

public class MenuState implements ControllerState, ControllerProtocol,
		ActivityIndentify {

	private final static String TAG = MenuState.class.getSimpleName();
	private final static String STATE = RECEIVER_MENU;
	private final Controller controller;

	public MenuState(Controller controller) {
		this.controller = controller;
	}

	@Override
	public boolean handleMessage(Message msg) {
		return parse((byte[]) msg.obj);
	}

	@Override
	public String getCurrentState() {
		// TODO Auto-generated method stub
		return STATE;
	}

	@Override
	public boolean receiveBroadcast(Intent intent) {
		Bundle extras = intent.getExtras();
		int nType = extras.getInt(GUI_REQ, 0);
		byte[] cmd = null;
		switch (nType) {
		case V_REQUEST_EXIT_DIAGNOSE:
			Log.i(TAG, "V_REQUEST_EXIT_DIAGNOSE");
			cmd = new byte[8];
			cmd[0] = 0;
			cmd[1] = 6;
			cmd[4] = -1;
			cmd[5] = -1;
			controller.resultToDiagnose(cmd);
			controller.changeState(new OriginalState(controller));
			controller.exitDiagnose();
			break;
		case V_REQUEST_EXIT_MENU: {
			Log.i(TAG, "V_REQUEST_EXIT_MENU");
			// Message msg = new Message();
			cmd = new byte[8];
			cmd[0] = 0;
			cmd[1] = 6;
			cmd[4] = -1;
			cmd[5] = -1;
			//controller.changeState(new OriginalState(controller));
			controller.resultToDiagnose(cmd);
			break;
		}
		case V_REQUEST_SELECT_ITEM: {
			
			int nSel = extras.getInt(GUI_SEL_ITEM);
			Log.i(TAG, "V_REQUEST_SELECT_ITEM item[" + nSel + "]");
			cmd = new byte[8];
			cmd[0] = 0;
			cmd[1] = 6;
			cmd[4] = (byte) ((nSel >> 8) & 0xFF);
			cmd[5] = (byte) (nSel & 0xFF);
			controller.resultToDiagnose(cmd);
			break;
		}

		default:
			break;
		}
		return false;
	}

	private boolean parse(byte[] param) {
		boolean bRes = false;

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
		case GUI_MENU: {
			bRes = parseMenu(param);
			break;
		}
		case GUI_MESSAGE_BOX: {
			bRes = parseMessageBox(param);
			break;
		}
		case GUI_ECU_INFORMATION:// 显示ECU信息
		{
			bRes = parseECUInfo(param);
			break;
		}
		case GUI_DATA_STREAM: {
			int nOffset = 4;
			GUIParam guiParam = new GUIParam();
			// 按钮标识
			int nItems = (((int) (param[nOffset] & 0xFF) << 8) & 0xFF)
					+ (int) (param[nOffset + 1] & 0xFF);
			nOffset += 2;
			guiParam.setCounts(nItems);
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
			 * arrContents[0] 标题文本 arrContents[1...n] 版本信息内容
			 */
			boolean bInitTitle = false;
			String title = null;
			while (tokenizer.hasMoreElements()) {
				if (!bInitTitle) {
					title = new String();
					title = tokenizer.nextToken();
					Log.i(TAG, "title" + title);
					bInitTitle = true;
				} else {
					arrContents[nPos] = new String();
					arrContents[nPos] = tokenizer.nextToken();
					Log.i(TAG, "content" + arrContents[nPos]);
					++nPos;
				}
			}
			guiParam.setTitle(title);
			guiParam.setContents(arrContents);
			controller.changeActivity(ACTIVITY_ECUINFO, guiParam);
			controller.changeState(new ECUInfoState(controller));
			break;
		}
		case GUI_MULTI_SELECT: {
			bRes = parseMultiSelect(param);
			break;
		}
		case GUI_DIAGNOSE_EXIT:
			Log.i(TAG, "GUI_DIAGNOSE_EXIT");
			notifyToUI(C_EXIT_DIAGNOSE, null);
			controller.exitDiagnose();
			break;
		default:
			break;
		}
		return bRes;
	}

	private boolean parseMenu(byte[] param) {
		// 解析菜单显示内容
		int nOffset = 8;
		GUIParam guiParam = new GUIParam();
		// 菜单项数量
		int nCount = (((int) (param[nOffset] & 0xFF) << 8) & 0xFF)
				+ (int) (param[nOffset + 1] & 0xFF);
		nOffset += 2;
		guiParam.setCounts(nCount);

		// 诊断传递的数据内容：标题+菜单项内容
		byte[] temp = new byte[param.length - nOffset];
		System.arraycopy(param, nOffset, temp, 0, param.length - nOffset);
		String strContent = new String(temp);

		// 字符串解析分隔符“0”
		byte[] delimiters = new byte[1];
		delimiters[0] = 0x00;
		String strDelimiters = new String(delimiters);
		StringTokenizer tokenizer = new StringTokenizer(strContent,
				strDelimiters);
		String[] arrContents = new String[tokenizer.countTokens() - 1];
		int nPos = 0;
		boolean bInitTitle = false;

		String strTitle = new String();
		while (tokenizer.hasMoreElements()) {
			// 获取title信息
			if (false == bInitTitle) {
				strTitle = tokenizer.nextToken();
				bInitTitle = true;
			} else {// 获取菜单显示内容
				arrContents[nPos] = new String();
				arrContents[nPos] = tokenizer.nextToken();
				Log.i(TAG, "Show item " + arrContents[nPos]);
				++nPos;
			}

		}
		guiParam.setTitle(strTitle);
		guiParam.setContents(arrContents);
		notifyToUI(C_SHOW_MENU, guiParam);
		return true;
	}

	private boolean parseMessageBox(byte[] param) {
		// 解析提示框显示内容
		int nOffset = 4;
		GUIParam guiParam = new GUIParam();
		// 按钮标识
		int nBtnFlag = (((int) (param[nOffset] & 0xFF) << 8) & 0xFF)
				+ (int) (param[nOffset + 1] & 0xFF);
		nOffset += 2;
		guiParam.setCounts(nBtnFlag);
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
		 * arrContents[0] 内容提示信息 arrContents[1] 标题文本 arrContents[2] 标题文本
		 */
		while (tokenizer.hasMoreElements()) {
			arrContents[nPos] = new String();
			arrContents[nPos] = tokenizer.nextToken();
			Log.i(TAG, arrContents[nPos]);
			++nPos;
		}
		guiParam.setContents(arrContents);
		notifyToUI(C_SHOW_MESSAGEBOX, guiParam);
		return true;
	}

	private boolean parseMultiSelect(byte[] param) {
		int nOffset = 4;
		GUIParam guiParam = new GUIParam();
		// 多项选择数量标识
		int nItems = (((int) (param[nOffset] & 0xFF) << 8) & 0xFF)
				+ (int) (param[nOffset + 1] & 0xFF);
		nOffset += 2;
		guiParam.setCounts(nItems);

		// ////////////////屏幕第一行序号 byte[6],byte[7]////////////////////////
		// 屏幕第一行序号暂时不处理
		nOffset += 2;
		// ///////////////////////////////////////////////////////////////

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

		boolean bInitTitle = false;
		String title = null;
		while (tokenizer.hasMoreElements()) {
			if (!bInitTitle) {
				title = new String();
				title = tokenizer.nextToken();
				Log.i(TAG, "title" + title);
				bInitTitle = true;
			} else {
				arrContents[nPos] = new String();
				arrContents[nPos] = tokenizer.nextToken();
				Log.i(TAG, "content-->" + arrContents[nPos]);
				++nPos;
			}
		}
		guiParam.setTitle(title);
		guiParam.setContents(arrContents);
		controller.changeActivity(ACTIVITY_MULTISELECT, guiParam);
		controller.changeState(new MultiSelectState(controller));
		return true;
	}

	private boolean parseECUInfo(byte[] param) {
		int nOffset = 4;
		GUIParam guiParam = new GUIParam();
		// 项目条数
		int nItems = (((int) (param[nOffset] & 0xFF) << 8) & 0xFF)
				+ (int) (param[nOffset + 1] & 0xFF);
		nOffset += 2;
		guiParam.setCounts(nItems);
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
		 * arrContents[0] 标题文本 arrContents[1...n] 版本信息内容
		 */
		boolean bInitTitle = false;
		String title = null;
		while (tokenizer.hasMoreElements()) {
			if (!bInitTitle) {
				title = new String();
				title = tokenizer.nextToken();
				Log.i(TAG, "title" + title);
				bInitTitle = true;
			} else {
				arrContents[nPos] = new String();
				arrContents[nPos] = tokenizer.nextToken();
				Log.i(TAG, "content-->" + arrContents[nPos]);
				++nPos;
			}
		}
		guiParam.setTitle(title);
		guiParam.setContents(arrContents);
		controller.changeActivity(ACTIVITY_ECUINFO, guiParam);
		controller.changeState(new ECUInfoState(controller));
		return true;
	}

	private void notifyToUI(int type, Serializable value) {
		controller.notifyToUI(STATE, type, value);
	}

}
