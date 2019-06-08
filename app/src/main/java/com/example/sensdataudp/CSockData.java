package com.example.sensdataudp;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;

class CSockData
{
    InetSocketAddress mInAddr=null;
    Socket mSocket = null;

    private boolean mBConnected=false;
    CSockData(InetSocketAddress InAddr)
    {

        mInAddr=InAddr;
        try
        {
            mSocket = new Socket();
            mSocket.setTcpNoDelay(true);
            mSocket.setSoTimeout(1);
        }
        catch (SocketException e)
        {
            mSocket = null;
        }
    }

    boolean IsConnected()
    {
        return mBConnected;
    }
    boolean TryConnect()
    {
        if(mSocket==null)
        {
            return  false;
        }
        if(mBConnected)
        {
            return  true;
        }
        try
        {
            mSocket.connect(mInAddr,10);
         //  mSocket.setTcpNoDelay(true);
         //  mSocket.setSoTimeout(1);
            mBConnected=true;
        }
        catch (IOException e)
        {
            mBConnected=false;
        }
        return  mBConnected;
    }

    public boolean SendPacket(byte[] buf)
    {
        if(buf!=null&&buf.length>0&&mBConnected)
        {

            try
            {
                mSocket.getOutputStream().write(buf);
            }

            catch (java.net.SocketException e)
            {
                mBConnected=false;
               // e.printStackTrace();
            }
            catch (IOException e)
            {
                mBConnected=false;
                //e.printStackTrace();
            }

        }
        return  mBConnected;
    }

    public InetSocketAddress GetAddr()
    {
        return  mInAddr;
    }

}
