package com.example.sensdataudp;
import android.widget.TextView;

public class CDataStream implements IStreamInterface
{
    private CNetStream m_NetStream = null;
    private CFileStream m_FileStream = null;
    private long m_LastPacketTime = 0;
    private boolean m_bStreamEnable = false;
    private ArrayStream m_MultiPack = null;
    private boolean m_bStartNet =false;
    private boolean m_bStartFile =false;
    private StringBuilder mSB=null;
    private TextView m_TextView;

    CDataStream(CNetStream NetStream, TextView tv)
    {
        m_NetStream = NetStream;
        m_FileStream = new CFileStream();
        m_MultiPack = new ArrayStream();
        mSB=new StringBuilder();
        m_TextView=tv;
    }

    @Override
    public boolean StartStream()
    {
       m_bStartNet = m_NetStream.StartStream();
       m_bStartFile = m_FileStream.StartStream();
        m_bStreamEnable = m_bStartFile || m_bStartNet;
        return m_bStreamEnable;
    }

    @Override
    public void StopStream()
    {
        m_FileStream.StopStream();
        m_NetStream.StopStream();
        m_bStreamEnable = false;
    }

    @Override
    public void SendPacket(ArrayStream DataPacket)
    {
    }

    @Override
    public StringBuilder GetStat()
    {
        mSB.setLength(0);
        if(m_bStartFile)
        {
            mSB.append(m_FileStream.GetStat());
        }
        if(m_bStartNet)
        {
            mSB.append(m_NetStream.GetStat());
        }
        return mSB;
    }

    public void SendPacket(int PacketID, ArrayStream Pack)
    {
        if (!m_bStreamEnable)
        {
            return;
        }
        ArrayStream OutPacket = new ArrayStream();
        OutPacket.write(Integer.valueOf(0x1));
        OutPacket.write(Integer.valueOf(PacketID));
        OutPacket.write(Long.valueOf(System.nanoTime()));
        // OutPacket.write(Long.valueOf(SystemClock.elapsedRealtimeNanos()));
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
        m_TextView.setText(Log);

    }
}
