#include "AndroidHelperFuncs.h"

size_t AMotionEvent_getPointerCount(const AInputEvent* motion_event)
{
   if(!motion_event)
   {
      return 0;
   }
   return motion_event->MotEv.PointerCount;
}

int32_t AMotionEvent_getPointerId(const AInputEvent* motion_event, size_t pointer_index)
{
   if(!motion_event || pointer_index > motion_event->MotEv.PointerCount)
   {
      return 0;
   }
   return motion_event->vPtr[pointer_index].PointerId;
}

float AMotionEvent_getX(const AInputEvent* motion_event, size_t pointer_index)
{
   if(!motion_event)
   {
      return 0.0;
   }
   return motion_event->vPtr[pointer_index].X;
}

float AMotionEvent_getY(const AInputEvent* motion_event, size_t pointer_index)
{
   if(!motion_event)
   {
      return 0.0;
   }
   return motion_event->vPtr[pointer_index].Y;
}

int32_t AMotionEvent_getAction(const AInputEvent* motion_event)
{
   if(!motion_event)
   {
      return 0;
   }
   return motion_event->MotEv.Action;
}

int64_t AMotionEvent_getDownTime(const AInputEvent* motion_event)
{
   if(!motion_event)
   {
      return 0;
   }
   return motion_event->MotEv.DownTime;
}

int64_t AMotionEvent_getEventTime(const AInputEvent* motion_event)
{
   if(!motion_event)
   {
      return 0;
   }
   return motion_event->MotEv.EventTime;
}
