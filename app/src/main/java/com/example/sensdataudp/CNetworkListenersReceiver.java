package com.example.sensdataudp;
import android.util.Log;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class CNetworkListenersReceiver
{
    private BlockingQueue<InetAddress> mBroadcastAddr = null;
    private DatagramSocket mSocket = null;
    CNetworkListenersReceiver()
    {
        try
        {
            mSocket = new DatagramSocket();
            mSocket.setSoTimeout(1);
        }
        catch (SocketException e)
        {
            mSocket=null;
        }
        mBroadcastAddr=new LinkedBlockingQueue<>();
    }

    private boolean GetAvaiableBroadcastIP()
    {
        //mBroadcastAddr.clear();
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
                        mBroadcastAddr.add(BroadAddr);
                    }
                }
            }
        }
        catch (Exception ignored)
        {
        } // for now eat exceptions
        //return !mBroadcastAddr.isEmpty();
        return !mBroadcastAddr.isEmpty();
    }



    List<InetSocketAddress>  GetNetworkReceivers()
    {

        List<InetSocketAddress> OutList=new LinkedList<>();

        DatagramPacket Packet = null;
        DatagramPacket ReceivePacket=null;

        byte RecvBuf[]=null;
        if(!(mSocket==null))
        {
            ArrayStream mDataOut = new ArrayStream();
            mDataOut.write(0xFF00EE11);
            mDataOut.write(0x22DD33CC);
            mDataOut.write(0xBB44AA55);
            if (GetAvaiableBroadcastIP())
            {
                for (InetAddress InAddr : mBroadcastAddr)
                {
                    Packet = new DatagramPacket(mDataOut.toByteArray(), mDataOut.size(), InAddr, 4452);
                    RecvBuf = new byte[2048];
                    ReceivePacket = new DatagramPacket(RecvBuf, RecvBuf.length);
                    try
                    {
                        mSocket.send(Packet);
                        mSocket.receive(ReceivePacket);
                    }
                    catch (IOException e)
                    {
                        ReceivePacket = null;
                    }
                    if (ReceivePacket != null)
                    {
                        int RecvLen = ReceivePacket.getLength();
                        if (RecvLen == 16)
                        {
                            IntBuffer intBuf = ByteBuffer.wrap(RecvBuf).order(ByteOrder.LITTLE_ENDIAN).asIntBuffer();
                            int[] array = new int[intBuf.remaining()];
                            intBuf.get(array);
                            if (array[0] == 0x00FF11EE && array[1] == 0xDD22CC33 && array[2] == 0x44BB55AA)
                            {
                                OutList.add(new InetSocketAddress(ReceivePacket.getAddress(),array[3]));
                            }
                        }
                        Log.d("z", "ZZZZZ");
                    }
                }
            }
        }
        return OutList;
    }


}
