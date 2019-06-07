#include "CSensorEvent.h"
#include <inttypes.h>
#include "CDataPacket.h"
#include "ConsoleTools.h"

CSensorEvent::CSensorEvent() : 
IEventReceiver(SENSOR_EV_ID),
m_Acc(CIMUAcc::Create()),
m_Gyr(CIMUGyr::Create()),
m_Mag(CIMUMag::Create())
{
   m_vIMU[m_Acc->GetType()] = m_Acc;
   m_vIMU[m_Gyr->GetType()] = m_Gyr;
   m_vIMU[m_Mag->GetType()] = m_Mag;
}

CSensorEvent::~CSensorEvent() {}

bool CSensorEvent::ParseEvent(const CDataPacket& pPacket)
{
   ComSensorsHdr_t SensHDR;
   float           FlCoords[3];
   float           FlCalib[3] = {0, 0, 0};
   bool            bReadOK = true;

   
   if(pPacket.GetDataSize() < sizeof(ComSensorsHdr_t))
   {
      return false;

   }
   if(
      !pPacket.GetData(SensHDR.nTimeStamp) ||
      !pPacket.GetData(SensHDR.nType) ||
      !pPacket.GetData(SensHDR.nAccuracy) ||
      !pPacket.GetData(SensHDR.nSize) ||
      !pPacket.GetData(SensHDR.flRes) ||
      !pPacket.GetData(SensHDR.flMaxRange) ||
      !pPacket.GetData(SensHDR.nCount) ||
      pPacket.GetRemainDataSize() < SensHDR.nSize * sizeof(float)
   )
   {
      return false;
   }

   if(SensHDR.nSize >= 3)
   {
      if(
         !pPacket.GetData(FlCoords[0]) ||
         !pPacket.GetData(FlCoords[1]) ||
         !pPacket.GetData(FlCoords[2])
         )
      {
         return false;
      }

   }
   if(SensHDR.nSize >= 6)
   {
      if(
         !pPacket.GetData(FlCalib[0]) ||
         !pPacket.GetData(FlCalib[1]) ||
         !pPacket.GetData(FlCalib[2])
         )
      {
         return false;
      }

   }


   auto fIMU = m_vIMU.find(IMUType_t(SensHDR.nType));
   if(fIMU!=m_vIMU.end())
   {   
      fIMU->second->AddFrame(CIMUFrame(pPacket.GetCommonData(), SensHDR.nTimeStamp, Vec3D(FlCoords[0], FlCoords[1], FlCoords[2]), Vec3D(FlCalib[0], FlCalib[1], FlCalib[2])), SensHDR.flRes, SensHDR.flMaxRange);
   }
   static size_t nCOunt = 0;
   auto AccTime = m_Acc->GetLastFrameTime();
   auto GyrTime = m_Gyr->GetLastFrameTime();
   auto MagTime = m_Mag->GetLastFrameTime();
   double dTimeDiv = 1000000.0;
   if(AccTime&&GyrTime&&MagTime)
   {
      m_AccGyrDiff.AddElement(abs(AccTime - GyrTime));
      m_AccMagDiff.AddElement(abs(AccTime - MagTime));
      m_GyrMagDiff.AddElement(abs(GyrTime - MagTime));
      m_AccTime.AddElement(m_Acc->GetFrameTimeDiff());
      m_GyrTime.AddElement(m_Gyr->GetFrameTimeDiff());
      m_MagTime.AddElement(m_Mag->GetFrameTimeDiff());

      gotoxy(0, 0);

      printf("m_AccGyrDiff= %06.7f %06.7f %06.7f\n", (double)m_AccGyrDiff.GetMin() / dTimeDiv, ((double)m_AccGyrDiff.GetSum() / (double)m_AccGyrDiff.GetCount()) / dTimeDiv, (double)m_AccGyrDiff.GetMax() / dTimeDiv);
      printf("m_AccMagDiff= %06.7f %06.7f %06.7f\n", (double)m_AccMagDiff.GetMin() / dTimeDiv, ((double)m_AccMagDiff.GetSum() / (double)m_AccMagDiff.GetCount()) / dTimeDiv, (double)m_AccMagDiff.GetMax() / dTimeDiv);
      printf("m_GyrMagDiff= %06.7f %06.7f %06.7f\n", (double)m_GyrMagDiff.GetMin() / dTimeDiv, ((double)m_GyrMagDiff.GetSum() / (double)m_GyrMagDiff.GetCount()) / dTimeDiv, (double)m_GyrMagDiff.GetMax() / dTimeDiv);
      printf("m_AccTime= %06.7f %06.7f %06.7f\n", (double)m_AccTime.GetMin() / dTimeDiv, ((double)m_AccTime.GetSum() / (double)m_AccTime.GetCount()) / dTimeDiv, (double)m_AccTime.GetMax() / dTimeDiv);
      printf("m_GyrTime= %06.7f %06.7f %06.7f\n", (double)m_GyrTime.GetMin() / dTimeDiv, ((double)m_GyrTime.GetSum() / (double)m_GyrTime.GetCount()) / dTimeDiv, (double)m_GyrTime.GetMax() / dTimeDiv);
      printf("m_MagTime= %06.7f %06.7f %06.7f\n", (double)m_MagTime.GetMin() / dTimeDiv, ((double)m_MagTime.GetSum() / (double)m_MagTime.GetCount()) / dTimeDiv, (double)m_MagTime.GetMax() / dTimeDiv);

      
      nCOunt++;

      if(nCOunt > 1000)
      {
         nCOunt = 0;
         m_AccGyrDiff.Reset();
         m_AccMagDiff.Reset();
         m_GyrMagDiff.Reset();
         m_AccTime.Reset();
         m_GyrTime.Reset();
         m_MagTime.Reset();
      }
   }
 
  
   return true;
}

std::shared_ptr<IEventReceiver> CSensorEvent::GetEvShared()
{
   return shared_from_this();
}

