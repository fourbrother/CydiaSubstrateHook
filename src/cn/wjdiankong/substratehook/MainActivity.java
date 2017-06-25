package cn.wjdiankong.substratehook;

import android.app.Activity;
import android.os.Bundle;
import android.telephony.TelephonyManager;
import android.util.Log;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		Log.i("jw", "get imei:"+getIMEI());
	}

	private String getIMEI(){
		TelephonyManager tm = (TelephonyManager) this.getSystemService(TELEPHONY_SERVICE);     
		return tm.getDeviceId();
	}
	
}
