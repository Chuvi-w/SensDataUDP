package com.example.sensdataudp;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.app.Activity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.MotionEvent.PointerCoords;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements OnTouchListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
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
    }

    @Override
    public boolean onTouch(View view, MotionEvent event) {
        // событие
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
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
