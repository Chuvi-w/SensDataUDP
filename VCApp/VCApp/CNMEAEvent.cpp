#include "CNMEAEvent.h"
#include "CDataPacket.h"
#include <iostream>
#include <iomanip>
using namespace std;
CNMEAEvent::CNMEAEvent(): IEventReceiver(NMEA_EV_ID), m_NMEAGps(m_NMEAParser)
{
  // m_NMEAParser.log = true;
//    m_NMEAGps.onUpdate += [this]()
//    {
//       auto &gps = this->m_NMEAGps;
//       cout <<gps.fix.timestamp.toString()<<" "<< (gps.fix.locked() ? "[*] " : "[ ] ") << setw(2) << setfill(' ') << gps.fix.trackingSatellites << "/" << setw(2) << setfill(' ') << gps.fix.visibleSatellites << " ";
//       cout << fixed << setprecision(2) << setw(5) << setfill(' ') << gps.fix.almanac.averageSNR() << " dB   ";
//       cout << fixed << setprecision(2) << setw(6) << setfill(' ') << gps.fix.speed << " km/h [" << nmea::GPSFix::travelAngleToCompassDirection(gps.fix.travelAngle, false) << "]  ";
//       cout << fixed << setprecision(6) << gps.fix.latitude << "\xF8 " "N, " << gps.fix.longitude << "\xF8 " "E" << "  ";
//       cout << "+/- " << setprecision(1) << gps.fix.horizontalAccuracy() << "m  ";
//       cout << endl;
//    };

}

CNMEAEvent::~CNMEAEvent() {}

bool CNMEAEvent::ParseEvent(CDataPacket& pPacket)
{

   int32_t         nPacketID;
   uint64_t        nTimeStamp;
 
   std::string sNmeaPacket;

   LocationChangedEvent_t LocEvent;

  
   if(pPacket.GetData(nPacketID))
   {
      if(nPacketID == 0xa5)
      {
         if(pPacket.GetData(nTimeStamp) && pPacket.GetData(sNmeaPacket))
         {
            try
            {
               m_NMEAParser.readLine(sNmeaPacket.c_str());
            }
            catch(nmea::NMEAParseError& e)
            {
               cout << e.message << endl << endl;
            }

            auto Packet = ParseNMEAString(sNmeaPacket);
            if(Packet.sTalker[0] != 0)
            {
               std::string sTalker = std::string(Packet.sTalker, Packet.sTalker + 2);
               std::string sMsgType = std::string(Packet.sMsgType, Packet.sMsgType + 3);
               if(m_nEvTypes.find(sTalker) == m_nEvTypes.end())
               {
                  m_nEvTypes[sTalker] = std::map<std::string, uint32_t>();
               }

               if(m_nEvTypes[sTalker].find(sMsgType) == m_nEvTypes[sTalker].end())
               {
                  m_nEvTypes[sTalker][sMsgType] = 1;
               }
               else
               {
                  m_nEvTypes[sTalker][sMsgType]++;
               }

            }

         }
      }
      else if(nPacketID == 0x5a)
      {
         if(pPacket.GetData(LocEvent.sProvider) && pPacket.GetData(LocEvent.nTime) && pPacket.GetData(LocEvent.nElapsedRealtimeNanos) && pPacket.GetData(LocEvent.nFieldMask) && pPacket.GetData(LocEvent.dLatitude) && pPacket.GetData(LocEvent.dLongitude) && pPacket.GetData(LocEvent.dAltitude) && pPacket.GetData(LocEvent.flSpeed) && pPacket.GetData(LocEvent.flBearing) && pPacket.GetData(LocEvent.flAccuracy) && pPacket.GetData(LocEvent.flVerticalAccuracyMeters) && pPacket.GetData(LocEvent.flSpeedAccuracyMetersPerSecond) && pPacket.GetData(LocEvent.flBearingAccuracyDegrees))
         {
            // bswap(LocEvent.nElapsedRealtimeNanos);
            printf("GPS %.9f %.9f %.9f %llu %llu %llu\n", LocEvent.dLatitude, LocEvent.dLongitude, LocEvent.dAltitude, LocEvent.nTime, LocEvent.nElapsedRealtimeNanos, pPacket.GetNanoTime().m_TS);
         }
      }
   }
   return true;
}

std::shared_ptr<IEventReceiver> CNMEAEvent::GetEvShared() { return shared_from_this(); }

NMEAPacket_t CNMEAEvent::ParseNMEAString(std::string& sNMEA) const
{
   NMEAPacket_t Packet;
   memset(Packet.sTalker, 0, sizeof(Packet.sTalker));
   memset(Packet.sMsgType, 0, sizeof(Packet.sMsgType));
   std::vector<std::string> vTokens;
   std::string sToken;
   if(sNMEA.empty()) //$XXXXX*ZZ\r\n
   {
      return Packet;
   }
   uint8_t nCalcCRC = 0, nRecvCRC;
   while(sNMEA.back() == '\r' || sNMEA.back() == '\n')
   {
      sNMEA.pop_back();
   }

   if(sNMEA[0] != '$' && sNMEA[0] != '!')
   {
      return Packet;
   }
   auto nStarPos = sNMEA.find_last_of('*');

   if(nStarPos == sNMEA.npos || sNMEA.length() - nStarPos != 3)
   {
      return Packet;
   }

   char* sEndPtr = 0;
   char StrChar;
   nRecvCRC      = strtol(&sNMEA[nStarPos + 1], &sEndPtr, 0x10);
   if(&sNMEA[sNMEA.length()] != sEndPtr)
   {
      return Packet;
   }
   sToken.clear();
   for(size_t nChar = 1; nChar < nStarPos; nChar++)
   {
      StrChar = sNMEA[nChar];
      nCalcCRC ^= StrChar;
      if(StrChar == ',')
      {
         vTokens.push_back(sToken);
         sToken.clear();
      }
      else
      {
         sToken.push_back(StrChar);
      }
     
   }

   vTokens.push_back(sToken);

   if(nCalcCRC != nRecvCRC)
   {
      return Packet;
   }
  
   if(vTokens.empty() || vTokens[0].length() != 5)
   {
      return Packet;
   }

   Packet.sTalker[0] = vTokens[0][0];
   Packet.sTalker[1] = vTokens[0][1];
   Packet.sMsgType[0] = vTokens[0][2];
   Packet.sMsgType[1] = vTokens[0][3];
   Packet.sMsgType[2] = vTokens[0][4];
   
   for(size_t n = 1; n < vTokens.size(); n++)
   {
      Packet.vData.push_back(vTokens[n]);
   }


   return Packet;
}
