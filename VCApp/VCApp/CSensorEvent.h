#pragma once

#include "EvCommon.h"
#include "CIMUBase.h"
#include <map>


#pragma pack(push, 1)
typedef struct ComSensorsHdr_s
{
   jlong  nTimeStamp;
   jint   nType;
   jint   nAccuracy;
   jfloat flRes;
   jfloat flMaxRange;
   jint  nSize;
} ComSensorsHdr_t;
#pragma pack(pop)


class CSensorEvent : public IEventReceiver
{
 private:
   CSensorEvent();
   using PTR = std::shared_ptr<CSensorEvent>;

 public:
    ~CSensorEvent();
   static PTR Create() { return std::shared_ptr<CSensorEvent>(new CSensorEvent); }

 
   virtual bool ParseEvent(const CDataPacket& pPacket) override;
   virtual std::shared_ptr<IEventReceiver> GetEvShared() override;

   bool AddIMU(CBaseIMUSensor::PTR IMU);
 private:
    std::map<IMUType_t, CBaseIMUSensor::PTR> m_vIMU;
};