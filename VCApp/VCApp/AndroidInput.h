#pragma once

/**
 * Bit shift for the action bits holding the pointer index as
 * defined by AMOTION_EVENT_ACTION_POINTER_INDEX_MASK.
 */
#define AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT 8

/** Motion event actions */
enum
{
   /** Bit mask of the parts of the action code that are the action itself. */
   AMOTION_EVENT_ACTION_MASK = 0xff,

   /**
    * Bits in the action code that represent a pointer index, used with
    * AMOTION_EVENT_ACTION_POINTER_DOWN and AMOTION_EVENT_ACTION_POINTER_UP.  Shifting
    * down by AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT provides the actual pointer
    * index where the data for the pointer going up or down can be found.
    */
   AMOTION_EVENT_ACTION_POINTER_INDEX_MASK = 0xff00,

   /** A pressed gesture has started, the motion contains the initial starting location. */
   AMOTION_EVENT_ACTION_DOWN = 0,

   /**
    * A pressed gesture has finished, the motion contains the final release location
    * as well as any intermediate points since the last down or move event.
    */
   AMOTION_EVENT_ACTION_UP = 1,

   /**
    * A change has happened during a press gesture (between AMOTION_EVENT_ACTION_DOWN and
    * AMOTION_EVENT_ACTION_UP).  The motion contains the most recent point, as well as
    * any intermediate points since the last down or move event.
    */
   AMOTION_EVENT_ACTION_MOVE = 2,

   /**
    * The current gesture has been aborted.
    * You will not receive any more points in it.  You should treat this as
    * an up event, but not perform any action that you normally would.
    */
   AMOTION_EVENT_ACTION_CANCEL = 3,

   /**
    * A movement has happened outside of the normal bounds of the UI element.
    * This does not provide a full gesture, but only the initial location of the movement/touch.
    */
   AMOTION_EVENT_ACTION_OUTSIDE = 4,

   /**
    * A non-primary pointer has gone down.
    * The bits in AMOTION_EVENT_ACTION_POINTER_INDEX_MASK indicate which pointer changed.
    */
   AMOTION_EVENT_ACTION_POINTER_DOWN = 5,

   /**
    * A non-primary pointer has gone up.
    * The bits in AMOTION_EVENT_ACTION_POINTER_INDEX_MASK indicate which pointer changed.
    */
   AMOTION_EVENT_ACTION_POINTER_UP = 6,

   /**
    * A change happened but the pointer is not down (unlike AMOTION_EVENT_ACTION_MOVE).
    * The motion contains the most recent point, as well as any intermediate points since
    * the last hover move event.
    */
   AMOTION_EVENT_ACTION_HOVER_MOVE = 7,

   /**
    * The motion event contains relative vertical and/or horizontal scroll offsets.
    * Use getAxisValue to retrieve the information from AMOTION_EVENT_AXIS_VSCROLL
    * and AMOTION_EVENT_AXIS_HSCROLL.
    * The pointer may or may not be down when this event is dispatched.
    * This action is always delivered to the winder under the pointer, which
    * may not be the window currently touched.
    */
   AMOTION_EVENT_ACTION_SCROLL = 8,

   /** The pointer is not down but has entered the boundaries of a window or view. */
   AMOTION_EVENT_ACTION_HOVER_ENTER = 9,

   /** The pointer is not down but has exited the boundaries of a window or view. */
   AMOTION_EVENT_ACTION_HOVER_EXIT = 10,

   /* One or more buttons have been pressed. */
   AMOTION_EVENT_ACTION_BUTTON_PRESS = 11,

   /* One or more buttons have been released. */
   AMOTION_EVENT_ACTION_BUTTON_RELEASE = 12,
};
