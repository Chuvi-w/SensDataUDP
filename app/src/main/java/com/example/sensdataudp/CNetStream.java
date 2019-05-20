package com.example.sensdataudp;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.io.IOException;

public class CNetStream {

    public static DatagramSocket mSocket = null;
    //public static DatagramPacket mPacket = null;
    public static InetAddress mClientAdr=null;

    public boolean start_UDP_Stream()
    {
		/*
		boolean isOnWifi = isOnWifi();
    	if(isOnWifi == false)
    	{
    		showDialog(R.string.error_warningwifi);
    		return false;
    	}
    	*/


        try
        {
            mClientAdr = InetAddress.getByName("192.168.1.10");
        }
        catch (UnknownHostException e)
        {
            //showDialog(R.string.error_invalidaddr);
            return false;
        }
        try
        {
            mSocket = new DatagramSocket();
            mSocket.setReuseAddress(true);
        }
        catch (SocketException e)
        {
            mSocket = null;
            //showDialog(R.string.error_neterror);
            return false;
        }
        return true;


    }

    public void stop_UDP_Stream()
    {
        if (mSocket != null)
        {
            mSocket.close();
        }
        mSocket = null;
        mClientAdr = null;
    }

    public void SendPacket(ArrayStream Pack)
    {
        if(mSocket!=null&&mClientAdr!=null)
        {
            try {
                byte[] sendBuf = Pack.toByteArray();
                DatagramPacket packet = new DatagramPacket(sendBuf, sendBuf.length, mClientAdr, 4452);
                mSocket.send(packet);
            }
            catch (IOException e)
            {
                // TODO Auto-generated catch block
                e.printStackTrace();
                //Log.e("Error", "SendBlock");
                return;
            }
        }
    }


}
