package com.example.sensdataudp;

import android.Manifest;
import android.hardware.Sensor;
import android.net.wifi.WifiManager;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.util.Log;
import android.hardware.SensorManager;
import android.location.LocationManager;
import android.content.pm.PackageManager;
import android.content.res.Configuration;

public class MainActivity extends AppCompatActivity
{

	static
	{
		System.loadLibrary("native-lib");
	}


	TextView tv;

	TouchEvListener TEvListener = new TouchEvListener();
	static CDataStream DataStream = null;

	public static SensorManager mSensor_Stream;
	public static LocationManager mLocationmanager;
	private static int mDelay = SensorManager.SENSOR_DELAY_FASTEST;
	private GNSSDataListener m_GNSSData = null;
	SensorListener SensListener = new SensorListener();

	public static boolean m_PermissionWrite = false;
	public static boolean m_PermissionInternet = false;
	public static boolean m_PermissionNetworkState = false;
	public static boolean m_PermissionWifiState = false;

	/**
	 * Called when the activity is first created.
	 */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{

		super.onCreate(savedInstanceState);
		tv = new TextView(this);
		tv.setTextSize(8);
		tv.setOnTouchListener(TEvListener);
		setContentView(tv);
		//m_Touch.Init();
		mSensor_Stream = (SensorManager) getSystemService(SENSOR_SERVICE);
		mLocationmanager = (LocationManager) getSystemService(LOCATION_SERVICE);
		CheckPermissions();

		WifiManager wifiMgr = (WifiManager) getSystemService(WIFI_SERVICE);


		DataStream = new CDataStream(new CNetStream(wifiMgr));

		DataStream.StartStream();

		StartSensors();

		m_GNSSData = new GNSSDataListener(mLocationmanager);


	}

	@Override
	protected void onResume()
	{
		super.onResume();

	}

	@Override
	protected void onStop()
	{
		super.onStop();

	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		DataStream.StopStream();
		StopSensors();
		m_GNSSData.OnDestroy();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig)
	{
		super.onConfigurationChanged(newConfig);

		if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE)
		{
			Log.d("ZZZZZ", "ORIENTATION_LANDSCAPE");
		}
		else if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT)
		{
			Log.d("ZZZZZ", "ORIENTATION_PORTRAIT");
		}
	}


	public static void SendData(int PacketID, ArrayStream Pack)
	{
		DataStream.SendPacket(PacketID, Pack);
	}

	private void CheckPermissions()
	{
		m_PermissionWrite = CheckPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);
		m_PermissionInternet = CheckPermission(Manifest.permission.INTERNET);
		m_PermissionNetworkState = CheckPermission(Manifest.permission.ACCESS_NETWORK_STATE);
		m_PermissionWifiState = CheckPermission(Manifest.permission.ACCESS_WIFI_STATE);
		CheckPermission(Manifest.permission.ACCESS_FINE_LOCATION);
		CheckPermission(Manifest.permission.ACCESS_COARSE_LOCATION);
	}

	public boolean CheckPermission(String permission)
	{
		if (checkSelfPermission(permission) == PackageManager.PERMISSION_GRANTED)
		{
			return true;
		}
		else
		{
			ActivityCompat.requestPermissions(MainActivity.this, new String[]{permission}, 1);
			if (checkSelfPermission(permission) == PackageManager.PERMISSION_GRANTED)
			{
				Log.d("ZZZZZ", "Permission is granted");
				return true;
			}
		}
		return false;
	}

	public boolean StartSensors()
	{
		try
		{
			mSensor_Stream.registerListener(SensListener, mSensor_Stream.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), mDelay);
			mSensor_Stream.registerListener(SensListener, mSensor_Stream.getDefaultSensor(Sensor.TYPE_ACCELEROMETER_UNCALIBRATED), mDelay);
			mSensor_Stream.registerListener(SensListener, mSensor_Stream.getDefaultSensor(Sensor.TYPE_GYROSCOPE), mDelay);
			mSensor_Stream.registerListener(SensListener, mSensor_Stream.getDefaultSensor(Sensor.TYPE_GYROSCOPE_UNCALIBRATED), mDelay);
			mSensor_Stream.registerListener(SensListener, mSensor_Stream.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD), mDelay);
			mSensor_Stream.registerListener(SensListener, mSensor_Stream.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED), mDelay);

		}
		catch (Exception e)
		{
			e.printStackTrace();
			return false;
		}

		return true;
	}

	public void StopSensors()
	{
		mSensor_Stream.unregisterListener(SensListener);
	}
}
