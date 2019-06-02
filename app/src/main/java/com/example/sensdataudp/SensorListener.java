package com.example.sensdataudp;

import android.app.usage.UsageEvents;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorManager;
import android.hardware.SensorEventListener;
import android.app.Activity;

public class SensorListener implements SensorEventListener

{

	ArrayStream mSensorPacket = new ArrayStream();

	@Override
	public void onSensorChanged(SensorEvent event)
	{
		mSensorPacket.reset();
		mSensorPacket.write(Long.valueOf(event.sensor.getType()));
		mSensorPacket.write(event.timestamp);
		// mSensorPacket.write(event.sensor);
		mSensorPacket.write(Long.valueOf(event.values.length));
		mSensorPacket.write(event.sensor.getResolution());
		mSensorPacket.write(event.sensor.getMaximumRange());
		for (int i = 0; i < event.values.length; i++)
		{
			mSensorPacket.write(event.values[i]);
		}
		MainActivity.SendData(0xBB00, mSensorPacket);
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy)
	{
		// sensor.
		//sensor.getType();
	}
}
