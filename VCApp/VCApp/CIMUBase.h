#pragma once
#include "CTimeStamp.h"
#include "EvCommon.h"
#include <MatConv.h>
#include <mutex>

typedef enum IMUType_e
{
   ASENSOR_TYPE_INVALID                     = -1,
   ASENSOR_TYPE_ACCELEROMETER               = 1,
   ASENSOR_TYPE_MAGNETIC_FIELD              = 2,
   ASENSOR_TYPE_GYROSCOPE                   = 4,
   ASENSOR_TYPE_LIGHT                       = 5,
   ASENSOR_TYPE_PRESSURE                    = 6,
   ASENSOR_TYPE_PROXIMITY                   = 8,
   ASENSOR_TYPE_GRAVITY                     = 9,
   ASENSOR_TYPE_LINEAR_ACCELERATION         = 10,
   ASENSOR_TYPE_ROTATION_VECTOR             = 11,
   ASENSOR_TYPE_RELATIVE_HUMIDITY           = 12,
   ASENSOR_TYPE_AMBIENT_TEMPERATURE         = 13,
   ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED = 14,
   ASENSOR_TYPE_GAME_ROTATION_VECTOR        = 15,
   ASENSOR_TYPE_GYROSCOPE_UNCALIBRATED      = 16,
   ASENSOR_TYPE_SIGNIFICANT_MOTION          = 17,
   ASENSOR_TYPE_STEP_DETECTOR               = 18,
   ASENSOR_TYPE_STEP_COUNTER                = 19,
   ASENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR = 20,
   ASENSOR_TYPE_HEART_RATE                  = 21,
   ASENSOR_TYPE_POSE_6DOF                   = 28,
   ASENSOR_TYPE_STATIONARY_DETECT           = 29,
   ASENSOR_TYPE_MOTION_DETECT               = 30,
   ASENSOR_TYPE_HEART_BEAT                  = 31,
   ASENSOR_TYPE_LOW_LATENCY_OFFBODY_DETECT  = 34,
   ASENSOR_TYPE_ACCELEROMETER_UNCALIBRATED  = 35,
} IMUType_t;

typedef enum IMUAccuracy_e
{
   ASENSOR_STATUS_NO_CONTACT      = -1,
   ASENSOR_STATUS_UNRELIABLE      = 0,
   ASENSOR_STATUS_ACCURACY_LOW    = 1,
   ASENSOR_STATUS_ACCURACY_MEDIUM = 2,
   ASENSOR_STATUS_ACCURACY_HIGH   = 3
} IMUAccuracy_t;

class CIMUFrame
{
 public:
   CIMUFrame(const CommonPacketData_t& Com, const CTimeStampNS& ImuTS, const Vec3D& Val, const Vec3D& Unc = Vec3D()) : m_Com(Com), m_iTS(ImuTS), m_Val(Val), m_Uncalb(Unc) {}

   Vec3D& GetVal() { return m_Val; }

   Vec3D        GetVal() const { return m_Val; }
   CTimeStampNS GetIMUTime() const { return m_iTS; }

 private:
   CommonPacketData_t m_Com;
   CTimeStampNS       m_iTS;
   Vec3D              m_Val;
   Vec3D              m_Uncalb;
};

class CBaseIMUSensor
{
 protected:
   CBaseIMUSensor(IMUType_t Type, bool bUncalib, int nOutPos);

 public:
   using PTR = std::shared_ptr<CBaseIMUSensor>;

   float GetMaxRange() const;
   float GetRes() const;

   IMUType_t GetType() const;

   void AddFrame(const CIMUFrame& Fr, float flResolution, float flMaxRange);

   void GetAndPrintSKO(const CIMUFrame& Fr) const;
   void ResetFrames();

   virtual bool PreAddFrame(CIMUFrame& fr);
   virtual void OnReset() {}
   virtual void Calibrate() {}

   Vec3D GetAvg(size_t Offset = 0, size_t nCount = -1) const;

   CTimeStampNS GetLastFrameTime() const { return m_CurFrameTime; }

   CTimeStampNS GetFrameTimeDiff() const { return m_FrameTimeDiff; }

   bool HaveOneFrame() const { return m_flMaxRange != 0.0 && m_FlRes != 0.0; }
   Vec3D GetLastFrame() const
   {
      return m_vFrames.back().GetVal();
   }

   bool IsProcessed() const
   {
      return m_bProcessed;
   }
   void SetProcessed()
   {
      m_bProcessed = true;
   }
 private:
   std::mutex             m_FrMux;
   std::list<CIMUFrame> m_vFrames;
   IMUType_t              m_ImuType;
   size_t                 m_SkipCount;
   int                    m_OutPos;
   bool                   m_bHaveUncalibrated;
   float                  m_flMaxRange;
   float                  m_FlRes;

   CTimeStampNS m_FrameTimeDiff;
   CTimeStampNS m_CurFrameTime;

   bool m_bProcessed;
};
