package com.example.sensdataudp;
import android.hardware.Sensor;

import java.net.InetSocketAddress;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class CDataStream implements IStreamInterface
{
  //  private CNetStream m_NetStream = null;
    private CFileStream m_FileStream = null;
    private CNetworkListenersReceiver m_NetReceiver=null;
    private StringBuilder mSB=null;
    private MainActivity mMain=null;
    private  int m_nGNSSCount=0;
    private  int m_nNMEACount=0;
    private  int m_nTouchCount=0;
    private  int m_nSensCount=0;
    private  int m_nAccCount=0;
    private  int m_nAccCountUnc=0;
    private  int m_nGyrCount=0;
    private  int m_nGyrCountUnc=0;
    private  int m_nMagCount=0;
    private  int m_nMagCountUnc=0;
   private  int m_nCount=1001;
    Timer m_Timer=null;
    //CCheckReceiverTask m_CheckReceiverTask=null;

    class CCheckReceiverTask extends TimerTask
    {
        private long StartTime=0;
        CCheckReceiverTask(long nStart)
        {
            StartTime=nStart;
        }
        @Override
        public void run()
        {
/*
            if(m_NetStream.CountReceivers()==0&&m_NetStream.IsEnable())
            {
                m_NetStream.StopStream();
            }

            if(System.currentTimeMillis()-StartTime>=1000)
            {
                if(!m_NetStream.IsEnable())
                {
                    if(!m_FileStream.IsEnable())
                    {
                        m_FileStream.StartStream();
                    }
                }
                else
                {
                    if(m_FileStream.IsEnable())
                    {
                        m_FileStream.StopStream();
                    }
                }
            }

            List<InetSocketAddress> Receivers= m_NetReceiver.GetNetworkReceivers();
            if(!Receivers.isEmpty())
            {
                for (InetSocketAddress nAddr : Receivers)
                {
                   m_NetStream.AddReceiver(nAddr);
                }
            }
            */
            if(!m_FileStream.IsEnable())
            {
               m_nGNSSCount=0;
               m_nTouchCount=0;
               m_nSensCount=0;
               m_nAccCount=0;
               m_nAccCountUnc=0;
               m_nGyrCount=0;
               m_nGyrCountUnc=0;
               m_nMagCount=0;
               m_nMagCountUnc=0;
               m_nNMEACount=0;
               m_FileStream.StartStream();
            }
        }
    }

    CDataStream(MainActivity Main)
    {
        mMain=Main;
        m_NetReceiver=new CNetworkListenersReceiver();
       // m_NetStream = new CNetStream(Main);
        m_FileStream = new CFileStream(Main);
        mSB=new StringBuilder();
        m_Timer = new Timer();
      //  m_CheckReceiverTask=new CCheckReceiverTask(System.currentTimeMillis());

    }

    @Override
    public boolean StartStream()
    {

      // m_Timer.schedule(m_CheckReceiverTask,1000,1000);
       if(!m_FileStream.IsEnable())
       {
          m_nGNSSCount=0;
          m_nNMEACount=0;
          m_nTouchCount=0;
          m_nSensCount=0;
          m_nAccCount=0;
          m_nAccCountUnc=0;
          m_nGyrCount=0;
          m_nGyrCountUnc=0;
          m_nMagCount=0;
          m_nMagCountUnc=0;
          m_FileStream.StartStream();
       }
       return  true;
    }


    @Override
    public void StopStream()
    {
        //m_Timer.

        m_FileStream.StopStream();
       // m_NetStream.StopStream();
    }

    @Override
    public void SendPacket(ArrayStream DataPacket)
    {
    }

    @Override
    public StringBuilder GetStat()
    {
        mSB.setLength(0);
        if(m_FileStream.IsEnable())
        {
            mSB.append(m_FileStream.GetStat());
        }
        /*
        if(m_NetStream.IsEnable())
        {
            mSB.append(m_NetStream.GetStat());
        }
        */
     /*  m_nGNSSCount=0;
       m_nTouchCount=0;
       m_nSensCount=0;
       m_nAccCount=0;
       m_nAccCountUnc=0;
       m_nGyrCount=0;
       m_nGyrCountUnc=0;
       m_nMagCount=0;
       m_nMagCountUnc=0;
       */

       mSB.append("Count:\n{\n");
       mSB.append("\tGNSS="+m_nGNSSCount+":"+m_nNMEACount+"\n\tTouch="+m_nTouchCount+"\n\tSens="+m_nSensCount+"\n\t{\n");
       mSB.append("\t\tAcc  ="+m_nAccCount+"\n\t\tAccUn="+m_nAccCountUnc+"\n");
       mSB.append("\t\tGyr  ="+m_nGyrCount+"\n\t\tGyrUn="+m_nGyrCountUnc+"\n");
       mSB.append("\t\tMag  ="+m_nMagCount+"\n\t\tMagUn="+m_nMagCountUnc+"\n");
       mSB.append("\n}\n");

        return mSB;
    }

    @Override
    public boolean IsEnable()
    {
        return m_FileStream.IsEnable()/* || m_NetStream.IsEnable()*/;
    }

    public void SendPacket(int PacketID, ArrayStream Pack, int SensorType)
    {


        if (!IsEnable())
        {
            return;
        }
        ArrayStream OutPacket = new ArrayStream();
        OutPacket.write(Integer.valueOf(0x1));
        OutPacket.write(Integer.valueOf(PacketID));
        OutPacket.write(System.nanoTime());
        OutPacket.write(System.currentTimeMillis());
        OutPacket.write(Integer.valueOf(Pack.size()));
        OutPacket.write(Pack);
        m_FileStream.SendPacket(OutPacket);
      //  m_NetStream.SendPacket(OutPacket);
        //	m_MultiPack.write(OutPacket);
        //	//if(System.nanoTime()-m_LastPacketTime>(20*1000*1000))
        //	if(m_MultiPack.size()>1024)
        //	{
        //		m_FileStream.SendPacket(m_MultiPack);
        //		m_NetStream.SendPacket(m_MultiPack);
        //		m_MultiPack.reset();
        //		m_LastPacketTime=System.nanoTime();
        //	}

       if(PacketID==0xCC00)
       {
          if(SensorType==1)
          {
             m_nNMEACount++;
          }
          else
          {
             m_nGNSSCount++;
          }
       }
       if(PacketID==0xAA00)
       {
          m_nTouchCount++;
       }
       if(PacketID==0xBB00)
       {
          m_nSensCount++;
          switch (SensorType)
          {
             case Sensor.TYPE_ACCELEROMETER:
                m_nAccCount++;
                break;
             case Sensor.TYPE_ACCELEROMETER_UNCALIBRATED:
                m_nAccCountUnc++;
                break;
             case Sensor.TYPE_GYROSCOPE:
                m_nGyrCount++;
                break;
             case Sensor.TYPE_GYROSCOPE_UNCALIBRATED:
                m_nGyrCountUnc++;
                break;
             case Sensor.TYPE_MAGNETIC_FIELD:
                m_nMagCount++;
                break;
             case Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED:
                m_nMagCountUnc++;
                break;
                default:
                   break;
          }
       }

       if(m_nCount++>=199)
       {
         m_nCount=0;
          StringBuilder Log = GetStat();
          mMain.tv.setText(Log);
       }

    }
}
