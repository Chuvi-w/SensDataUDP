#include "CNMEAEvent.h"
#include "CDataPacket.h"
#include <iostream>
#include <iomanip>
using namespace std;

#include <windows.h>
#include <thread>
#include <chrono>
#include "ConsoleTools.h"

std::map<uint32_t, std::string> g_KnownPrn;

CNMEAEvent::CNMEAEvent() : IEventReceiver(NMEA_EV_ID), m_NMEAGps(m_NMEAParser)
{
   // m_NMEAParser.log = true;

   //
   auto OnUpdete = [this](const std::string& sTalker, const nmea::GPSFix& Fix) {
      std::stringstream sOut;
      bool              bSatFound;

      //       if (sTalker == "GP")
      //       {
      //          return;
      //       }

      auto EmptySatString = [](uint32_t prn) {
         stringstream ss;

         ss << "[PRN: " << setw(3) << setfill(' ') << prn << " "
            << "  SNR: " << setw(3) << setfill(' ') << 0.0 << " dB  "
            << "  Azimuth: " << setw(3) << setfill(' ') << 0.0 << " deg "
            << "  Elevation: " << setw(3) << setfill(' ') << 0.0 << " deg  "
            << "]";

         return ss.str();
      };

      if(Fix.almanac.satellites.empty())
      {
         return;
      }
      gotoxy(0, 1);
      //       sOut << "[" << sTalker << "]:" << Fix.timestamp.toString() << " " << (Fix.locked() ? "[*] " : "[ ] ") << setw(2) << setfill(' ') << Fix.trackingSatellites << "/" << setw(2) << setfill(' ') << Fix.visibleSatellites << " ";
      //       sOut << fixed << setprecision(2) << setw(5) << setfill(' ') << Fix.almanac.averageSNR() << " dB   ";
      //       sOut << fixed << setprecision(2) << setw(6) << setfill(' ') << Fix.speed << " km/h [" << nmea::GPSFix::travelAngleToCompassDirection(Fix.travelAngle, false) << "]  ";
      //       sOut << fixed << setprecision(6) << Fix.latitude << "\xF8 " "N, " << Fix.longitude << "\xF8 " "E" << "  ";
      //       sOut << "+/- " << setprecision(1) << Fix.horizontalAccuracy() << "m  ";
      //       sOut << endl;

      // printf("NMEA_%s\n",sTalker.c_str());
      for(const auto& Sat : Fix.almanac.satellites)
      {
         g_KnownPrn[Sat.prn] = Sat.toString();
      }
      gotoxy(0, 1);
      // printf("%s\n", sOut.str().c_str());
      for(auto t : g_KnownPrn)
      {
         bSatFound = false;
         for(const auto& Sat : Fix.almanac.satellites)
         {
            if(Sat.prn == t.first)
            {
               bSatFound = true;
               printf("\t[*]%s\n", Sat.toString().c_str());
            }
         }
         if(!bSatFound)
         {
            // printf("\t[o]%s\n", EmptySatString(t.first).c_str());
            printf("\t[o]%s\n", t.second.c_str());
         }

        // std::this_thread::sleep_for(std::chrono::microseconds(20));
         // Sleep(2);
      }

#ifdef NMEA_DEBUG_EVENTS
      if(Fix.almanac.satellites.empty())
      {
         return;
      }

      sOut << "[" << sTalker << "]:" << Fix.timestamp.toString() << " " << (Fix.locked() ? "[*] " : "[ ] ") << setw(2) << setfill(' ') << Fix.trackingSatellites << "/" << setw(2) << setfill(' ') << Fix.visibleSatellites << " ";
      sOut << fixed << setprecision(2) << setw(5) << setfill(' ') << Fix.almanac.averageSNR() << " dB   ";
      sOut << fixed << setprecision(2) << setw(6) << setfill(' ') << Fix.speed << " km/h [" << nmea::GPSFix::travelAngleToCompassDirection(Fix.travelAngle, false) << "]  ";
      sOut << fixed << setprecision(6) << Fix.latitude
           << "\xF8 "
              "N, "
           << Fix.longitude
           << "\xF8 "
              "E"
           << "  ";
      sOut << "+/- " << setprecision(1) << Fix.horizontalAccuracy() << "m  ";
      sOut << endl;

      if(Fix.locked() && Fix.latitude != 0.0 && Fix.longitude != 0.0)
      {
         if(m_vCoordsLogNMEA.empty() || (!m_vCoordsLogNMEA.empty() && m_vCoordsLogNMEA.back().lat != Fix.latitude && m_vCoordsLogNMEA.back().lng != Fix.longitude))
         {
            m_vCoordsLogNMEA.push_back({Fix.latitude, Fix.longitude, Fix.speed});
         }
      }
      m_vGPSLog.push_back(sOut.str());
#endif

      printf("%s", sOut.str().c_str());
      // std::cout << sOut.str();
   };

   m_NMEAGps.onUpdate += OnUpdete;
}

CNMEAEvent::~CNMEAEvent()
{
#ifdef NMEA_DEBUG_EVENTS
   FILE* f = fopen("GPS.log", "wb");
   if(f)
   {
      for(const auto& s : m_vGPSLog)
      {
         fwrite(s.c_str(), s.length(), 1, f);
      }

      fclose(f);
   }

   f = fopen("GPSLog.csv", "wt");
   if(f)
   {
      for(size_t nCount = 0; nCount < m_vCoordsLog.size(); nCount++)
      {
         fprintf(f, "%.12f, %.12f, %llu_%.6f\n", m_vCoordsLog[nCount].lat, m_vCoordsLog[nCount].lng, m_vCoordsLog.size() - 1 - nCount, m_vCoordsLog[nCount].speed);
      }
      fclose(f);
   }
   f = fopen("GPSLogNMEA.csv", "wt");
   if(f)
   {
      for(size_t nCount = 0; nCount < m_vCoordsLogNMEA.size(); nCount++)
      {
         fprintf(f, "%.12f, %.12f, %llu_%.6f\n", m_vCoordsLogNMEA[nCount].lat, m_vCoordsLogNMEA[nCount].lng, m_vCoordsLogNMEA.size() - 1 - nCount, m_vCoordsLogNMEA[nCount].speed);
      }

      fclose(f);
   }

   std::cout << std::endl;
   for(const auto& EvTypes : m_nEvTypes)
   {
      std::cout << EvTypes.first << std::endl << "{" << std::endl;
      for(const auto& EvType : EvTypes.second)
      {
         std::cout << "\t" << EvType.first << "=" << EvType.second << std::endl;
      }
      std::cout << "}" << std::endl;
   }
#endif
}

bool CNMEAEvent::ParseEvent(const CDataPacket& pPacket)
{

   int32_t  nPacketID;
   uint64_t nTimeStamp;

   std::string sNmeaPacket;

   LocationChangedEvent_t LocEvent;

   if(pPacket.GetData(nPacketID))
   {
      if(nPacketID == 0xa5)
      {
         if(pPacket.GetData(nTimeStamp) && pPacket.GetData(sNmeaPacket))
         {
#ifdef NMEA_DEBUG_EVENTS
            printf("%s", sNmeaPacket.c_str());
#endif
            try
            {
               m_NMEAParser.readSentence(sNmeaPacket);
               // m_NMEAParser.readLine(sNmeaPacket.c_str());
            }
            catch(nmea::NMEAParseError& e)
            {
               cout << e.message << endl << endl;
            }
#ifdef NMEA_DEBUG_EVENTS
            auto Packet = ParseNMEAString(sNmeaPacket);
            if(Packet.sTalker[0] != 0)
            {
               std::string sTalker  = std::string(Packet.sTalker, Packet.sTalker + 2);
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
#endif
         }
         else
         {
            printf("BAD PACKET");
            do
            { } while(1); }
      }
      else if(nPacketID == 0x5a)
      {
         if(pPacket.GetData(LocEvent.sProvider) && pPacket.GetData(LocEvent.nTime) && pPacket.GetData(LocEvent.nElapsedRealtimeNanos) && pPacket.GetData(LocEvent.nFieldMask) && pPacket.GetData(LocEvent.dLatitude) && pPacket.GetData(LocEvent.dLongitude) && pPacket.GetData(LocEvent.dAltitude) && pPacket.GetData(LocEvent.flSpeed) && pPacket.GetData(LocEvent.flBearing) && pPacket.GetData(LocEvent.flAccuracy) && pPacket.GetData(LocEvent.flVerticalAccuracyMeters) && pPacket.GetData(LocEvent.flSpeedAccuracyMetersPerSecond) && pPacket.GetData(LocEvent.flBearingAccuracyDegrees))
         {
            m_vLocPackets.emplace_back(pPacket.GetNanoTime(), LocEvent);
#if 1       //#ifdef NMEA_DEBUG_EVENTS
            // bswap(LocEvent.nElapsedRealtimeNanos);
            // m_vCoordsLog.push_back({ LocEvent.dLatitude, LocEvent.dLongitude,LocEvent.flSpeed });
            // gotoxy(0, 0);
            //  printf("GPS %.9f %.9f %.9f %llu %llu %llu\n", LocEvent.dLatitude, LocEvent.dLongitude, LocEvent.dAltitude, LocEvent.nTime, LocEvent.nElapsedRealtimeNanos, pPacket.GetNanoTime().m_TS);
#endif
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
   std::string              sToken;
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
   char  StrChar;
   nRecvCRC = strtol(&sNMEA[nStarPos + 1], &sEndPtr, 0x10);
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

   Packet.sTalker[0]  = vTokens[0][0];
   Packet.sTalker[1]  = vTokens[0][1];
   Packet.sMsgType[0] = vTokens[0][2];
   Packet.sMsgType[1] = vTokens[0][3];
   Packet.sMsgType[2] = vTokens[0][4];

   for(size_t n = 1; n < vTokens.size(); n++)
   {
      Packet.vData.push_back(vTokens[n]);
   }

   return Packet;
}
