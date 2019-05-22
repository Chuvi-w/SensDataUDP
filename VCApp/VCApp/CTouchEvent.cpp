#include "CTouchEvent.h"
#include <string.h>
#include <stdio.h>

CTouchEvent::CTouchEvent() {}

CTouchEvent::~CTouchEvent() {}

bool CTouchEvent::ParseEvent(const void* pData, size_t nDataSize, bool bEndian)
{
   if(nDataSize < sizeof(MotionEvent_t))
   {
      return false;
   }
   TouchEvent_t         TEv;
   const PointerData_t* pPtrData;
   PointerData_t        PtrData;
   memcpy(&TEv.MotEv, pData, sizeof(MotionEvent_t));
   if(!bEndian)
   {
      bswap(TEv.MotEv.EventTime);
      bswap(TEv.MotEv.DownTime);
      bswap(TEv.MotEv.Action);
      bswap(TEv.MotEv.ActionMasked);
      bswap(TEv.MotEv.XPrecision);
      bswap(TEv.MotEv.YPrecision);
      bswap(TEv.MotEv.RawX);
      bswap(TEv.MotEv.RawY);
      bswap(TEv.MotEv.ActionIndex);
      bswap(TEv.MotEv.PointerCount);
   }
   if(nDataSize < sizeof(MotionEvent_t) + TEv.MotEv.PointerCount * sizeof(PointerData_t))
   {
      printf("Wrong DataSize\n");
      return false;
   }
   pPtrData = reinterpret_cast<const PointerData_t*>((size_t)pData + sizeof(MotionEvent_t));
   // printf("PC=%i\n", TEv.MotEv.PointerCount);
   for(int i = 0; i < TEv.MotEv.PointerCount; i++)
   {
      memcpy(&PtrData, &pPtrData[i], sizeof(PointerData_t));
      if(!bEndian)
      {
         bswap(PtrData.nID);
         bswap(PtrData.PointerId);
         bswap(PtrData.X);
         bswap(PtrData.Y);
         bswap(PtrData.ToolType);
         bswap(PtrData.Size);
         bswap(PtrData.ToolMajor);
         bswap(PtrData.ToolMinor);
         bswap(PtrData.TouchMajor);
         bswap(PtrData.TouchMinor);
         bswap(PtrData.Pressure);
         bswap(PtrData.Orientation);
      }

      //  printf("%i %i %.4f %.4f\n", PtrData.nID, PtrData.PointerId, PtrData.X, PtrData.Y);
      TEv.vPtr.push_back(PtrData);
   }

   auto PringGestState = [](const std::string& sName, ndk_helper::GESTURE_STATE nState) {
      switch(nState)
      {
         case ndk_helper::GESTURE_STATE_NONE: printf("%s: GESTURE_STATE_NONE", sName.c_str()); break;
         case ndk_helper::GESTURE_STATE_START: printf("%s: GESTURE_STATE_START", sName.c_str()); break;
         case ndk_helper::GESTURE_STATE_MOVE: printf("%s: GESTURE_STATE_MOVE", sName.c_str()); break;
         case ndk_helper::GESTURE_STATE_END: printf("%s: GESTURE_STATE_END", sName.c_str()); break;
         case ndk_helper::GESTURE_STATE_ACTION: printf("%s: GESTURE_STATE_ACTION", sName.c_str()); break;
         default: break;
      }
   };

   auto             TapState       = m_TapDetector.Detect(&TEv);
   auto             DoubleTapState = m_DoubletapDetector.Detect(&TEv);
   auto             PinchState     = m_PinchDetector.Detect(&TEv);
   auto             DragState      = m_DragDetector.Detect(&TEv);
   ndk_helper::Vec2 p1, p2;
   if(TapState != ndk_helper::GESTURE_STATE_NONE)
   {
      PringGestState("TapState", TapState);
      printf("\n");
   }

   if(DoubleTapState != ndk_helper::GESTURE_STATE_NONE)
   {
      PringGestState("DoubleTapState", DoubleTapState);
      printf("\n");
   }

   if(PinchState != ndk_helper::GESTURE_STATE_NONE)
   {
      PringGestState("PinchState", PinchState);
      m_PinchDetector.GetPointers(p1, p2);
      printf("{%.3f %.3f}", p1.x_, p1.y_);
      printf("{%.3f %.3f}", p2.x_, p2.y_);
      printf("\n");
   }

   if(DragState != ndk_helper::GESTURE_STATE_NONE)
   {
      PringGestState("DragState", DragState);
      m_DragDetector.GetPointer(p1);
      printf("{%.3f %.3f}", p1.x_, p1.y_);
      printf("\n");
   }
   return true;
}

uint32_t CTouchEvent::GetEventID() { return 0xAA00; }

std::shared_ptr<IEventReceiver> CTouchEvent::GetEvShared() { return shared_from_this(); }
