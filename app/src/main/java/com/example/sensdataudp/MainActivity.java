package com.example.sensdataudp;

import android.hardware.Sensor;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.view.MotionEvent;
import android.view.View;
import android.util.Log;
import android.view.View.OnTouchListener;
import android.view.GestureDetector;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.hardware.SensorManager;
import android.location.LocationManager;



public class MainActivity extends AppCompatActivity  {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }


    TextView tv;

    TouchEvListener TEvListener=new TouchEvListener();
    int upPI = 0;
    int downPI = 0;
    boolean inTouch = false;
    static boolean  bStreamEnable=false;
    static CNetStream NetStream=new CNetStream();
    String result = "";

    public static SensorManager mSensor_Stream;
    public static LocationManager mLocationmanager;
    private static int mDelay = SensorManager.SENSOR_DELAY_FASTEST;

    SensorListener SensListener=new SensorListener();
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        tv = new TextView(this);
        tv.setTextSize(8);
        tv.setOnTouchListener(TEvListener);
        setContentView(tv);
        //m_Touch.Init();
        mSensor_Stream = (SensorManager) getSystemService(SENSOR_SERVICE);
        mLocationmanager = (LocationManager) getSystemService(LOCATION_SERVICE);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(bStreamEnable==false)
        {
            bStreamEnable=NetStream.start_UDP_Stream();
        }

        StartSensors();
    }

    @Override
    protected void onStop()
    {
        super.onStop();
       if(bStreamEnable)
       {
           NetStream.stop_UDP_Stream();
           bStreamEnable=false;
       }
       StopSensors();

    }

    public static void SendData(long PacketID, ArrayStream Pack)
    {
        if(bStreamEnable)
        {
            NetStream.SendPacket(PacketID,Pack);
        }
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

        return  true;
    }

    public void StopSensors()
    {
        mSensor_Stream.unregisterListener(SensListener);
    }
}
