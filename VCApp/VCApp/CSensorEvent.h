#pragma once

#include "EvCommon.h"
#include "CIMUBase.h"
#include <map>
#include "CIMU_Acc.h"
#include <algorithm>

#pragma pack(push, 1)
typedef struct ComSensorsHdr_s
{
   jlong  nTimeStamp;
   jint   nType;
   jint   nAccuracy;
   jint   nSize;
   jfloat flRes;
   jfloat flMaxRange;
   jlong  nCount;
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

   virtual bool                            ParseEvent(const CDataPacket& pPacket) override;
   virtual std::shared_ptr<IEventReceiver> GetEvShared() override;

 private:
   std::map<IMUType_t, CBaseIMUSensor::PTR> m_vIMU;

   CMinMax<int64_t> m_AccGyrDiff;
   CMinMax<int64_t> m_GyrMagDiff;
   CMinMax<int64_t> m_AccMagDiff;
   CMinMax<int64_t> m_AccTime;
   CMinMax<int64_t> m_GyrTime;
   CMinMax<int64_t> m_MagTime;
   CIMUAcc::PTR     m_Acc;
   CIMUGyr::PTR     m_Gyr;
   CIMUMag::PTR     m_Mag;
   int64_t          m_nLastPacketCount;
   CTimeStampNS     m_FirstFrameTime;
};