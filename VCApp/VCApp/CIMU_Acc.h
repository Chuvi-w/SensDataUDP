#pragma once
#include "CIMUBase.h"

class CIMUAcc : public CBaseIMUSensor
{
 private:
   CIMUAcc() : CBaseIMUSensor(ASENSOR_TYPE_ACCELEROMETER, false, 0) {}

 public:
   using PTR = std::shared_ptr<CIMUAcc>;

   static PTR Create() { return PTR(new CIMUAcc); }
};

class CIMUGyr : public CBaseIMUSensor
{
 private:
   CIMUGyr() : CBaseIMUSensor(ASENSOR_TYPE_GYROSCOPE_UNCALIBRATED, false, 1) {}

 public:
   using PTR = std::shared_ptr<CIMUGyr>;

   static PTR Create() { return PTR(new CIMUGyr); }
};

class CIMUMag : public CBaseIMUSensor
{
 private:
   CIMUMag() : CBaseIMUSensor(ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED, false, 2)
   {
      m_bCalibrated = false;
      m_bFirstFrame = true;
   }

 public:
   using PTR = std::shared_ptr<CIMUMag>;

   static PTR Create() { return PTR(new CIMUMag); }

   void         AddCalibrationFrame(const CIMUFrame& fr);
   virtual bool PreAddFrame(CIMUFrame& fr) override;
   void         Calibrate() override;

   virtual void OnReset() override;

 private:
   Vec3D m_MinVal, m_MaxVal;
   bool  m_bCalibrated;
   bool  m_bFirstFrame;
};