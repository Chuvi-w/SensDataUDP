#include "CTouchEvent.h"
#include <string.h>
#include <stdio.h>



CTouchEvent::CTouchEvent()
{

}

CTouchEvent::~CTouchEvent()
{

}


bool CTouchEvent::ParseEvent(const void *pData, size_t nDataSize, bool bEndian)
{
   if (nDataSize < sizeof(MotionEvent_t))
   {
      return false;
   }
   TouchEvent_t TEv;
   const PointerData_t *pPtrData;
   PointerData_t PtrData;
   memcpy(&TEv.MotEv, pData, sizeof(MotionEvent_t));
   if (!bEndian)
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
   if (nDataSize < sizeof(MotionEvent_t) + TEv.MotEv.PointerCount*sizeof(PointerData_t))
   {
      printf("Wrong DataSize\n");
      return false;
   }
   pPtrData = reinterpret_cast<const PointerData_t*>((size_t)pData + sizeof(MotionEvent_t));
   printf("PC=%i\n", TEv.MotEv.PointerCount);
   for (int i = 0; i < TEv.MotEv.PointerCount; i++)
   {
      memcpy(&PtrData, &pPtrData[i], sizeof(PointerData_t));
      if (!bEndian)
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

      printf("%i %i %.4f %.4f\n", PtrData.nID, PtrData.PointerId, PtrData.X, PtrData.Y);
      TEv.vPtr.push_back(PtrData);

   }



   return true;
}

uint32_t CTouchEvent::GetEventID()
{
   return 0xAA00;
}

std::shared_ptr<IEventReceiver> CTouchEvent::GetEvShared()
{
   return shared_from_this();
}
