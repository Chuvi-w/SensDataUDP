package com.example.sensdataudp;
import android.net.wifi.WifiManager;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

public class CNetStream implements IStreamInterface
{
    public static BlockingQueue<CSockData> mSockets = null;
    WifiManager m_WiFiMGR = null;
    //public static ArrayList<InetAddress> mBroadcastAddr = null;
    //public static ArrayList<InetAddress> mLocalAddr = null;
    //private Queue<ArrayStream> mDataQueue = null;
    private BlockingQueue<ArrayStream> mDataQueue = null;
    private volatile boolean m_bThreadRunning = false;
    private Thread mNetStreamThread = null;
    private StringBuilder mSB=null;
    private long m_TotalSend=0;
    public CNetStream(WifiManager WiFiMgr)
    {
        m_WiFiMGR = WiFiMgr;
        //mBroadcastAddr = new ArrayList<InetAddress>();
        //mLocalAddr = new ArrayList<InetAddress>();
        //mDataQueue=new ConcurrentLinkedQueue<ArrayStream>();
        mDataQueue = new LinkedBlockingQueue<ArrayStream>();
        mSockets = new LinkedBlockingQueue<>();
        mSB=new StringBuilder();
    }

    @Override
    public boolean StartStream()
    {
        if (!GetAvaiableBroadcastIP())
        {
            return false;
        }
/*
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
*/
        mNetStreamThread = new Thread(new CNetStreamThread());
        if (mNetStreamThread != null)
        {
            m_bThreadRunning = true;
            mNetStreamThread.start();
            mNetStreamThread.setPriority(8);
            return true;
        }
        return false;
    }

    @Override
    public void StopStream()
    {
        m_bThreadRunning = false;
        if (mNetStreamThread != null && mNetStreamThread.isAlive())
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
		/*
		if (mSocket != null)
		{
			mSocket.close();
		}
		mSocket = null;
		mBroadcastAddr = null;
		*/
    }

    @Override
    public void SendPacket(ArrayStream DataPacket)
    {
        mDataQueue.add(DataPacket);
        m_TotalSend++;
    }

    @Override
    public StringBuilder GetStat()
    {
        mSB.setLength(0);
        mSB.append("NetStat:\n{\n\tAddr:\n\t{\n");
        for (CSockData nSocket : mSockets)
        {
            mSB.append("\t\t"+nSocket.GetLocalAddr().toString()+" "+nSocket.GetBroadcastAddr().toString()+"\n");
        }
        mSB.append("\t}\n");
        mSB.append("\tTotal="+m_TotalSend+"\n"+"\tBuff="+mDataQueue.size()+"\n");

        mSB.append("}\n");
        return mSB;
    }

    private boolean GetAvaiableBroadcastIP()
    {
        //mBroadcastAddr.clear();
        mSockets.clear();
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
                            mSockets.add(new CSockData(BroadAddr, addr));
                            //mBroadcastAddr.add(BroadAddr);
                            //mLocalAddr.add(addr);
                        }
                    }
                }
            }
        }
        catch (Exception ignored)
        {
        } // for now eat exceptions
        //return !mBroadcastAddr.isEmpty();
        return !mSockets.isEmpty();
    }

    class CSockData
    {
        private DatagramPacket mPacket = null;
        private DatagramSocket mSocket = null;
        private InetAddress mBroadcastAddr = null;
        private InetAddress mLocalAddr = null;

        CSockData(InetAddress BrAddr, InetAddress LocAddr)
        {
            mBroadcastAddr = BrAddr;
            mLocalAddr = LocAddr;
            try
            {
                mSocket = new DatagramSocket();
                mSocket.setSoTimeout(1);
            }
            catch (SocketException e)
            {
                mSocket = null;
            }
        }

        public void SendPacket(byte[] buf)
        {
            try
            {
                if (mPacket == null)
                {
                    mPacket = new DatagramPacket(buf, buf.length, mBroadcastAddr, 4452);
                }
                else
                {
                    mPacket.setData(buf);
                }
                mSocket.send(mPacket);
            }
            catch (IOException e)
            {
            }
        }

        public InetAddress GetLocalAddr()
        {
            return  mLocalAddr;
        }

        public InetAddress GetBroadcastAddr()
        {
            return  mBroadcastAddr;
        }

    }

    class CNetStreamThread implements Runnable
    {
        public void run()
        {
            ArrayStream RecvData = null;
            byte[] OutPacketArray = null;
            while (!Thread.currentThread().isInterrupted() && m_bThreadRunning)
            {
                try
                {
                    RecvData = mDataQueue.poll(1, TimeUnit.NANOSECONDS);
                }
                catch (InterruptedException e)
                {
                    RecvData = null;
                }
                if (RecvData != null)
                {
                    OutPacketArray = RecvData.toByteArray();
                    for (CSockData nSocket : mSockets)
                    {
                        nSocket.SendPacket(OutPacketArray);
                    }
                    OutPacketArray = null;
                    RecvData = null;
                }
            }
        }
    }
}
