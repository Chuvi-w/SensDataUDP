package com.example.sensdataudp;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.view.MotionEvent;
import android.view.View;
import android.util.Log;
import android.view.View.OnTouchListener;
import android.view.GestureDetector;
import android.view.GestureDetector.SimpleOnGestureListener;

public class MainActivity extends AppCompatActivity implements OnTouchListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    ArrayStream m_Touch = new ArrayStream();
    StringBuilder sb = new StringBuilder();
    TextView tv;
    int upPI = 0;
    int downPI = 0;
    boolean inTouch = false;
    String result = "";

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        tv = new TextView(this);
        tv.setTextSize(8);
        tv.setOnTouchListener(this);
        setContentView(tv);
        m_Touch.Init();
    }

    @Override
    public boolean onTouch(View view, MotionEvent event) {
        // событие
        m_Touch.reset();
        int pointerCount = event.getPointerCount();
        m_Touch.write(Long.valueOf(1));
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
            m_Touch.write(pointerCount);
        }

        ParseTouchEvent(m_Touch.toByteArray());
        PrintTouch(event);
        gestureDetector.onTouchEvent(event);
        return true;
    }

    private boolean PrintTouch(MotionEvent event)
    {

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
        return true;
    }

    @SuppressWarnings("deprecation")
    private final GestureDetector gestureDetector = new GestureDetector(new GestureListener());


    private final class GestureListener extends SimpleOnGestureListener {

        private static final int SWIPE_THRESHOLD = 100;
        private static final int SWIPE_VELOCITY_THRESHOLD = 100;

        @Override
        public boolean onDown(MotionEvent e) {
            Log.d("MyTagGoesHere", "onDown");
            return true;
        }
        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            Log.d("MyTagGoesHere", "onSingleTapConfirmed");
            //onTouch(e);
            return true;
        }


        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
            boolean result = false;
            try {
                float diffY = e2.getY() - e1.getY();
                float diffX = e2.getX() - e1.getX();
                if (Math.abs(diffX) > Math.abs(diffY)) {
                    if (Math.abs(diffX) > SWIPE_THRESHOLD && Math.abs(velocityX) > SWIPE_VELOCITY_THRESHOLD) {
                        if (diffX > 0) {
                            onSwipeRight();
                        } else {
                            onSwipeLeft();
                        }
                    }
                } else {
                    // onTouch(e);
                }
            } catch (Exception exception) {
                exception.printStackTrace();
            }
            return result;
        }
    }


    public void onSwipeRight() {
        Log.d("MyTagGoesHere", "onSwipeRight");
    }

    public void onSwipeLeft() {
        Log.d("MyTagGoesHere", "onSwipeLeft");
    }

    public void onSwipeTop() {
        Log.d("MyTagGoesHere", "onSwipeTop");
    }

    public void onSwipeBottom() {
        Log.d("MyTagGoesHere", "onSwipeBottom");
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native void ParseTouchEvent(byte[] array);
}
