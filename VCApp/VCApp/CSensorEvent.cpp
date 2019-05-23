#include "CSensorEvent.h"
#include <inttypes.h>
#include "CDataPacket.h"

CSensorEvent::CSensorEvent() {}

CSensorEvent::~CSensorEvent() {}

bool CSensorEvent::ParseEvent(CDataPacket &pPacket)
{
   ComSensorsHdr_t SensHDR;
   float *FlValue = nullptr;
   bool bReadOK = true;
   if(pPacket.GetDataSize() < sizeof(ComSensorsHdr_t))
   {
      return false;
   }
   if(
      !pPacket.GetData(SensHDR.nType) ||
      !pPacket.GetData(SensHDR.nTimeStamp) ||
      !pPacket.GetData(SensHDR.nSize) ||
      !pPacket.GetData(SensHDR.flRes) ||
      !pPacket.GetData(SensHDR.flMaxRange) ||
      pPacket.GetRemainDataSize() < SensHDR.nSize*sizeof(float)
      )
   {
      return false;
   }
   
   CTimeStampNS TS = SensHDR.nTimeStamp;
   double Sec1 = TS.GetSeconds();
   double Sec2 = pPacket.GetTimeStamp().GetSeconds();

   
   printf("%.3f %.3f %.3f\n", Sec1,Sec2,Sec1-Sec2);
   FlValue=new float[SensHDR.nSize];
   for(jlong i = 0; i < SensHDR.nSize; i++)
   {
      if(!pPacket.GetData(FlValue[i]))
      {
         bReadOK = false;
      }
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