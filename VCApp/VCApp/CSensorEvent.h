#pragma once

#include "EvCommon.h"

typedef struct ComSensorsHdr_s
{
   jlong  nType;
   jlong  nTimeStamp;
   jlong  nSize;
   jfloat flRes;
   jfloat flMaxRange;
} ComSensorsHdr_t;

class CSensorEvent : public IEventReceiver
{
 private:
   CSensorEvent();
   using PTR = std::shared_ptr<CSensorEvent>;

 public:
   static PTR Create() { return std::shared_ptr<CSensorEvent>(new CSensorEvent); }

   ~CSensorEvent();

   virtual bool ParseEvent(const CDataPacket& pPacket) override;

   virtual std::shared_ptr<IEventReceiver> GetEvShared() override;

 private:
};