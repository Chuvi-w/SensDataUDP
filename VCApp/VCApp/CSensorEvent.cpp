#include "CSensorEvent.h"
#include <inttypes.h>
#include "CDataPacket.h"

CSensorEvent::CSensorEvent() : IEventReceiver(SENSOR_EV_ID) {}

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
     ;
      fIMU->second->SetResolution(SensHDR.flRes, SensHDR.flMaxRange);

      fIMU->second->AddFrame( CIMUFrame(pPacket.GetCommonData(),SensHDR.nTimeStamp,Vec3D(FlCoords[0],FlCoords[1],FlCoords[2]),Vec3D(FlCalib[0],FlCalib[1],FlCalib[2])));


   }
#if 0

   if(m_Acc.CheckSensorType((SensTypes)pHdr->nType))
   {
      m_Acc.AddFrame(pValue, nDataSize - sizeof(ComSensorsHdr_t), pHdr->nTimeStamp, pHdr->flRes, pHdr->flMaxRange);
   }
   if(m_Gyr.CheckSensorType((SensTypes)pHdr->nType))
   {
      m_Gyr.AddFrame(pValue, nDataSize - sizeof(ComSensorsHdr_t), pHdr->nTimeStamp, pHdr->flRes, pHdr->flMaxRange);
   }

   if(m_Mag.CheckSensorType((SensTypes)pHdr->nType))
   {
      m_Mag.AddFrame(pValue, nDataSize - sizeof(ComSensorsHdr_t), pHdr->nTimeStamp, pHdr->flRes, pHdr->flMaxRange);
   }

   //  printf("%06u\n", m_Acc.GetDataCount());
   //    if (m_Acc.GetDataCount() == 4000)
   //    {
   //       m_Acc.Calibrate();
   //    }
#endif
   return true;
}

std::shared_ptr<IEventReceiver> CSensorEvent::GetEvShared()
{
   return shared_from_this();
}

bool CSensorEvent::AddIMU(CBaseIMUSensor::PTR IMU)
{
   if(!IMU)
   {
      return false;
   }

   m_vIMU[IMU->GetType()] = IMU;
   return true;
}
