package com.example.sensdataudp;
import java.io.File;
import java.io.FileOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.io.IOException;
import java.io.ByteArrayOutputStream;

import android.os.Environment;
import android.util.Log;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import android.os.SystemClock;
public class CNetStream {

    //private
    private File OutFile;
    private FileOutputStream fOutStream;

    public static DatagramSocket mSocket = null;
    //public static DatagramPacket mPacket = null;
    public static InetAddress mClientAdr=null;

    public boolean start_UDP_Stream()
    {
        File DataDir=new File(Environment.getExternalStorageDirectory(), "SensDataUdp");
      boolean dd1= DataDir.mkdirs();
      boolean dd2=  DataDir.mkdir();

        final DateFormat dateFormat = new SimpleDateFormat("yyyy_MM_dd-HH_mm_ss");
        OutFile = new File(DataDir.getAbsolutePath(), dateFormat.format(new Date(System.currentTimeMillis()))+".bin");
        //OutFile.mkdirs();
        try {
            OutFile.createNewFile();
        }catch (java.io.IOException e)
        {
            fOutStream=null;
            OutFile=null;
        }

        try {
            fOutStream = new FileOutputStream(OutFile);
        }catch (java.io.FileNotFoundException e)
        {
            fOutStream=null;
            OutFile=null;
        }
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
           // mClientAdr = InetAddress.getByName("192.168.1.10");
            mClientAdr = InetAddress.getByName("192.168.42.255");
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
        try {
            fOutStream.close();
        }catch (IOException e)
        {
            fOutStream=null;
        }
        OutFile = null;

    }

    public void SendPacket(int PacketID, ArrayStream Pack)
    {

        ArrayStream OutPacket=new ArrayStream();
        OutPacket.write(Integer.valueOf(0x1));
        OutPacket.write(Integer.valueOf(PacketID));
        OutPacket.write(Long.valueOf(System.nanoTime()));
        OutPacket.write(Long.valueOf(SystemClock.elapsedRealtimeNanos()));
        OutPacket.write(Integer.valueOf(Pack.size()));
        try {
            OutPacket.write(Pack.toByteArray());
        }
        catch (IOException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
            //Log.e("Error", "SendBlock");
            return;
        }
        byte OutPacketArray[]=OutPacket.toByteArray();


        try {
            fOutStream.write(OutPacketArray);
        }catch (IOException e)
        {
            fOutStream=null;
        }
        if(mSocket!=null&&mClientAdr!=null)
        {
            try {

                DatagramPacket packet = new DatagramPacket(OutPacketArray, OutPacketArray.length, mClientAdr, 4452);
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
