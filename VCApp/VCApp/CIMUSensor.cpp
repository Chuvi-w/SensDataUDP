#include "CIMUSensor.h"
#include <algorithm>



bool CBaseIMUSensor::AddFrame(const void *pData, size_t nDataSize, CTimeStampNS nTimeStamp)
{

   const float *fData = reinterpret_cast<const float*>(pData);
   Vec3D vData, vAddData=NullVec3D;
  
   if (nDataSize != sizeof(float) * 3 && nDataSize != sizeof(float) * 6)
   {
      return false;
   }

   if (nDataSize == sizeof(float) * 3)
   {
      vData = Vec3D(fData[0], fData[1], fData[2]);
      if (OnNewDataReceived(vData, vAddData, nTimeStamp, false))
      {
         m_Data.push_back({ vData, nTimeStamp });
        // m_Data.sort();
         OnNewDataAdded(false);
      }
     
   }
   else
   if (nDataSize == sizeof(float) * 6)
   {
      vData = Vec3D(fData[0], fData[1], fData[2]);
      vAddData = Vec3D(fData[3], fData[4], fData[5]);
      if (OnNewDataReceived(vData, vAddData, nTimeStamp, true))
      {
         m_UData.push_back({ vData,nTimeStamp,vAddData });
         //m_UData.sort();
         OnNewDataAdded(true);
      }
     
   }
   else
   {
      return false;
   }
   return true;
}


bool CBaseIMUSensor::OnNewDataReceived(Vec3D &vData, Vec3D &vAddData, CTimeStampNS TimeStamp, bool bHaveAddData)
{
   return true;
}

bool CBaseIMUSensor::OnNewDataAdded(bool bUncalibrated)
{
   return true;
}

void CBaseIMUSensor::Calibrate()
{
  
   Vec3D dMin, dMax, Dif;
   dMin.x = (float)(0xFFFFFFFF);
   dMin.y = (float)(0xFFFFFFFF);
   dMin.z = (float)(0xFFFFFFFF);

   dMax *= 0.0;

   for (const auto &i : m_Data)
   {
      for (const auto &j : m_Data)
      {
         if (&i == &j)
         {
            continue;
         }
         Dif = i.m_Vec - j.m_Vec;
         Dif.x = abs(Dif.x);
         Dif.y = abs(Dif.y);
         Dif.z = abs(Dif.z);
         

         if (Dif.x < dMin.x&&Dif.x != 0.0)
         {
            dMin.x = Dif.x;
         }

         if (Dif.y < dMin.y&&Dif.y != 0.0)
         {
            dMin.y = Dif.y;
         }

         if (Dif.z < dMin.z &&Dif.z != 0.0)
         {
            dMin.z = Dif.z;
         }

         if (Dif.x > dMax.x)
         {
            dMax.x = Dif.x;
         }

         if (Dif.y > dMax.y)
         {
            dMax.y = Dif.y;
         }

         if (Dif.z > dMax.z)
         {
            dMax.z = Dif.z;
         }
      }
   }

   printf("");
}

bool CACCSensor::CheckSensorType(SensTypes nType)
{
   return nType == TYPE_ACCELEROMETER || nType == TYPE_ACCELEROMETER_UNCALIBRATED;
}


bool CACCSensor::OnNewDataAdded(bool bUncalibrated /*= false*/)
{
   return true;
   Vec3D vSCOData = NullVec3D;
   if (!bUncalibrated)
   {
      auto vAvgData = GetAvg();
      auto vCurData = GetData();
     
      for (const auto &v : vCurData)
      {
         vSCOData.x += pow((vAvgData.x - v.m_Vec.x), 2.0);
         vSCOData.y += pow((vAvgData.y - v.m_Vec.y), 2.0);
         vSCOData.z += pow((vAvgData.z - v.m_Vec.z), 2.0);
      }
      vSCOData.x = sqrt(vSCOData.x / (double)vCurData.size());
      vSCOData.y = sqrt(vSCOData.y / (double)vCurData.size());
      vSCOData.z = sqrt(vSCOData.z / (double)vCurData.size());
      vSCOData *= 100.0;

      printf("{%05u} {%.15f, %.15f, %.15f} ,{%.15f, %.15f, %.15f}\n", GetDataCount(), vAvgData.x, vAvgData.y, vAvgData.z, vSCOData.x, vSCOData.y, vSCOData.z);

   }

   return true;
}


bool CGyroSensor::CheckSensorType(SensTypes nType)
{
   return nType == TYPE_GYROSCOPE || nType == TYPE_GYROSCOPE_UNCALIBRATED;
}

bool CGyroSensor::OnNewDataAdded(bool bUncalibrated /*= false*/)
{
   return true;
   Vec3D vSCOData = NullVec3D;
   if (bUncalibrated)
   {
      auto vAvgData = GetAvgUncalib();
      auto vCurData = GetDataUncalib();

      for (const auto &v : vCurData)
      {
         vSCOData.x += pow((vAvgData.x - v.m_Vec.x), 2.0);
         vSCOData.y += pow((vAvgData.y - v.m_Vec.y), 2.0);
         vSCOData.z += pow((vAvgData.z - v.m_Vec.z), 2.0);
      }
      vSCOData.x = sqrt(vSCOData.x / (double)vCurData.size());
      vSCOData.y = sqrt(vSCOData.y / (double)vCurData.size());
      vSCOData.z = sqrt(vSCOData.z / (double)vCurData.size());
      //vSCOData *= 100.0;

     
      vAvgData = RAD2DEG(vAvgData);
      vSCOData = RAD2DEG(vSCOData);
      vAvgData *= 3600.0;
      vSCOData *= 3600.0;
      printf("{%05u} {%.15f, %.15f, %.15f} ,{%.15f, %.15f, %.15f}\n", GetDataCountUncalib(), vAvgData.x, vAvgData.y, vAvgData.z, vSCOData.x, vSCOData.y, vSCOData.z);

   }

   return true;
}

bool CMagSensor::CheckSensorType(SensTypes nType)
{
   return nType == TYPE_MAGNETIC_FIELD || nType == TYPE_MAGNETIC_FIELD_UNCALIBRATED;
}

bool CMagSensor::OnNewDataAdded(bool bUncalibrated /*= false*/)
{

   return true;
   Vec3D vSCOData = NullVec3D;
   if (bUncalibrated)
   {
      auto vAvgData = GetAvgUncalib();
      auto vCurData = GetDataUncalib();

      for (const auto &v : vCurData)
      {
         vSCOData.x += pow((vAvgData.x - v.m_Vec.x), 2.0);
         vSCOData.y += pow((vAvgData.y - v.m_Vec.y), 2.0);
         vSCOData.z += pow((vAvgData.z - v.m_Vec.z), 2.0);
      }
      vSCOData.x = sqrt(vSCOData.x / (double)vCurData.size());
      vSCOData.y = sqrt(vSCOData.y / (double)vCurData.size());
      vSCOData.z = sqrt(vSCOData.z / (double)vCurData.size());
      //vSCOData *= 100.0;


      //vAvgData = RAD2DEG(vAvgData);
     // vSCOData = RAD2DEG(vSCOData);
     // vAvgData *= 3600.0;
     // vSCOData *= 3600.0;
      printf("{%05u} {%.15f, %.15f, %.15f} ,{%.15f, %.15f, %.15f}\n", GetDataCountUncalib(), vAvgData.x, vAvgData.y, vAvgData.z, vSCOData.x, vSCOData.y, vSCOData.z);

   }

   return true;
}
