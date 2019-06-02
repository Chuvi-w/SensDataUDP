package com.example.sensdataudp;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.io.IOException;

import android.net.wifi.WifiManager;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentLinkedQueue;
public class CNetStream
{

	public static DatagramSocket mSocket = null;
	public static ArrayList<InetAddress> mBroadcastAddr = null;
	public static ArrayList<InetAddress> mLocalAddr = null;
	//private Queue<ArrayStream> mDataQueue = null;
	private BlockingQueue<ArrayStream> mDataQueue = null;
	private volatile boolean m_bThreadRunning = false;
	private Thread mNetStreamThread=null;


	WifiManager m_WiFiMGR = null;


	public CNetStream(WifiManager WiFiMgr)
	{
		m_WiFiMGR = WiFiMgr;
		mBroadcastAddr = new ArrayList<InetAddress>();
		mLocalAddr = new ArrayList<InetAddress>();
		//mDataQueue=new ConcurrentLinkedQueue<ArrayStream>();
		mDataQueue=new LinkedBlockingQueue<ArrayStream>();
	}

	public boolean StartStream()
	{


		if (!GetAvaiableBroadcastIP())
		{
			return false;
		}

		try
		{
			mSocket = new DatagramSocket();
			//mSocket.setReuseAddress(true);
		}
		catch (SocketException e)
		{
			mSocket = null;
			//showDialog(R.string.error_neterror);
			return false;
		}

		mNetStreamThread=new Thread(new CNetStreamThread());
		if(mNetStreamThread!=null)
		{
			m_bThreadRunning=true;
			mNetStreamThread.start();
			return  true;
		}
		return false;
	}

	public void StopStream()
	{
		m_bThreadRunning=false;
		if (mNetStreamThread != null&&mNetStreamThread.isAlive())
		{
			mNetStreamThread.interrupt();
			try
			{
				mNetStreamThread.join();
			}
			catch (java.lang.InterruptedException e)
			{

			}
		}
		if (mSocket != null)
		{
			mSocket.close();
		}
		mSocket = null;
		mBroadcastAddr = null;
	}

	public void SendPacket(ArrayStream DataPacket)
	{
		mDataQueue.add(DataPacket);


	}

	private boolean GetAvaiableBroadcastIP()
	{
		mBroadcastAddr.clear();
		try
		{
			List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
			for (NetworkInterface intf : interfaces)
			{
				List<InetAddress> addrs = Collections.list(intf.getInetAddresses());
				List<InterfaceAddress> intfAddrs = intf.getInterfaceAddresses();

				for (InterfaceAddress IntF : intfAddrs)
				{
					InetAddress addr = IntF.getAddress();
					InetAddress BroadAddr = IntF.getBroadcast();
					if (!addr.isLoopbackAddress() && BroadAddr != null)
					{
						String sAddr = addr.getHostAddress();
						boolean isIPv4 = sAddr.indexOf(':') < 0;
						if (isIPv4)
						{
							mBroadcastAddr.add(BroadAddr);
							mLocalAddr.add(addr);
						}
					}
				}
			}
		}
		catch (Exception ignored)
		{
		} // for now eat exceptions
		return !mBroadcastAddr.isEmpty();
	}
	class CNetStreamThread implements Runnable
	{
		public void run()
		{

			while (!Thread.currentThread().isInterrupted()&&m_bThreadRunning)
			{
				//try
				{
					if (!mDataQueue.isEmpty())
					{

						byte [] OutPacketArray=mDataQueue.poll().toByteArray();
						if (mSocket != null && mBroadcastAddr != null)
						{
							for (InetAddress BrAddr : mBroadcastAddr)
							{
								try
								{
									DatagramPacket packet = new DatagramPacket(OutPacketArray, OutPacketArray.length, BrAddr, 4452);
									mSocket.send(packet);

								}
								catch (IOException e)
								{

								}
							}
						}
					}
				//	Thread.sleep(0,500);
				}
				//catch (InterruptedException ex)
				//{
				//	Thread.currentThread().interrupt();
				//}

			}
		}
	}

}
