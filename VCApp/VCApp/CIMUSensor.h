#pragma once
#include <stdint.h>
#include "MathLib/MathLib3D.h"
#include <list>

using Vec3D = MATHLIB3D::Vector3D;

const Vec3D NullVec3D = Vec3D(0.0, 0.0, 0.0);

typedef enum SensTypes
{
   TYPE_ACCELEROMETER = 1,
   TYPE_MAGNETIC_FIELD = 2,
   TYPE_GYROSCOPE = 4,
   TYPE_MAGNETIC_FIELD_UNCALIBRATED = 14,
   TYPE_GYROSCOPE_UNCALIBRATED = 16,
   TYPE_ACCELEROMETER_UNCALIBRATED = 35,
}SensTypes;

inline Vec3D DEG2RAD(Vec3D val)
{
   return val * MATHLIB3D::MathUtils::PI / 180.0;
}

inline Vec3D RAD2DEG(Vec3D val)
{
   return val * 180.0 / MATHLIB3D::MathUtils::PI;
}

inline double DEG2RAD(double val)
{
   return val * MATHLIB3D::MathUtils::PI / 180.0;
}
inline double RAD2DEG(double val)
{
   return val * 180.0 / MATHLIB3D::MathUtils::PI;
}

class CTimeStampNS
{
public:
   CTimeStampNS(uint64_t Ts) :m_TS(Ts)
   {

   }
   bool operator <(const CTimeStampNS &pOther) const
   {
      return m_TS < pOther.m_TS;
   }

   uint64_t m_TS;
};

class CSensorFrame
{
public:
   CSensorFrame(const Vec3D &vData, CTimeStampNS TimeStamp) :m_RecvTime(TimeStamp), m_Vec(vData)
   {

   }
   bool operator <(const CSensorFrame &pOther) const
   {
      return m_RecvTime < pOther.m_RecvTime;
   }
   
   CTimeStampNS m_RecvTime;
   Vec3D m_Vec;
 
};

class CSensorFrameUncalibrated :public CSensorFrame
{
public:
   CSensorFrameUncalibrated(const Vec3D &vData, CTimeStampNS TimeStamp, const Vec3D &vAddData = NullVec3D) :CSensorFrame(vData, TimeStamp), m_AddData(vAddData)
   {

   }
   Vec3D m_AddData;
};

class CBaseIMUSensor
{
  
public:
   CBaseIMUSensor(const std::string &sName, SensTypes nType1, SensTypes nType2) :m_Name(sName), m_Type1(nType1), m_Type2(nType2)
   {

   }
   void Reset()
   {
      m_Data.clear();
      m_UData.clear();
   }
   bool AddFrame(const void *pData, size_t nDataSize, CTimeStampNS nTimeStamp, float flRes, float flMaxRange);

   bool HaveData() const
   {
      return !m_Data.empty();
   }

   bool HaveUncalibrated() const
   {
      return !m_UData.empty();
   }
   bool CheckSensorType(SensTypes nType);
   virtual bool OnNewDataReceived(Vec3D &vData, Vec3D &vAddData, CTimeStampNS TimeStamp, bool bHaveAddData);
   virtual bool OnNewDataAdded(bool bUncalibrated=false);
   std::list<CSensorFrame> GetData() const
   {
      return m_Data;
   }

   size_t GetDataCount() const
   {
      return m_Data.size();
   }
   Vec3D GetAvg() const
   {

      Vec3D vRes = NullVec3D;
      if (!m_Data.empty())
      {
         for (const auto &v : m_Data)
         {
            vRes += v.m_Vec;
         }
         vRes /= m_Data.size();
      }

      return vRes;
   }

   std::list<CSensorFrameUncalibrated> GetDataUncalib() const
   {
      return m_UData;
   }
   size_t GetDataCountUncalib() const
   {
      return m_UData.size();
   }
   Vec3D GetAvgUncalib() const
   {

      Vec3D vRes = NullVec3D;
      if (!m_UData.empty())
      {
         for (const auto &v : m_UData)
         {
            vRes += v.m_Vec;
         }
         vRes /= m_Data.size();
      }

      return vRes;
   }

   void Calibrate();
   float m_flRes;
   float m_flMaxRange;
private:

   std::string m_Name;
   SensTypes m_Type1, m_Type2;
   std::list<CSensorFrame> m_Data;
   std::list<CSensorFrameUncalibrated> m_UData;
  
};



class CACCSensor :public CBaseIMUSensor
{
public:
   CACCSensor() :CBaseIMUSensor("Acc",TYPE_ACCELEROMETER,TYPE_ACCELEROMETER_UNCALIBRATED)
   {

   }
   virtual bool OnNewDataAdded(bool bUncalibrated = false) override; 
};


class CGyroSensor :public CBaseIMUSensor
{
public:
   CGyroSensor() :CBaseIMUSensor("Gyr",TYPE_GYROSCOPE,TYPE_GYROSCOPE_UNCALIBRATED)
   {

   }
   virtual bool OnNewDataAdded(bool bUncalibrated = false) override;

};


class CMagSensor :public CBaseIMUSensor
{
public:
   CMagSensor() :CBaseIMUSensor("Mag",TYPE_MAGNETIC_FIELD,TYPE_MAGNETIC_FIELD_UNCALIBRATED)
   {

   }
   virtual bool OnNewDataAdded(bool bUncalibrated = false) override;

};