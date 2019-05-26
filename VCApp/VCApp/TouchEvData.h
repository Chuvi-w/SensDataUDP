#pragma once
#include "EvCommon.h"
#include <map>
#include <memory>
#include "CTimeStamp.h"
typedef struct PointerData_s
{
   jint   nID;
   jint   PointerId;
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
} PointerData_t;

typedef struct MotionEvent_s
{
   //    jlong   IsEndian;
   //    jlong   PacketID;
   jlong  EventTime;
   jlong  DownTime;
   jint   Action;
   jint   ActionMasked;
   jfloat XPrecision;
   jfloat YPrecision;
   jfloat RawX;
   jfloat RawY;
   jint   ActionIndex;
   jint   PointerCount;
   jint   ViewW;
   jint   ViewH;
} MotionEvent_t;

typedef struct TouchEvent_s
{
   MotionEvent_t              MotEv;
   std::vector<PointerData_t> vPtr;
} TouchEvent_t;


class CTouchPointer:public std::enable_shared_from_this<CTouchPointer>
{
public:
   CTouchPointer(const PointerData_t &pData);
   CTouchPointer(const CTouchPointer &pOther);
   ~CTouchPointer();
   int   GetID() const
   {
      return m_Ptr.nID;
   }
   int   GetPointerId() const
   {
      return m_Ptr.PointerId;
   }
   float GetX() const
   {
      return m_Ptr.X;
   }
   float GetY() const
   {
      return m_Ptr.Y;
   }
   int   GetToolType() const
   {
      return m_Ptr.ToolType;
   }
   float GetSize() const
   {
      return m_Ptr.Size;
   }
   float GetToolMajor() const
   {
      return m_Ptr.ToolMajor;
   }
   float GetToolMinor() const
   {
      return m_Ptr.ToolMinor;
   }
   float GetTouchMajor() const
   {
      return m_Ptr.TouchMajor;
   }
   float GetTouchMinor() const
   {
      return m_Ptr.TouchMinor;
   }
   float GetPressure() const
   {
      return m_Ptr.Pressure;
   }
   float GetOrientation() const
   {
      return m_Ptr.Orientation;
   }
private:
   PointerData_t m_Ptr;
};



class CMotionEvent : public std::enable_shared_from_this<CMotionEvent>
{
public:
   CMotionEvent(const MotionEvent_t &MEv);
   CMotionEvent(const CMotionEvent &pOther);

   bool AddPointer(const std::shared_ptr<CTouchPointer> TouchPtr);
   std::shared_ptr<CTouchPointer> FindPtrID(int nID) const;
   std::shared_ptr<CTouchPointer> GetPtr(int nID) const;
   bool IsValidPtrCount() const
   {
      return m_Ptr.size() == m_Ev.PointerCount;
   }
   CTimeStampNS GetEventTime() const
   {
      return m_Ev.EventTime;
   }
   CTimeStampNS GetDownTime() const
   {
      return m_Ev.DownTime;
   }
   int GetAction() const
   {
      return m_Ev.Action;
   }
   int GetActionMasked() const
   {
      return m_Ev.ActionMasked;
   }
   float GetXPrecision() const
   {
      return m_Ev.XPrecision;
   }
   float GetYPrecision() const
   {
      return m_Ev.YPrecision;
   }
   float GetRawX() const
   {
      return m_Ev.RawX;
   }
   float GetRawY() const
   {
      return m_Ev.RawY;
   }
   int GetActionIndex() const
   {
      return m_Ev.ActionIndex;
   }
   int GetPointerCount() const
   {
      return m_Ev.PointerCount;
   }
   int GetViewW() const
   {
      return m_Ev.ViewW;
   }
   int GetViewH() const
   {
      return m_Ev.ViewH;
   }

private:
   MotionEvent_t m_Ev;
   std::vector<std::shared_ptr<CTouchPointer>> m_Ptr;

};
