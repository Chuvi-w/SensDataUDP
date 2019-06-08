package com.example.sensdataudp;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.util.Log;

public class SensorListener implements SensorEventListener
{
    ArrayStream mSensorPacket = new ArrayStream();
    long m_Count=0;
    CDataStream m_Sender=null;

    SensorListener(CDataStream Sender)
    {
        m_Count=0;
        m_Sender=Sender;
    }
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
        mSensorPacket.write(m_Count++);
        for (int i = 0; i < event.values.length; i++)
        {
            mSensorPacket.write(event.values[i]);
        }
        m_Sender.SendPacket(0xBB00, mSensorPacket);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy)
    {
        Log.d("AAAAA","Accurasy"+accuracy);
        // sensor.
        //sensor.getType();
    }
}
