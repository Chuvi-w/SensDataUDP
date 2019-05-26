#ifndef AndroidHelperFuncs_h__
#define AndroidHelperFuncs_h__

#include <vector>
#include "MatConv.h"
#include "TouchEvData.h"
class AConfiguration;
typedef TouchEvent_t AInputEvent;

/**
 * Get the number of pointers of data contained in this event.
 * Always >= 1.
 */
size_t AMotionEvent_getPointerCount(const AInputEvent* motion_event);

/**
 * Get the pointer identifier associated with a particular pointer
 * data index in this event.  The identifier tells you the actual pointer
 * number associated with the data, accounting for individual pointers
 * going up and down since the start of the current gesture.
 */
int32_t AMotionEvent_getPointerId(const AInputEvent* motion_event, size_t pointer_index);

/**
 * Get the current X coordinate of this event for the given pointer index.
 * Whole numbers are pixels; the value may have a fraction for input devices
 * that are sub-pixel precise.
 */
float AMotionEvent_getX(const AInputEvent* motion_event, size_t pointer_index);

/**
 * Get the current Y coordinate of this event for the given pointer index.
 * Whole numbers are pixels; the value may have a fraction for input devices
 * that are sub-pixel precise.
 */
float AMotionEvent_getY(const AInputEvent* motion_event, size_t pointer_index);

/** Get the combined motion event action code and pointer index. */
int32_t AMotionEvent_getAction(const AInputEvent* motion_event);

/** Get the motion event flags. */
// int32_t AMotionEvent_getFlags(const AInputEvent* motion_event);

/**
 * Get the time when the user originally pressed down to start a stream of
 * position events, in the java.lang.System.nanoTime() time base.
 */
int64_t AMotionEvent_getDownTime(const AInputEvent* motion_event);

/**
 * Get the time when this specific event was generated,
 * in the java.lang.System.nanoTime() time base.
 */
int64_t AMotionEvent_getEventTime(const AInputEvent* motion_event);
#endif // AndroidHelperFuncs_h__
