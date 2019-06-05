#include "CIMUBase.h"
#include "ConsoleTools.h"
#include <mutex>




CBaseIMUSensor::CBaseIMUSensor(IMUType_t Type, bool bUncalib, int nOutPos):m_ImuType(Type), m_bHaveUncalibrated(bUncalib), m_OutPos(nOutPos)
{
   ResetFrames();
}

void CBaseIMUSensor::SetResolution(float Res, float Max)
{
   m_FlRes = Res;
   m_flMaxRange = Max;
}

float CBaseIMUSensor::GetMaxRange() const
{
   return m_flMaxRange;
}

float CBaseIMUSensor::GetRes() const
{
   return m_FlRes;
}

IMUType_t CBaseIMUSensor::GetType() const
{
   return m_ImuType;
}

void CBaseIMUSensor::AddFrame(const CIMUFrame& Fr)
{
   std::lock_guard <std::mutex> _Guard(m_FrMux);
   if(m_SkipCount++ < 200)
   {
      return;
   }
   auto FrameData = Fr;
   if(PreAddFrame(FrameData))
   {
      m_vFrames.push_back(FrameData);
   }

  
   int nOutLine;
   if(m_OutPos < 0)
   {
      return;
   }
   nOutLine = 12 + m_OutPos * 4;

   Vec3D vAvg;
   try
   {
      vAvg = GetAvg(m_vFrames.size() - 5);
      //vAvg = GetAvg();
   }
   catch(...)
   {
      return;
   }
   Vec3D vCur = Fr.GetVal();

   Vec3D vSCOData = NullVec3D;
   Vec3D vSCOMin;
   float flRes = m_FlRes;
   auto Mag = vCur.GetMagnitude();
   for(const auto& v : m_vFrames)
   {
      vSCOData.x += pow((vAvg.x - v.GetVal().x), 2.0);
      vSCOData.y += pow((vAvg.y - v.GetVal().y), 2.0);
      vSCOData.z += pow((vAvg.z - v.GetVal().z), 2.0);
   }
   vSCOData.x = sqrt(vSCOData.x / (double)m_vFrames.size());
   vSCOData.y = sqrt(vSCOData.y / (double)m_vFrames.size());
   vSCOData.z = sqrt(vSCOData.z / (double)m_vFrames.size());
   vSCOMin = vSCOData / m_FlRes;
   Vec3D SpCoords;
   if(m_ImuType == ASENSOR_TYPE_ACCELEROMETER)
   {
      vAvg *= 100.0;
      vSCOData *= 100.0;
      vCur *= 100.0;
      flRes *= 100.0;
      Mag *= 100.0;
   }

   if(m_ImuType == ASENSOR_TYPE_GYROSCOPE_UNCALIBRATED)
   {
      vAvg = RAD2DEG(vAvg)*60.0;
      vSCOData = RAD2DEG(vSCOData)*60.0;
      vCur = RAD2DEG(vCur)*60.0;
      flRes = RAD2DEG(flRes)*60.0;
      Mag = RAD2DEG(Mag)*60.0;
   }
  
   SpCoords.x = vCur.GetSphericCoordinates(SpCoords.y, SpCoords.z);
   SpCoords.y = RAD2DEG(SpCoords.y);
   SpCoords.z = RAD2DEG(SpCoords.z);
   gotoxy(0, nOutLine);
   printf("%08u {%018.15f, %018.15f, %018.15f} ,{%018.15f, %018.15f, %018.15f}                   \n", m_vFrames.size(), vAvg.x, vAvg.y, vAvg.z, vSCOData.x, vSCOData.y, vSCOData.z);
   printf("%08u {%018.15f, %018.15f, %018.15f} ,{%018.15f, %018.15f, %018.15f}                   \n", m_vFrames.size(), vCur.x, vCur.y, vCur.z, vSCOMin.x, vSCOMin.y, vSCOMin.z);
   printf("%08u %018.15f %018.15f {%018.15f, %018.15f, %018.15f}                \n", m_vFrames.size(), flRes, Mag, SpCoords.x, SpCoords.y, SpCoords.z);
}

void CBaseIMUSensor::ResetFrames()
{
   std::lock_guard <std::mutex> _Guard(m_FrMux);
   OnReset();
   m_vFrames.clear();
   m_SkipCount = 0;
}

bool CBaseIMUSensor::PreAddFrame(CIMUFrame &fr)
{

   return true;
}


Vec3D CBaseIMUSensor::GetAvg(size_t Offset/*=0*/, size_t nCount/*=-1*/) const
{
   Vec3D Res = {0, 0, 0};
  
   if(Offset >= m_vFrames.size()||m_vFrames.empty())
   {
      throw std::exception("CBaseIMUSensor AVG overflow.");
   }

   if(nCount == -1)
   {
      nCount = (m_vFrames.size()) - Offset;
   }


   if(Offset + nCount > m_vFrames.size())
   {
      throw std::exception("CBaseIMUSensor AVG overflow.");
   }

   for(size_t i = Offset; i < Offset+nCount; i++)
   {
      Res += m_vFrames[i].GetVal();
   }

   Res /= double(nCount);

   return Res;

}
