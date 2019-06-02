/*
 * GPSService.h
 *
 *  Created on: Aug 14, 2014
 *      Author: Cameron Karlsson
 *
 *  See the license file included with this source.
 */

#ifndef GPSSERVICE_H_
#define GPSSERVICE_H_

#include <string>
#include <chrono>
#include <functional>
#include <map>
#include <nmeaparse/GPSFix.h>
#include <nmeaparse/NMEAParser.h>
#include <nmeaparse/Event.h>

namespace nmea
{

class GPSService
{
 private:
   void read_PSRF150(const NMEASentence& nmea);
   void read_GPGGA(const NMEASentence& nmea);
   void read_GPGSA(const NMEASentence& nmea);
   void read_GPGSV(const NMEASentence& nmea);
   void read_GPRMC(const NMEASentence& nmea);
   void read_GPVTG(const NMEASentence& nmea);

   void CreateFixIfNotExist(const NMEASentence& nmea);
 public:
   std::map<std::string,GPSFix> mfix;

   GPSService(NMEAParser& parser);
   virtual ~GPSService();

   Event<void(bool)> onLockStateChanged; // user assignable handler, called whenever lock changes
   Event<void(const std::string &, const GPSFix&)>     onUpdate;           // user assignable handler, called whenever fix changes

   void attachToParser(NMEAParser& parser); // will attach to this parser's nmea sentence events
};

} // namespace nmea

#endif /* GPSSERVICE_H_ */
