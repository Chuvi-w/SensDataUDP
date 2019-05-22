#pragma once
#include "EvCommon.h"
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
} MotionEvent_t;

typedef struct TouchEvent_s
{
   MotionEvent_t              MotEv;
   std::vector<PointerData_t> vPtr;
} TouchEvent_t;
