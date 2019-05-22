#include "CSensorEvent.h"
#include <inttypes.h>

CSensorEvent::CSensorEvent() {}

CSensorEvent::~CSensorEvent() {}

bool CSensorEvent::ParseEvent(const void* pData, size_t nDataSize, bool bEndian)
{
   const ComSensorsHdr_t* pHdr = reinterpret_cast<const ComSensorsHdr_t*>(pData);

   const float* pValue = reinterpret_cast<const float*>((size_t)pHdr + sizeof(ComSensorsHdr_t));
#if 0
   char *sType;
   bool bprint = false;
   switch (pHdr->nType)
   {
      case TYPE_ACCELEROMETER:
         sType = "TYPE_ACCELEROMETER";
        
         break;
      case TYPE_MAGNETIC_FIELD:
         sType = "TYPE_MAGNETIC_FIELD";
         break;
      case TYPE_GYROSCOPE:
         sType = "TYPE_GYROSCOPE";
         break;
      case TYPE_MAGNETIC_FIELD_UNCALIBRATED:
         sType = "TYPE_MAGNETIC_FIELD_UNCALIBRATED";
         bprint = true;
         break;
      case TYPE_GYROSCOPE_UNCALIBRATED:
         sType = "TYPE_GYROSCOPE_UNCALIBRATED";
         break;
      case TYPE_ACCELEROMETER_UNCALIBRATED:
         sType = "TYPE_ACCELEROMETER_UNCALIBRATED";
         break;
      default:
         sType = "UNKNOWN";
         break;
   }

   uint64_t InvTS = pHdr->nTimeStamp;

   if (bprint)
   {
      printf("SensEv: %35s  %llu %llu %i %i {", sType, pHdr->nTimeStamp / 1000000000ll, pHdr->nTimeStamp % 1000000000ll, pHdr->nSize, nDataSize);
      for (int i = 0; i < pHdr->nSize; i++)
      {
         printf("%.6f, ", pValue[i]);
      }
      printf("}\n");
   }
#endif
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

   return true;
}

uint32_t CSensorEvent::GetEventID() { return 0xBB00; }

std::shared_ptr<IEventReceiver> CSensorEvent::GetEvShared() { return shared_from_this(); }
