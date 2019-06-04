package com.example.sensdataudp;

import java.io.IOException;

public class CDataStream
{

	CNetStream m_NetStream = null;
	CFileStream m_FileStream = null;

	long m_LastPacketTime=0;
	boolean m_bStreamEnable=false;
	ArrayStream m_MultiPack =null;
	CDataStream(CNetStream NetStream)
	{
		m_NetStream = NetStream;
		m_FileStream = new CFileStream();
		m_MultiPack= new ArrayStream();
	}

	public boolean StartStream()
	{


		boolean bStartNet = m_NetStream.StartStream();
		boolean bStartFile = m_FileStream.StartStream();
		m_bStreamEnable=bStartFile || bStartNet;
		return m_bStreamEnable;
	}

	public void StopStream()
	{
		m_FileStream.StopStream();
		m_NetStream.StopStream();
		m_bStreamEnable=false;
	}

	public void SendPacket(int PacketID, ArrayStream Pack)
	{

		if(!m_bStreamEnable)
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

	}
}
