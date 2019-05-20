#pragma once
#include "EvCommon.h"
typedef struct PointerData_s
{
   jint  nID;
   jint  PointerId;
   jfloat X;
   jfloat Y;
   jint   ToolType;
   jfloat Size;
   jfloat ToolMajor;
   jfloat ToolMinor;
   jfloat TouchMajor;
   jfloat TouchMinor;
   jfloat Pressure;
   jfloat Orientation;
}PointerData_t;
typedef  struct MotionEvent_s
{
   //    jlong   IsEndian;
   //    jlong   PacketID;
   jlong   EventTime;
   jlong   DownTime;
   jint    Action;
   jint    ActionMasked;
   jfloat  XPrecision;
   jfloat  YPrecision;
   jfloat  RawX;
   jfloat  RawY;
   jint    ActionIndex;
   jint    PointerCount;
}MotionEvent_t;


typedef struct TouchEvent_s
{
   MotionEvent_t MotEv;
   std::vector<PointerData_t> vPtr;
}TouchEvent_t;

class CTouchEvent :public IEventReceiver
{
private:
   CTouchEvent();
   using PTR = std::shared_ptr<CTouchEvent>;
public:
   static PTR Create()
   {

      return std::shared_ptr<CTouchEvent>(new CTouchEvent);
   }
   ~CTouchEvent();
   virtual bool ParseEvent(const void *pData, size_t nDataSize, bool bEndian) override;
   virtual uint32_t GetEventID() override;
   virtual std::shared_ptr<IEventReceiver> GetEvShared() override;

  

};

