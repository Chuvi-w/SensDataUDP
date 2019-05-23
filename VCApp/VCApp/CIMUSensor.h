#pragma once
#include <stdint.h>

#include <list>
#include "CTimeStamp.h"
#include "MatConv.h"






class CSensorFrame
{
 public:
   CSensorFrame(const Vec3D& vData, CTimeStampNS TimeStamp) : m_RecvTime(TimeStamp), m_Vec(vData) {}
   bool operator<(const CSensorFrame& pOther) const { return m_RecvTime < pOther.m_RecvTime; }

   CTimeStampNS m_RecvTime;
   Vec3D        m_Vec;
};

class CSensorFrameUncalibrated : public CSensorFrame
{
 public:
   CSensorFrameUncalibrated(const Vec3D& vData, CTimeStampNS TimeStamp, const Vec3D& vAddData = NullVec3D) : CSensorFrame(vData, TimeStamp), m_AddData(vAddData) {}
   Vec3D m_AddData;
};

class CBaseIMUSensorOLD
{

 public:
   CBaseIMUSensorOLD(const std::string& sName, SensTypes nType1, SensTypes nType2) : m_Name(sName), m_Type1(nType1), m_Type2(nType2) {}
   void Reset()
   {
      m_Data.clear();
      m_UData.clear();
   }
   bool AddFrame(const void* pData, size_t nDataSize, CTimeStampNS nTimeStamp, float flRes, float flMaxRange);

   bool HaveData() const { return !m_Data.empty(); }

   bool                    HaveUncalibrated() const { return !m_UData.empty(); }
   bool                    CheckSensorType(SensTypes nType);
   virtual bool            OnNewDataReceived(Vec3D& vData, Vec3D& vAddData, CTimeStampNS TimeStamp, bool bHaveAddData);
   virtual bool            OnNewDataAdded(bool bUncalibrated = false);
   std::list<CSensorFrame> GetData() const { return m_Data; }

   size_t GetDataCount() const { return m_Data.size(); }
   Vec3D  GetAvg() const
   {

      Vec3D vRes = NullVec3D;
      if(!m_Data.empty())
      {
         for(const auto& v : m_Data)
         {
            vRes += v.m_Vec;
         }
         vRes /= m_Data.size();
      }

      return vRes;
   }

   std::list<CSensorFrameUncalibrated> GetDataUncalib() const { return m_UData; }
   size_t                              GetDataCountUncalib() const { return m_UData.size(); }
   Vec3D                               GetAvgUncalib() const
   {

      Vec3D vRes = NullVec3D;
      if(!m_UData.empty())
      {
         for(const auto& v : m_UData)
         {
            vRes += v.m_Vec;
         }
         vRes /= m_Data.size();
      }

      return vRes;
   }

   void  Calibrate();
   float m_flRes;
   float m_flMaxRange;

 private:
   std::string                         m_Name;
   SensTypes                           m_Type1, m_Type2;
   std::list<CSensorFrame>             m_Data;
   std::list<CSensorFrameUncalibrated> m_UData;
};

class CACCSensor : public CBaseIMUSensorOLD
{
 public:
   CACCSensor() : CBaseIMUSensorOLD("Acc", TYPE_ACCELEROMETER, TYPE_ACCELEROMETER_UNCALIBRATED) {}
   virtual bool OnNewDataAdded(bool bUncalibrated = false) override;
};

class CGyroSensor : public CBaseIMUSensorOLD
{
 public:
   CGyroSensor() : CBaseIMUSensorOLD("Gyr", TYPE_GYROSCOPE, TYPE_GYROSCOPE_UNCALIBRATED) {}
   virtual bool OnNewDataAdded(bool bUncalibrated = false) override;
};

class CMagSensor : public CBaseIMUSensorOLD
{
 public:
   CMagSensor() : CBaseIMUSensorOLD("Mag", TYPE_MAGNETIC_FIELD, TYPE_MAGNETIC_FIELD_UNCALIBRATED) {}
   virtual bool OnNewDataAdded(bool bUncalibrated = false) override;
};