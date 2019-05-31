#pragma once

#include "EvCommon.h"
#include <map>
#include <nmeaparse/nmea.h>


typedef struct LocationChangedEvent_s
{
   std::string sProvider;
   int64_t     nTime;
   int64_t     nElapsedRealtimeNanos;
   uint32_t    nFieldMask;
   double      dLatitude;
   double      dLongitude;
   double      dAltitude;
   float       flSpeed;
   float       flBearing;
   float       flAccuracy;
   float       flVerticalAccuracyMeters;
   float       flSpeedAccuracyMetersPerSecond;
   float       flBearingAccuracyDegrees;
} LocationChangedEvent_t;


typedef struct NMEAPacket_s
{
   char sTalker[2];
   char sMsgType[3];
   std::vector<std::string> vData;
}NMEAPacket_t;

class CNMEAEvent : public IEventReceiver
{
 private:
   CNMEAEvent();
   using PTR = std::shared_ptr<CNMEAEvent>;

 public:
   static PTR Create() { return std::shared_ptr<CNMEAEvent>(new CNMEAEvent); }

   ~CNMEAEvent();

   virtual bool ParseEvent(CDataPacket& pPacket) override;

   virtual std::shared_ptr<IEventReceiver> GetEvShared() override;

 private:
   NMEAPacket_t ParseNMEAString(std::string& sNMEA) const;
   nmea::NMEAParser m_NMEAParser;
   nmea::GPSService m_NMEAGps;


   std::map < std::string, std::map<std::string, uint32_t>> m_nEvTypes;
};