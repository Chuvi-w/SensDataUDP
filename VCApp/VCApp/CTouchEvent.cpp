#include "CTouchEvent.h"
#include <string.h>
#include <stdio.h>
#include "CDataPacket.h"

CTouchEvent::CTouchEvent():IEventReceiver(TOUCH_EV_ID) {}

CTouchEvent::~CTouchEvent() {}

bool CTouchEvent::ParseEvent(CDataPacket &pPacket)
{
   if(pPacket.GetDataSize() < sizeof(MotionEvent_t))
   {
      return false;
   }
   
   std::shared_ptr<CMotionEvent> MotEvPtr;
   //TouchEvent_t         TEv;
   MotionEvent_t MotEv;
   PointerData_t PtrData;
   if(
      !pPacket.GetData(MotEv.EventTime) ||
      !pPacket.GetData(MotEv.DownTime) ||
      !pPacket.GetData(MotEv.Action) ||
      !pPacket.GetData(MotEv.ActionMasked) ||
      !pPacket.GetData(MotEv.XPrecision) ||
      !pPacket.GetData(MotEv.YPrecision) ||
      !pPacket.GetData(MotEv.RawX) ||
      !pPacket.GetData(MotEv.RawY) ||
      !pPacket.GetData(MotEv.ActionIndex) ||
      !pPacket.GetData(MotEv.PointerCount)||
      !pPacket.GetData(MotEv.ViewW)||
      !pPacket.GetData(MotEv.ViewH)
      )
   {
      return false;
   }

   
   if(pPacket.GetRemainDataSize() < MotEv.PointerCount * sizeof(PointerData_t))
   {
      printf("Wrong DataSize\n");
      return false;
   }
   MotEvPtr = std::make_shared<CMotionEvent>(MotEv);

   for(int i = 0; i < MotEv.PointerCount; i++)
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
      MotEvPtr->AddPointer(std::make_shared<CTouchPointer>(PtrData));
   }
   if (!MotEvPtr->IsValidPtrCount())
   {
      return false;
   }
#if 0
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
   Vec2 p1, p2;
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
      printf("{%.3f %.3f}", p1.x, p1.y);
      printf("{%.3f %.3f}", p2.x, p2.y);
      printf("\n");
   }

   if(DragState != ndk_helper::GESTURE_STATE_NONE)
   {
      PringGestState("DragState", DragState);
      m_DragDetector.GetPointer(p1);
      printf("{%.3f %.3f}", p1.x, p1.y);

      printf("{%i %i}", TEv.MotEv.ViewW,TEv.MotEv.ViewH);
      printf("\n");
   }

#endif
   return true;
}


std::shared_ptr<IEventReceiver> CTouchEvent::GetEvShared() { return shared_from_this(); }
