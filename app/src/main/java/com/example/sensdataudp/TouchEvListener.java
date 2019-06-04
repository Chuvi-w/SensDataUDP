package com.example.sensdataudp;
import android.view.MotionEvent;
import android.view.View;

public class TouchEvListener implements View.OnTouchListener
{
    StringBuilder sb = new StringBuilder();
    ArrayStream m_Touch = new ArrayStream();

    @Override
    public boolean onTouch(View view, MotionEvent event)
    {
        // событие
        m_Touch.reset();
        int pointerCount = event.getPointerCount();
        //m_Touch.write(Long.valueOf(0x1));
        //m_Touch.write(Long.valueOf(0xAA00));
        int ViewW = view.getWidth();
        int ViewH = view.getHeight();
        m_Touch.write(event.getEventTime());
        m_Touch.write(event.getDownTime());
        m_Touch.write(event.getAction());
        m_Touch.write(event.getActionMasked());
        m_Touch.write(event.getXPrecision());
        m_Touch.write(event.getYPrecision());
        m_Touch.write(event.getRawX());
        m_Touch.write(event.getRawY());
        m_Touch.write(event.getActionIndex());
        m_Touch.write(pointerCount);
        m_Touch.write(ViewW);
        m_Touch.write(ViewH);
        for (int i = 0; i < pointerCount; i++)
        {
            m_Touch.write(i);
            m_Touch.write(event.getPointerId(i));
            m_Touch.write(event.getX(i));
            m_Touch.write(event.getY(i));
            m_Touch.write(event.getToolType(i));
            m_Touch.write(event.getSize(i));
            m_Touch.write(event.getToolMajor(i));
            m_Touch.write(event.getToolMinor(i));
            m_Touch.write(event.getTouchMajor(i));
            m_Touch.write(event.getTouchMinor(i));
            m_Touch.write(event.getPressure(i));
            m_Touch.write(event.getOrientation(i));
            //m_Touch.write(pointerCount);
        }
        MainActivity.SendData(0xAA00, m_Touch);
        return true;
    }
}
