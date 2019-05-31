/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gestureDetector.h"
#include "AndroidInput.h"
#if 1
//--------------------------------------------------------------------------------
// gestureDetector.cpp
//--------------------------------------------------------------------------------

#define LOGI(...) \
   {}
//--------------------------------------------------------------------------------
// includes
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// GestureDetector
//--------------------------------------------------------------------------------
GestureDetector::GestureDetector() { dp_factor_ = 1.f; }

void GestureDetector::SetConfiguration()
{
   dp_factor_ = 160.f; // / AConfiguration_getDensity(config);
}

//--------------------------------------------------------------------------------
// TapDetector
//--------------------------------------------------------------------------------
GESTURE_STATE TapDetector::Detect(const AInputEvent motion_event)
{
   if(!motion_event || motion_event->GetPointerCount() > 1)
   {
      // Only support single touch
      return GESTURE_STATE_NONE;
   }
   std::shared_ptr<CTouchPointer> TouchPtr = motion_event->GetPtr(0);
   switch(motion_event->GetActionMasked())
   {
      case AMOTION_EVENT_ACTION_DOWN:
         if(TouchPtr)
         {
            down_pointer_id_ = TouchPtr->GetPointerId();
            down_x_          = TouchPtr->GetX();
            down_y_          = TouchPtr->GetY();
         }
         break;
      case AMOTION_EVENT_ACTION_UP:
      {
         auto eventTime = motion_event->GetEventTime();
         auto downTime  = motion_event->GetDownTime();
         if(eventTime - downTime <= TAP_TIMEOUT)
         {
            if(down_pointer_id_ == TouchPtr->GetID())
            {
               float x = TouchPtr->GetX() - down_x_;
               float y = TouchPtr->GetY() - down_y_;
               if(x * x + y * y < TOUCH_SLOP * TOUCH_SLOP * dp_factor_)
               {
                  LOGI("TapDetector: Tap detected");
                  return GESTURE_STATE_ACTION;
               }
            }
         }
         break;
      }
   }
   return GESTURE_STATE_NONE;
}

//--------------------------------------------------------------------------------
// DoubletapDetector
//--------------------------------------------------------------------------------
GESTURE_STATE DoubletapDetector::Detect(const AInputEvent motion_event)
{
   if(motion_event->GetPointerCount() > 1)
   {
      // Only support single double tap
      return GESTURE_STATE_NONE;
   }

   bool                           tap_detected = tap_detector_.Detect(motion_event);
   std::shared_ptr<CTouchPointer> TouchPtr     = motion_event->GetPtr(0);
   switch(motion_event->GetActionMasked())
   {
      case AMOTION_EVENT_ACTION_DOWN:
      {
         auto eventTime = motion_event->GetEventTime();
         if(eventTime - last_tap_time_ <= DOUBLE_TAP_TIMEOUT)
         {
            if(TouchPtr)
            {
               float x = TouchPtr->GetX() - last_tap_x_;
               float y = TouchPtr->GetY() - last_tap_y_;
               if(x * x + y * y < DOUBLE_TAP_SLOP * DOUBLE_TAP_SLOP * dp_factor_)
               {
                  LOGI("DoubletapDetector: Doubletap detected");
                  last_tap_time_ = 0;
                  last_tap_x_    = -100;
                  last_tap_y_    = -100;
                  return GESTURE_STATE_ACTION;
               }
            }
         }
         break;
      }
      case AMOTION_EVENT_ACTION_UP:
         if(tap_detected)
         {
            last_tap_time_ = motion_event->GetEventTime();
            if(TouchPtr)
            {
               last_tap_x_ = TouchPtr->GetX();
               last_tap_y_ = TouchPtr->GetY();
            }
         }
         break;
   }
   return GESTURE_STATE_NONE;
}

void DoubletapDetector::SetConfiguration()
{
   dp_factor_ = 160.f; // / AConfiguration_getDensity(config);
   tap_detector_.SetConfiguration();
}

//--------------------------------------------------------------------------------
// PinchDetector
//--------------------------------------------------------------------------------

std::shared_ptr<CTouchPointer> PinchDetector::FindIndex(const AInputEvent event, int32_t id) { return event ? event->FindPtrID(id) : nullptr; }

GESTURE_STATE PinchDetector::Detect(const AInputEvent event)
{
   GESTURE_STATE ret = GESTURE_STATE_NONE;
   event_            = event;
   int32_t pID       = 0;
   int32_t count     = event->GetPointerCount();
   switch(event->GetActionMasked())
   {
      case AMOTION_EVENT_ACTION_DOWN:

         pID = event->GetPtr(0)->GetPointerId();
         vec_pointers_.push_back(pID);
         break;
      case AMOTION_EVENT_ACTION_POINTER_DOWN:
      {
         int32_t iIndex = event->GetActionIndex();
         pID            = event->GetPtr(iIndex)->GetPointerId();
         vec_pointers_.push_back(pID);
         if(count == 2)
         {
            // Start new pinch
            ret = GESTURE_STATE_START;
         }
      }
      break;
      case AMOTION_EVENT_ACTION_UP:
         if(!vec_pointers_.empty())
         {
            vec_pointers_.pop_back();
         }
         break;
      case AMOTION_EVENT_ACTION_POINTER_UP:
      {
         int32_t index               = event->GetActionIndex();
         int32_t released_pointer_id = event->GetPtr(index)->GetPointerId();

         std::vector<int32_t>::iterator it     = vec_pointers_.begin();
         std::vector<int32_t>::iterator it_end = vec_pointers_.end();
         int32_t                        i      = 0;
         for(; it != it_end; ++it, ++i)
         {
            if(*it == released_pointer_id)
            {
               vec_pointers_.erase(it);
               break;
            }
         }

         if(i <= 1)
         {
            // Reset pinch or drag
            if(count != 2)
            {
               // Start new pinch
               ret = (GESTURE_STATE)(GESTURE_STATE_START | GESTURE_STATE_END);
            }
         }
      }
      break;
      case AMOTION_EVENT_ACTION_MOVE:
         switch(count)
         {
            case 1: break;
            default:
               // Multi touch
               ret = GESTURE_STATE_MOVE;
               break;
         }
         break;
      case AMOTION_EVENT_ACTION_CANCEL: break;
   }

   return ret;
}

bool PinchDetector::GetPointers(Vec2& v1, Vec2& v2)
{
   if(vec_pointers_.size() < 2) return false;

   auto IdxPtr = FindIndex(event_, vec_pointers_[0]);
   if(!IdxPtr) return false;

   float x = IdxPtr->GetX();
   float y = IdxPtr->GetY();

   IdxPtr = FindIndex(event_, vec_pointers_[1]);
   if(!IdxPtr) return false;

   float x2 = IdxPtr->GetX();
   float y2 = IdxPtr->GetY();

   v1 = Vec2(x, y);
   v2 = Vec2(x2, y2);

   return true;
}

//--------------------------------------------------------------------------------
// DragDetector
//--------------------------------------------------------------------------------

std::shared_ptr<CTouchPointer> DragDetector::FindIndex(const AInputEvent event, int32_t id) { return event ? event->FindPtrID(id) : nullptr; }

GESTURE_STATE DragDetector::Detect(const AInputEvent event)
{
   GESTURE_STATE ret = GESTURE_STATE_NONE;

   int32_t index = event->GetActionIndex();
   event_        = event;

   int32_t count = event->GetPointerCount();
   switch(event->GetActionMasked())
   {
      case AMOTION_EVENT_ACTION_DOWN:

         vec_pointers_.push_back(event->GetPtr(0)->GetPointerId());
         ret = GESTURE_STATE_START;
         break;
      case AMOTION_EVENT_ACTION_POINTER_DOWN: vec_pointers_.push_back(event->GetPtr(index)->GetPointerId()); break;
      case AMOTION_EVENT_ACTION_UP:
         if(!vec_pointers_.empty())
         {
            vec_pointers_.pop_back();
         }
         ret = GESTURE_STATE_END;
         break;
      case AMOTION_EVENT_ACTION_POINTER_UP:
      {
         int32_t released_pointer_id = event->GetPtr(index)->GetPointerId();

         std::vector<int32_t>::iterator it     = vec_pointers_.begin();
         std::vector<int32_t>::iterator it_end = vec_pointers_.end();
         int32_t                        i      = 0;
         for(; it != it_end; ++it, ++i)
         {
            if(*it == released_pointer_id)
            {
               vec_pointers_.erase(it);
               break;
            }
         }

         if(i <= 1)
         {
            // Reset pinch or drag
            if(count == 2)
            {
               ret = GESTURE_STATE_START;
            }
         }
         break;
      }
      case AMOTION_EVENT_ACTION_MOVE:
         switch(count)
         {
            case 1:
               // Drag
               ret = GESTURE_STATE_MOVE;
               break;
            default: break;
         }
         break;
      case AMOTION_EVENT_ACTION_CANCEL: break;
   }

   return ret;
}

bool DragDetector::GetPointer(Vec2& v)
{
   if(vec_pointers_.size() < 1) return false;

   auto iIndex = FindIndex(event_, vec_pointers_[0]);
   if(!iIndex) return false;

   float x = iIndex->GetX();
   float y = iIndex->GetY();

   v = Vec2(x, y);

   return true;
}

#endif