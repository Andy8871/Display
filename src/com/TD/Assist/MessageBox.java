package com.TD.Assist;

import com.TD.Controller.Controller.dialogListener;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.view.WindowManager;

public class MessageBox {
	private AlertDialog diag = null;
	private dialogListener dlgListener = null;
	private OnClickListener btnClickListener = new OnClickListener() {
		
		@Override
		public void onClick(DialogInterface dialog, int which) {
			dlgListener.callback(which);
		}
	};
	public MessageBox(dialogListener listener)
	{
		this.dlgListener = listener;
	}

	/**
	 * Function:Show the messagebox without buttons 
	 * Params:@param context The context about current application 
	 * Params:@param text The message contents
	 * that user want to show Returns:void
	 * 
	 * @author Andy DateTime 2015-11-18 ÉÏÎç10:47:44
	 */
	public void Show(Context context, String title, String message) {
		if (null == diag) {
			AlertDialog.Builder builder = new AlertDialog.Builder(context);
			builder.setTitle(title);
			builder.setMessage(message);
			diag = builder.create();
			diag.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
		} else {
			diag.setTitle(title);
			diag.setMessage(message);
		}
		diag.show();
	}

	/**
	 * 	Function:TODO
	 *	Params:@param context
	 *	Params:@param title
	 *	Params:@param message
	 *	Params:@param nigativeBtn
	 *	Params:@param positiveBtn
	 *  Returns:void 
	 *  @author Andy  DateTime 2015-11-18 ÏÂÎç5:59:46
	 */
	public void Show(Context context, String title, String message,
			String nigativeBtn, String positiveBtn) {
		if (null == diag) {
			AlertDialog.Builder builder = new AlertDialog.Builder(context);
			builder.setTitle(title);
			builder.setMessage(message);
			builder.setNegativeButton(nigativeBtn, btnClickListener);
			diag = builder.create();
			diag.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
		} else {
			diag.setTitle(title);
			diag.setMessage(message);
		}
		diag.show();
	}
	
	public void Cancel()
	{
		if (null != diag)
		{
			diag.dismiss();
			diag = null;
		}
	}
}
