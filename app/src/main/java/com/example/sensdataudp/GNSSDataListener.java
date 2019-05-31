package com.example.sensdataudp;
import android.location.GnssStatus;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.OnNmeaMessageListener;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import java.io.IOException;
import android.os.Parcel;



public class GNSSDataListener {

    private LocationManager m_locationManager=null;
    private LocationListener m_locationListener = null;
    private OnNmeaMessageListener m_NmeaListener = null;
    private GnssStatus.Callback m_GNSSCallBack=null;
    private  ArrayStream m_NmeaPacket = new ArrayStream();
    private  ArrayStream m_LocationPacket=new ArrayStream();
    //private GpsStatus.Listener gpsStatusListener = null;


    /**
     * Bit mask for mFieldsMask indicating the presence of mAltitude.
     */
    private static final int HAS_ALTITUDE_MASK = 1;
    private static final int HAS_SPEED_MASK = 2;
    private static final int HAS_BEARING_MASK = 4;
    private static final int HAS_HORIZONTAL_ACCURACY_MASK = 8;
    private static final int HAS_MOCK_PROVIDER_MASK = 16;
    private static final int HAS_VERTICAL_ACCURACY_MASK = 32;
    private static final int HAS_SPEED_ACCURACY_MASK = 64;
    private static final int HAS_BEARING_ACCURACY_MASK = 128;


    public GNSSDataListener(LocationManager pLocManager) {

        m_locationManager=pLocManager;

        CreateListeners();
        //m_locationManager.add

    }



    private void CreateListeners()
    {
        m_NmeaListener=new OnNmeaMessageListener() {
            @Override
            public void onNmeaMessage(String message, long timestamp) {
                nmeaProgress(message,timestamp);
                Log.d("SensDataLogMsg", "onNmeaMessage: " );
            }
        };

        m_locationListener=new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {


                byte mFieldsMask=0;
                if(location.hasAltitude())
                {
                    mFieldsMask|=HAS_ALTITUDE_MASK;
                }
                if(location.hasSpeed())
                {
                    mFieldsMask|=HAS_SPEED_MASK;
                }
                if(location.hasBearing())
                {
                    mFieldsMask|=HAS_BEARING_MASK;
                }
                if(location.hasAccuracy())
                {
                    mFieldsMask|=HAS_HORIZONTAL_ACCURACY_MASK;
                }
                if(Build.VERSION.SDK_INT>= Build.VERSION_CODES.O) {
                    if (location.hasVerticalAccuracy()) {
                        mFieldsMask |= HAS_VERTICAL_ACCURACY_MASK;
                    }
                    if (location.hasSpeedAccuracy()) {
                        mFieldsMask |= HAS_SPEED_ACCURACY_MASK;
                    }
                    if (location.hasBearingAccuracy()) {
                        mFieldsMask |= HAS_BEARING_ACCURACY_MASK;
                    }
                }
                if(location.isFromMockProvider())
                {
                    mFieldsMask|=HAS_MOCK_PROVIDER_MASK;
                }
                try {
                m_LocationPacket.reset();


                m_LocationPacket.write(Integer.valueOf(0x5a));
                m_LocationPacket.write(location.getProvider());
                m_LocationPacket.write(location.getTime());
                m_LocationPacket.write(location.getElapsedRealtimeNanos());
                m_LocationPacket.write(mFieldsMask);
                m_LocationPacket.write(location.getLatitude());
                m_LocationPacket.write(location.getLongitude());
                m_LocationPacket.write(location.getAltitude());
                m_LocationPacket.write(location.getSpeed());
                m_LocationPacket.write(location.getBearing());
                m_LocationPacket.write(location.getAccuracy());
                if(Build.VERSION.SDK_INT>= Build.VERSION_CODES.O) {
                    m_LocationPacket.write(location.getVerticalAccuracyMeters());
                    m_LocationPacket.write(location.getSpeedAccuracyMetersPerSecond());
                    m_LocationPacket.write(location.getBearingAccuracyDegrees());
                }
                else
                {
                    m_LocationPacket.write(0.0f);
                    m_LocationPacket.write(0.0f);
                    m_LocationPacket.write(0.0f);
                }

                MainActivity.SendData(0xCC00,m_LocationPacket);

                }catch(IOException exc)
                {

                }
                Log.d("SensDataLogMsg", "onLocationChanged: " );
            }

            @Override
            public void onStatusChanged(String provider, int status, Bundle extras) {
                Log.d("SensDataLogMsg", "onStatusChanged: " );
            }

            @Override
            public void onProviderEnabled(String provider) {
                Log.d("SensDataLogMsg", "onProviderEnabled: " );
            }

            @Override
            public void onProviderDisabled(String provider) {
                Log.d("SensDataLogMsg", "onProviderDisabled: " );
            }
        };

        m_GNSSCallBack=new GnssStatus.Callback() {
            @Override
            public void onStarted() {
                super.onStarted();
                Log.d("SensDataLogMsg", "GnssStatus OnStary: " );
            }

            @Override
            public void onStopped() {
                super.onStopped();
                Log.d("SensDataLogMsg", "GNSS onStopped: " );
            }

            @Override
            public void onFirstFix(int ttffMillis) {
                super.onFirstFix(ttffMillis);
                Log.d("SensDataLogMsg", "GNSS onFirstFix: " );
            }

            @Override
            public void onSatelliteStatusChanged(GnssStatus status) {
                super.onSatelliteStatusChanged(status);
                Log.d("SensDataLogMsg", "GNSS onSatelliteStatusChanged: " );
            }
        };


    try
    {
        m_locationManager.registerGnssStatusCallback(m_GNSSCallBack);
        m_locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 0, 0, m_locationListener);
        m_locationManager.addNmeaListener(m_NmeaListener);
    }
    catch (SecurityException exp)
    {
        Log.e("SensDataLogMsg", "CreateListeners: " );
    }

    }

    public void OnDestroy()
    {
        m_locationManager.removeUpdates(m_locationListener);
        m_locationManager.removeNmeaListener(m_NmeaListener);
    }

    private void nmeaProgress(String message, long timestamp) {


        try {
            m_NmeaPacket.reset();
            m_NmeaPacket.write(Integer.valueOf(0xa5));
            m_NmeaPacket.write(timestamp);
            m_NmeaPacket.write(message);
            MainActivity.SendData(0xCC00, m_NmeaPacket);
        }catch(IOException exc)
        {

        }

    }




    }
