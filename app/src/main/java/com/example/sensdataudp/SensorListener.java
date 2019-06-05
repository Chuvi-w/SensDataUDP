package com.example.sensdataudp;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.util.Log;

public class SensorListener implements SensorEventListener
{
    ArrayStream mSensorPacket = new ArrayStream();

    @Override
    public void onSensorChanged(SensorEvent event)
    {
        mSensorPacket.reset();
        mSensorPacket.write(event.timestamp);
        mSensorPacket.write(event.sensor.getType());
        mSensorPacket.write(event.accuracy);
        mSensorPacket.write(Integer.valueOf(event.values.length));
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
        Log.d("AAAAA","Accurasy"+accuracy);
        // sensor.
        //sensor.getType();
    }
}
