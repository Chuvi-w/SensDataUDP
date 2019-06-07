package com.example.sensdataudp;
import java.net.InetSocketAddress;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

public class CNetStream implements IStreamInterface
{
    public static BlockingQueue<CSockData> mReceivers = null;

    private BlockingQueue<ArrayStream> mDataQueue = null;
    private volatile boolean m_bThreadRunning = false;
    private Thread mNetStreamThread = null;
    private StringBuilder mSB=null;
    private long m_TotalSend=0;
    private MainActivity mMain=null;
   // private List<InetSocketAddress> m_Receivers=null;
    public CNetStream(MainActivity Main)
    {

        mMain=Main;
        mDataQueue = new LinkedBlockingQueue<ArrayStream>();
        mReceivers = new LinkedBlockingQueue<>();
        mSB=new StringBuilder();
    }

    @Override
    public boolean StartStream()
    {
        return  true;
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
    }

    boolean  StartThread()
    {
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
    public void SendPacket(ArrayStream DataPacket)
    {
        if(m_bThreadRunning)
        {
            mDataQueue.add(DataPacket);
            m_TotalSend++;
        }
    }

    @Override
    public StringBuilder GetStat()
    {
        mSB.setLength(0);
        mSB.append("NetStat:\n{\n\tAddr:\n\t{\n");
        for (CSockData nSocket : mReceivers)
        {
            mSB.append("\t\t"+nSocket.GetAddr().getAddress().toString()+":"+nSocket.GetAddr().getPort()+"\n");
        }
        mSB.append("\t}\n");
        mSB.append("\tTotal="+m_TotalSend+"\n"+"\tBuff="+mDataQueue.size()+"\n");

        mSB.append("}\n");
        return mSB;
    }

    @Override
    public boolean IsEnable()
    {
        return m_bThreadRunning;
    }


    Integer getIp(InetSocketAddress addr)
    {
        byte[] a = addr.getAddress().getAddress();
        return ((a[0] & 0xff) << 24) | ((a[1] & 0xff) << 16) | ((a[2] & 0xff) << 8) | (a[3] & 0xff);
    }

    public int CompareInAdr(InetSocketAddress o1, InetSocketAddress o2)
    {
        //TODO deal with nulls
        if (o1 == o2)
        {
            return 0;
        }
        else if(o1.isUnresolved() || o2.isUnresolved())
        {
            return o1.toString().compareTo(o2.toString());
        }
        else
            {
            int compare = getIp(o1).compareTo(getIp(o2));
            if (compare == 0)
            {
                compare = Integer.valueOf(o1.getPort()).compareTo(o2.getPort());
            }
            return compare;
        }
    }
    void AddReceiver(InetSocketAddress InAddr)
    {
        for (CSockData nReceiver : mReceivers)
        {
            if(CompareInAdr(nReceiver.GetAddr(),InAddr)==0)
            {
                return;
            }
        }

        CSockData SockData=new CSockData(InAddr);
        if(SockData.TryConnect())
        {
            mReceivers.add(SockData);
            if(!m_bThreadRunning)
            {
                StartThread();
            }
        }
    }

    int CountReceivers()
    {
        return mReceivers.size();
    }
    class CNetStreamThread implements Runnable
    {
        public void run()
        {
            boolean m_bHaveSendError=false;
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
                    for (CSockData nSocket : mReceivers)
                    {
                        if(!nSocket.SendPacket(OutPacketArray))
                        {
                            m_bHaveSendError=true;
                        }
                    }

                    mReceivers.removeIf(SD->(!SD.IsConnected()));

                    OutPacketArray = null;
                    RecvData = null;
                }
            }
        }
    }
}
