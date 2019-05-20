package com.example.sensdataudp;

import android.view.MotionEvent;
import android.view.View;

public class TouchEvListener implements View.OnTouchListener {

    StringBuilder sb = new StringBuilder();
    ArrayStream m_Touch = new ArrayStream();
    @Override
    public boolean onTouch(View view, MotionEvent event) {
        // событие
        m_Touch.reset();
        int pointerCount = event.getPointerCount();
        //m_Touch.write(Long.valueOf(0x1));
        //m_Touch.write(Long.valueOf(0xAA00));
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
        for (int i = 0; i < pointerCount; i++) {

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

        // ParseTouchEvent(m_Touch.toByteArray());
        //  PrintTouch(event);
        //  gestureDetector.onTouchEvent(event);

        MainActivity.SendData(0xAA00,m_Touch);
        return true;
    }

    private boolean PrintTouch(MotionEvent event)
    {
/*
        int actionMask = event.getActionMasked();
        // индекс касания
        int pointerIndex = event.getActionIndex();
        // число касаний
        int pointerCount = event.getPointerCount();

        sb.setLength(0);
        sb.append(event.actionToString(event.getAction())+"\r\n");
        sb.append("{" + event.getXPrecision()+", "+event.getYPrecision()+"}, ");
        sb.append("{" + event.getRawX()+", "+event.getRawY()+"}\r\n");
        sb.append(event.getEventTime()-event.getDownTime()+", "+event.getEdgeFlags()+"\r\n");

        for (int i = 0; i < 10; i++) {
            sb.append(i);
            if (i < pointerCount) {
                sb.append("," + event.getPointerId(i));
                sb.append(", {" + event.getToolMajor(i)+", "+event.getToolMinor(i)+"}");
                sb.append(", {" + event.getTouchMajor(i)+", "+event.getTouchMinor(i)+"}, "+event.getPressure(i)+", "+event.getOrientation(i));
                sb.append(", {" + event.getX(i)+", "+event.getY(i)+"}");

            } else {
                sb.append(",");
                sb.append("{");
                sb.append(",");
                sb.append("}");
            }
            sb.append("\r\n");
        }

        switch (actionMask) {
            case MotionEvent.ACTION_DOWN: // первое касание
                inTouch = true;
            case MotionEvent.ACTION_POINTER_DOWN: // последующие касания
                downPI = pointerIndex;
                break;

            case MotionEvent.ACTION_UP: // прерывание последнего касания
                inTouch = false;
                //sb.setLength(0);
            case MotionEvent.ACTION_POINTER_UP: // прерывания касаний
                upPI = pointerIndex;
                break;

            case MotionEvent.ACTION_MOVE: // движение

                break;
        }
        result = "down: " + downPI + "\n" + "up: " + upPI + "\n";

        //if (inTouch)
        {
            result += "pointerCount = " + pointerCount + "\n" + sb.toString();
        }
        tv.setText(result);
     */
        return true;
    }
}
