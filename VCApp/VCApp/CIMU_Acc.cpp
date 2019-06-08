#include "CIMU_Acc.h"

void CIMUMag::AddCalibrationFrame(const CIMUFrame& fr)
{
   auto& Val = fr.GetVal();

   if(m_bFirstFrame)
   {
      m_bFirstFrame = false;
      m_MaxVal      = Val;
      m_MinVal      = Val;
      return;
   }

   if(Val.x > m_MaxVal.x)
   {
      m_MaxVal.x = Val.x;
   }
   if(Val.y > m_MaxVal.y)
   {
      m_MaxVal.y = Val.y;
   }
   if(Val.z > m_MaxVal.z)
   {
      m_MaxVal.z = Val.z;
   }

   if(Val.x < m_MinVal.x)
   {
      m_MinVal.x = Val.x;
   }
   if(Val.y < m_MinVal.y)
   {
      m_MinVal.y = Val.y;
   }
   if(Val.z < m_MinVal.z)
   {
      m_MinVal.z = Val.z;
   }
}

bool CIMUMag::PreAddFrame(CIMUFrame& fr)
{
   if(!m_bCalibrated)
   {
      AddCalibrationFrame(fr);
   }
   return true;
}

void CIMUMag::Calibrate() { printf(""); }

void CIMUMag::OnReset()
{
   m_bCalibrated = false;
   m_MinVal      = NullVec3D;
   m_MaxVal      = NullVec3D;
   m_bFirstFrame = true;
}
