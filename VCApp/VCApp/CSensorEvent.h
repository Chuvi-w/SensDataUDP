#pragma once

#include "EvCommon.h"
#include "CIMUSensor.h"


typedef struct ComSensorsHdr_s
{
   jlong nType;
   jlong nTimeStamp;
   jlong nSize;

}ComSensorsHdr_t;


class CSensorEvent :public IEventReceiver
{
private:
   CSensorEvent();
   using PTR = std::shared_ptr<CSensorEvent>;
public:
   static PTR Create()
   {

      return std::shared_ptr<CSensorEvent>(new CSensorEvent);
   }
   
   ~CSensorEvent();

   virtual bool ParseEvent(const void *pData, size_t nDataSize, bool bEndian) override;
  

   virtual uint32_t GetEventID() override;

   virtual std::shared_ptr<IEventReceiver> GetEvShared() override;

private:

   CACCSensor m_Acc;
   CGyroSensor m_Gyr;
   CMagSensor m_Mag;

};