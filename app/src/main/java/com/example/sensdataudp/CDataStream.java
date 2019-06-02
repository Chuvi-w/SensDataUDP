package com.example.sensdataudp;

import java.io.IOException;

public class CDataStream
{

	CNetStream m_NetStream = null;
	CFileStream m_FileStream = null;


	CDataStream(CNetStream NetStream)
	{
		m_NetStream = NetStream;
		m_FileStream = new CFileStream();
	}

	public boolean StartStream()
	{


		boolean bStartNet = m_NetStream.StartStream();
		boolean bStartFile = m_FileStream.StartStream();

		return bStartFile || bStartNet;
	}

	public void StopStream()
	{
		m_FileStream.StopStream();
		m_NetStream.StopStream();
	}

	public void SendPacket(int PacketID, ArrayStream Pack)
	{
		ArrayStream OutPacket = new ArrayStream();
		OutPacket.write(Integer.valueOf(0x1));
		OutPacket.write(Integer.valueOf(PacketID));
		OutPacket.write(Long.valueOf(System.nanoTime()));
		// OutPacket.write(Long.valueOf(SystemClock.elapsedRealtimeNanos()));
		OutPacket.write(Integer.valueOf(Pack.size()));
		try
		{

			OutPacket.write(Pack.toByteArray());
		}
		catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
			//Log.e("Error", "SendBlock");
			return;
		}


		m_FileStream.SendPacket(OutPacket);
		m_NetStream.SendPacket(OutPacket);
	}
}
