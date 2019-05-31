package com.example.sensdataudp;

import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.io.IOException;
class ArrayStream extends ByteArrayOutputStream
{
    private ByteBuffer m_Buff=ByteBuffer.allocate(32);

    public ArrayStream()
    {
        this.m_Buff = ByteBuffer.allocate(32);
        m_Buff.order(ByteOrder.LITTLE_ENDIAN);

    }

    public void Init()
    {
        m_Buff.order(ByteOrder.LITTLE_ENDIAN);
    }
    public void write(float value) {

        m_Buff.clear();
        m_Buff.putFloat(value);
        write(m_Buff.array(),0,m_Buff.position());
        //byte[] bytes = new byte[Float.BYTES];
        //ByteBuffer.wrap(bytes).putFloat(value);
        //write(bytes,0,Float.BYTES);
    }

    public void write(double value) {
        m_Buff.clear();
        m_Buff.putDouble(value);
        write(m_Buff.array(),0,m_Buff.position());
        //byte[] bytes = new byte[Double.BYTES];
        //ByteBuffer.wrap(bytes).putDouble(value);
        //write(bytes,0,Double.BYTES);
    }

    public void write(long value) {
        m_Buff.clear();
        m_Buff.putLong(value);
        write(m_Buff.array(),0,m_Buff.position());
        //byte[] bytes = new byte[Long.BYTES];
        //ByteBuffer.wrap(bytes).putLong(value);
        //write(bytes,0,Long.BYTES);
    }
    public void write(int value) {
        m_Buff.clear();
        m_Buff.putInt(value);
        write(m_Buff.array(),0,m_Buff.position());
        //byte[] bytes = new byte[Integer.BYTES];
        //ByteBuffer.wrap(bytes).putInt(value);
        //write(bytes,0,Integer.BYTES);
    }

    public void write(String nString)throws IOException  {
        write(nString.length());
        write(nString.getBytes());
        //byte[] bytes = new byte[Integer.BYTES];
        //ByteBuffer.wrap(bytes).putInt(value);
        //write(bytes,0,Integer.BYTES);
    }

}
