#include "CTouchEvent.h"
#include <string.h>
#include <stdio.h>
#include "CDataPacket.h"
#include "AndroidInput.h"

CTouchEvent::CTouchEvent() : IEventReceiver(TOUCH_EV_ID) {}

CTouchEvent::~CTouchEvent() {}

bool CTouchEvent::ParseEvent(CDataPacket& pPacket)
{
   if(pPacket.GetDataSize() < sizeof(MotionEvent_t))
   {
      return false;
   }

   std::shared_ptr<CMotionEvent> MotEvPtr;
   // TouchEvent_t         TEv;
   MotionEvent_t MotEv;
   PointerData_t PtrData;
   if(!pPacket.GetData(MotEv.EventTime) || !pPacket.GetData(MotEv.DownTime) || !pPacket.GetData(MotEv.Action) || !pPacket.GetData(MotEv.ActionMasked) || !pPacket.GetData(MotEv.XPrecision) || !pPacket.GetData(MotEv.YPrecision) || !pPacket.GetData(MotEv.RawX) || !pPacket.GetData(MotEv.RawY) || !pPacket.GetData(MotEv.ActionIndex) || !pPacket.GetData(MotEv.PointerCount) || !pPacket.GetData(MotEv.ViewW) || !pPacket.GetData(MotEv.ViewH))
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
      if(!pPacket.GetData(PtrData.nID) || !pPacket.GetData(PtrData.PointerId) || !pPacket.GetData(PtrData.X) || !pPacket.GetData(PtrData.Y) || !pPacket.GetData(PtrData.ToolType) || !pPacket.GetData(PtrData.Size) || !pPacket.GetData(PtrData.ToolMajor) || !pPacket.GetData(PtrData.ToolMinor) || !pPacket.GetData(PtrData.TouchMajor) || !pPacket.GetData(PtrData.TouchMinor) || !pPacket.GetData(PtrData.Pressure) || !pPacket.GetData(PtrData.Orientation))
      {
         return false;
      }
      MotEvPtr->AddPointer(std::make_shared<CTouchPointer>(PtrData));
   }
   if(!MotEvPtr->IsValidPtrCount())
   {
      return false;
   }
#if 1
   auto PringGestState = [](const std::string& sName, GESTURE_STATE nState) {
      switch(nState)
      {
         case GESTURE_STATE_NONE: printf("%s: GESTURE_STATE_NONE", sName.c_str()); break;
         case GESTURE_STATE_START: printf("%s: GESTURE_STATE_START", sName.c_str()); break;
         case GESTURE_STATE_MOVE: printf("%s: GESTURE_STATE_MOVE", sName.c_str()); break;
         case GESTURE_STATE_END: printf("%s: GESTURE_STATE_END", sName.c_str()); break;
         case GESTURE_STATE_ACTION: printf("%s: GESTURE_STATE_ACTION", sName.c_str()); break;
         default: break;
      }
   };

   auto TapState       = m_TapDetector.Detect(MotEvPtr);
   auto DoubleTapState = m_DoubletapDetector.Detect(MotEvPtr);
   auto PinchState     = m_PinchDetector.Detect(MotEvPtr);
   auto DragState      = m_DragDetector.Detect(MotEvPtr);
   Vec2 p1, p2;
   if(TapState != GESTURE_STATE_NONE)
   {
      PringGestState("TapState", TapState);
      printf("\n");
   }

   if(DoubleTapState != GESTURE_STATE_NONE)
   {
      PringGestState("DoubleTapState", DoubleTapState);
      printf("\n");
   }

   if(PinchState != GESTURE_STATE_NONE)
   {
      PringGestState("PinchState", PinchState);
      m_PinchDetector.GetPointers(p1, p2);
      printf("{%.3f %.3f}", p1.x, p1.y);
      printf("{%.3f %.3f}", p2.x, p2.y);
      printf("\n");
   }

   if(DragState != GESTURE_STATE_NONE)
   {
      PringGestState("DragState", DragState);
      m_DragDetector.GetPointer(p1);
      printf("{%.3f %.3f}", p1.x, p1.y);

      printf("{%i %i}", MotEvPtr->GetViewW(), MotEvPtr->GetViewH());
      printf("\n");
   }

   int32_t      action = MotEvPtr->GetAction();
   unsigned int flags  = action & AMOTION_EVENT_ACTION_MASK;
   auto         aMask  = MotEvPtr->GetActionMasked();

   printf("%i %i\n", flags, aMask);
#endif
   return true;
}

std::shared_ptr<IEventReceiver> CTouchEvent::GetEvShared() { return shared_from_this(); }
