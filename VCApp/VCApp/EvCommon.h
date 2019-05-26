#pragma once

#include "jni.h"
#include <intrin.h>
#include <vector>
#include <memory>


#define TOUCH_EV_ID (0xAA00)
#define SENSOR_EV_ID (0xBB00)


#pragma pack(push, 1)
typedef struct CommPacket_s
{
   jint IsEndian;
   jint PacketID;
   jlong NanoTime;
   jint DataSize;
} CommPacket_t;
#pragma pack(pop)
#define __builtin_bswap16 _byteswap_ushort
#define __builtin_bswap32 _byteswap_ulong
#define __builtin_bswap64 _byteswap_uint64

template <typename T> inline void bswap(T& s)
{

   switch(sizeof(T))
   {
      case 2:
      {
         auto res = __builtin_bswap16(*(uint16_t*)&s);
         s        = *(T*)&res;
         break;
      }
      case 4:
      {
         auto res = __builtin_bswap32(*(uint32_t*)&s);
         s        = *(T*)&res;
         break;
      }
      case 8:
      {
         auto res = __builtin_bswap64(*(uint64_t*)&s);
         s        = *(T*)&res;
         break;
      }
      default: return;
   }
}


class CDataPacket;
class IEventReceiver : public std::enable_shared_from_this<IEventReceiver>
{
 public:
    IEventReceiver(uint32_t nEvID):m_EvID(nEvID){}
    IEventReceiver(const IEventReceiver &pEv):m_EvID(pEv.m_EvID){ }
   ~IEventReceiver() {}
   virtual bool                            ParseEvent(CDataPacket &pPacket) = 0;
   
   virtual std::shared_ptr<IEventReceiver> GetEvShared()                                                 = 0;
   uint32_t                                GetEventID() const
   {
      return m_EvID;
   }

private:
   uint32_t m_EvID;
};

