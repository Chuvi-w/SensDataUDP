package com.example.sensdataudp;
import java.net.InetSocketAddress;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class CDataStream implements IStreamInterface
{
    private CNetStream m_NetStream = null;
    private CFileStream m_FileStream = null;
    private CNetworkListenersReceiver m_NetReceiver=null;
    private StringBuilder mSB=null;
    private MainActivity mMain=null;
    Timer m_Timer=null;
    CCheckReceiverTask m_CheckReceiverTask=null;

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





        }
    }

    CDataStream(MainActivity Main)
    {
        mMain=Main;
        m_NetReceiver=new CNetworkListenersReceiver();
        m_NetStream = new CNetStream(Main);
        m_FileStream = new CFileStream(Main);
        mSB=new StringBuilder();
        m_Timer = new Timer();
        m_CheckReceiverTask=new CCheckReceiverTask(System.currentTimeMillis());

    }

    @Override
    public boolean StartStream()
    {

       m_Timer.schedule(m_CheckReceiverTask,1000,1000);
       return  true;
    }


    @Override
    public void StopStream()
    {
        //m_Timer.

        m_FileStream.StopStream();
        m_NetStream.StopStream();
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
        if(m_NetStream.IsEnable())
        {
            mSB.append(m_NetStream.GetStat());
        }
        return mSB;
    }

    @Override
    public boolean IsEnable()
    {
        return m_FileStream.IsEnable() || m_NetStream.IsEnable();
    }

    public void SendPacket(int PacketID, ArrayStream Pack)
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
        m_NetStream.SendPacket(OutPacket);
        //	m_MultiPack.write(OutPacket);
        //	//if(System.nanoTime()-m_LastPacketTime>(20*1000*1000))
        //	if(m_MultiPack.size()>1024)
        //	{
        //		m_FileStream.SendPacket(m_MultiPack);
        //		m_NetStream.SendPacket(m_MultiPack);
        //		m_MultiPack.reset();
        //		m_LastPacketTime=System.nanoTime();
        //	}

        StringBuilder Log=GetStat();
        mMain.tv.setText(Log);

    }
}
