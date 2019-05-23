#pragma once

#include "EvCommon.h"
#include "CIMUSensor.h"

typedef struct ComSensorsHdr_s
{
   jlong  nType;
   jlong  nTimeStamp;
   jlong  nSize;
   jfloat flRes;
   jfloat flMaxRange;
} ComSensorsHdr_t;

class CSensorEvent: public IEventReceiverT<0xBB00>
{
 private:
   CSensorEvent();
   using PTR = std::shared_ptr<CSensorEvent>;

 public:
   static PTR Create() { return std::shared_ptr<CSensorEvent>(new CSensorEvent); }

   ~CSensorEvent();

   virtual bool ParseEvent(CDataPacket &pPacket) override;


   virtual std::shared_ptr<IEventReceiver> GetEvShared() override;

 private:
   CACCSensor  m_Acc;
   CGyroSensor m_Gyr;
   CMagSensor  m_Mag;
};