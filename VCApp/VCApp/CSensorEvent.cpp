#include "CSensorEvent.h"
#include <inttypes.h>
#include "CDataPacket.h"
#include "ConsoleTools.h"
#include "..\OpenSource_IMU\MadgwickAHRS_FIXED_C_NEW\MadgwickAHRS_FIXED_NEW.h"

CSensorEvent::CSensorEvent() : IEventReceiver(SENSOR_EV_ID), m_Acc(CIMUAcc::Create()), m_Gyr(CIMUGyr::Create()), m_Mag(CIMUMag::Create()), m_nLastPacketCount(-1)
{
   m_vIMU[m_Acc->GetType()] = m_Acc;
   m_vIMU[m_Gyr->GetType()] = m_Gyr;
   m_vIMU[m_Mag->GetType()] = m_Mag;
   m_FirstFrameTime = 0;
   InitMadgAHRS(&gMadgAHRSNew);
}

CSensorEvent::~CSensorEvent() {}

bool CSensorEvent::ParseEvent(const CDataPacket& pPacket)
{
   ComSensorsHdr_t SensHDR;
   float           FlCoords[3];
   float           FlCalib[3] = {0, 0, 0};
   bool            bReadOK    = true;

   if(pPacket.GetDataSize() < sizeof(ComSensorsHdr_t))
   {
      return false;
   }
   if(!pPacket.GetData(SensHDR.nTimeStamp) || !pPacket.GetData(SensHDR.nType) || !pPacket.GetData(SensHDR.nAccuracy) || !pPacket.GetData(SensHDR.nSize) || !pPacket.GetData(SensHDR.flRes) || !pPacket.GetData(SensHDR.flMaxRange) || !pPacket.GetData(SensHDR.nCount) || pPacket.GetRemainDataSize() < SensHDR.nSize * sizeof(float))
   {
      return false;
   }

   if(SensHDR.nSize >= 3)
   {
      if(!pPacket.GetData(FlCoords[0]) || !pPacket.GetData(FlCoords[1]) || !pPacket.GetData(FlCoords[2]))
      {
         return false;
      }
   }
   if(SensHDR.nSize >= 6)
   {
      if(!pPacket.GetData(FlCalib[0]) || !pPacket.GetData(FlCalib[1]) || !pPacket.GetData(FlCalib[2]))
      {
         return false;
      }
   }

   if(m_nLastPacketCount != -1)
   {
      if(m_nLastPacketCount + 1 != SensHDR.nCount)
      {
         printf("Packet lost (%llu)\n", SensHDR.nCount - (m_nLastPacketCount + 1));
      }
   }

   if (m_FirstFrameTime.m_TS == 0)
   {
      m_FirstFrameTime = pPacket.GetNanoTime();
   }
   m_nLastPacketCount = SensHDR.nCount;
   auto fIMU          = m_vIMU.find(IMUType_t(SensHDR.nType));

   if(SensHDR.nType == ASENSOR_TYPE_GYROSCOPE_UNCALIBRATED)
   {
      printf("");
   }

   if(fIMU != m_vIMU.end())
   {
      fIMU->second->AddFrame(CIMUFrame(pPacket.GetCommonData(), SensHDR.nTimeStamp, Vec3D(FlCoords[0], FlCoords[1], FlCoords[2]), Vec3D(FlCalib[0], FlCalib[1], FlCalib[2])), SensHDR.flRes, SensHDR.flMaxRange);
   }
   gotoxy(0, 2);
   printf("%011.7f\n", pPacket.GetNanoTime().GetSeconds() - m_FirstFrameTime.GetSeconds());
   static size_t nCOunt   = 0;
   auto          AccTime  = m_Acc->GetLastFrameTime();
   auto          GyrTime  = m_Gyr->GetLastFrameTime();
   auto          MagTime  = m_Mag->GetLastFrameTime();
   double        dTimeDiv = 1000000.0;
   if(0&&AccTime && GyrTime && MagTime)
   {
#define __abs(v) (v)

      m_AccGyrDiff.AddElement(__abs(AccTime - GyrTime));
      m_AccMagDiff.AddElement(__abs(AccTime - MagTime));
      m_GyrMagDiff.AddElement(__abs(GyrTime - MagTime));
      m_AccTime.AddElement(m_Acc->GetFrameTimeDiff());
      m_GyrTime.AddElement(m_Gyr->GetFrameTimeDiff());
      m_MagTime.AddElement(m_Mag->GetFrameTimeDiff());
      auto PrintMinMax = [dTimeDiv](const char *sName, const CMinMax<int64_t> &val)
      {
         printf("%s %011.7f %011.7f %011.7f | %011.7f %011.7f\t\t\t\t\n",sName, (double)val.GetMin() / dTimeDiv, (val.GetAvg<double>()) / dTimeDiv, (double)val.GetMax() / dTimeDiv, val.GetSKO<double>() / dTimeDiv, val.GetLastVal() / dTimeDiv);

      };

      gotoxy(0, 4);
      
      PrintMinMax("m_AccGyrDiff=", m_AccGyrDiff);
      PrintMinMax("m_AccMagDiff=", m_AccMagDiff);
      PrintMinMax("m_GyrMagDiff=", m_GyrMagDiff);
      PrintMinMax("m_AccTime=   ", m_AccTime);
      PrintMinMax("m_GyrTime=   ", m_GyrTime);
      PrintMinMax("m_MagTime=   ", m_MagTime);
     // printf("%.3f\n", m_MagTime.GetAvg<double>());
      nCOunt++;

//       if(nCOunt > 1000)
//       {
//          nCOunt = 0;
//          m_AccGyrDiff.Reset();
//          m_AccMagDiff.Reset();
//          m_GyrMagDiff.Reset();
//          m_AccTime.Reset();
//          m_GyrTime.Reset();
//          m_MagTime.Reset();
//       }
   }
   static int nFrCala = 0;
   if(AccTime&&GyrTime&&MagTime&&(double)(int64_t)(AccTime - GyrTime) / dTimeDiv <= 4.0&&!m_Acc->IsProcessed() && !m_Mag->IsProcessed())
   {

      double dTime = (double)(m_Acc->GetFrameTimeDiff().GetSeconds() + m_Gyr->GetFrameTimeDiff().GetSeconds()) / 2.0;
      m_Acc->SetProcessed();
      m_Mag->SetProcessed();
      gotoxy(0, 11);
      printf("%i\n", nFrCala++);
      MadgwickAHRSupdate_FixedNew(dTime,m_Gyr->GetLastFrame(), m_Acc->GetLastFrame(), NullVec3D);
      gotoxy(0, 12);
      printf("%018.15f {%018.15f, %018.15f, %018.15f, %018.15f}\n", gMadgAHRSNew.beta, gMadgAHRSNew._q.x, gMadgAHRSNew._q.y, gMadgAHRSNew._q.z, gMadgAHRSNew._q.w);
   }

//    if(SensHDR.nType == ASENSOR_TYPE_GYROSCOPE_UNCALIBRATED&&GyrTime)
//    {
//       MadgwickAHRSupdate_FixedNew(m_Gyr->GetLastFrame(), NullVec3D, NullVec3D);
//       gotoxy(0, 12);
//       printf("%018.15f {%018.15f, %018.15f, %018.15f, %018.15f}\n", gMadgAHRSNew.beta, gMadgAHRSNew._q.x, gMadgAHRSNew._q.y, gMadgAHRSNew._q.z, gMadgAHRSNew._q.w);
//    }
   return true;
}

std::shared_ptr<IEventReceiver> CSensorEvent::GetEvShared() { return shared_from_this(); }
