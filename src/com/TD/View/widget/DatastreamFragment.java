package com.TD.View.widget;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.TD.Controller.ControllerProtocol;
import com.TD.Controller.GUIParam;
import com.example.display.R;
import android.app.Activity;
import android.app.ListFragment;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ListView;
import android.widget.SimpleAdapter;


 /**
 *  Class Name: DTCListFragment.java
 *  Function:
 *  
 *     Modifications:   
 *  
 *  @author Andy  DateTime 2015-10-26 ÏÂÎç7:48:21    
 *  @version 1.0
 */
public class DatastreamFragment extends ListFragment implements ControllerProtocol{
	private final String TAG = DatastreamFragment.class.getSimpleName();
	private List<HashMap<String, Object>> data = null;
	private ItemClikListener onItemClickClikListener = null;
	private String[] from = new String[]{"item", "value", "unit"};
	private int[] to = new int[]{R.id.dsitem, R.id.dsvalue, R.id.dsunit};
	private SimpleAdapter adapter = null;
	@Override
	public void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		Bundle args = getArguments();
		GUIParam param = (GUIParam)args.getSerializable(CTL_GUI_PARAM);
		int itemCounts = param.getCounts();
		String[] contents = param.getContents();
		data = new ArrayList<HashMap<String,Object>>(itemCounts);
		if (itemCounts > 0 && (itemCounts * 3) <= contents.length)
		{
			for (int i = 0; i < itemCounts * 3; i += 3)
			{
				HashMap<String, Object> item = new HashMap<String, Object>();
				item.put(from[0], contents[i]);
				item.put(from[1], contents[i + 1]);
				item.put(from[2], contents[i + 2]);
				data.add(item);
			}
		}
		adapter = new SimpleAdapter(getActivity(), data, R.layout.item_dslist, from, to);
		setListAdapter(adapter);
	}
	@Override
	public void onListItemClick(ListView l, View v, int position, long id) {
		// TODO Auto-generated method stub
		String strLog = "Select item[" + position + "]";
		Log.i(TAG, strLog);
		onItemClickClikListener.OnItemClickListener(position);
		super.onListItemClick(l, v, position, id);
	}
	
	
	
	@Override
	public void onAttach(Activity activity) {
		// TODO Auto-generated method stub
		super.onAttach(activity);
		Log.i(TAG, "onAttach");
		onItemClickClikListener = (ItemClikListener)activity;
	}

	public void updateView(GUIParam param/*ArrayList<HashMap<String, Object>> update*/)
	{
		Log.i(TAG, "updateView");
		data.clear();
		//data = update;
		
		int itemCounts = param.getCounts();
		String[] contents = param.getContents();
		if (itemCounts > 0 && (itemCounts * 3) <= contents.length)
		{
			for (int i = 0; i < itemCounts * 3; i += 3)
			{
				HashMap<String, Object> item = new HashMap<String, Object>();
				item.put(from[0], contents[i]);
				item.put(from[1], contents[i + 1]);
				item.put(from[2], contents[i + 2]);
				Log.i(TAG, "updateView>>>>>" + contents[i] + " " + contents[i + 1] + " " + contents[i + 2]);
				data.add(item);
			}
		}
		adapter.notifyDataSetChanged();
		/*for (int i = 0; i < update.size(); ++i)
		{
			data.add(update.get(i));
		}*/
	}


	public interface ItemClikListener
	{
		public void OnItemClickListener(int position);
	}
}
