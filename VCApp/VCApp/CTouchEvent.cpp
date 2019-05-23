#include "CTouchEvent.h"
#include <string.h>
#include <stdio.h>
#include "CDataPacket.h"

CTouchEvent::CTouchEvent() {}

CTouchEvent::~CTouchEvent() {}

bool CTouchEvent::ParseEvent(CDataPacket &pPacket)
{
   if(pPacket.GetDataSize() < sizeof(MotionEvent_t))
   {
      return false;
   }
   TouchEvent_t         TEv;
   PointerData_t PtrData;
   if(
      !pPacket.GetData(TEv.MotEv.EventTime) ||
      !pPacket.GetData(TEv.MotEv.DownTime) ||
      !pPacket.GetData(TEv.MotEv.Action) ||
      !pPacket.GetData(TEv.MotEv.ActionMasked) ||
      !pPacket.GetData(TEv.MotEv.XPrecision) ||
      !pPacket.GetData(TEv.MotEv.YPrecision) ||
      !pPacket.GetData(TEv.MotEv.RawX) ||
      !pPacket.GetData(TEv.MotEv.RawY) ||
      !pPacket.GetData(TEv.MotEv.ActionIndex) ||
      !pPacket.GetData(TEv.MotEv.PointerCount)
      )
   {
      return false;
   }


   if(pPacket.GetRemainDataSize() < TEv.MotEv.PointerCount * sizeof(PointerData_t))
   {
      printf("Wrong DataSize\n");
      return false;
   }
  
   for(int i = 0; i < TEv.MotEv.PointerCount; i++)
   {
      if(
         !pPacket.GetData(PtrData.nID) ||
         !pPacket.GetData(PtrData.PointerId) ||
         !pPacket.GetData(PtrData.X) ||
         !pPacket.GetData(PtrData.Y) ||
         !pPacket.GetData(PtrData.ToolType) ||
         !pPacket.GetData(PtrData.Size) ||
         !pPacket.GetData(PtrData.ToolMajor) ||
         !pPacket.GetData(PtrData.ToolMinor) ||
         !pPacket.GetData(PtrData.TouchMajor) ||
         !pPacket.GetData(PtrData.TouchMinor) ||
         !pPacket.GetData(PtrData.Pressure) ||
         !pPacket.GetData(PtrData.Orientation)
         )
      {
         return false;
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


std::shared_ptr<IEventReceiver> CTouchEvent::GetEvShared() { return shared_from_this(); }
