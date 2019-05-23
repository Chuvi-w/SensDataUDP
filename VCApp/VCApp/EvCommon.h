#pragma once

#include "jni.h"
#include <intrin.h>
#include <vector>
#include <memory>
typedef struct CommPacket_s
{
   jint IsEndian;
   jint PacketID;
   jlong NanoTime;
   jlong elapsedRealtimeNanos;
   jint DataSize;
} CommPacket_t;

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
   ~IEventReceiver() {}
   virtual bool                            ParseEvent(CDataPacket &pPacket) = 0;
   virtual uint32_t                        GetEventID() const = 0;
   virtual std::shared_ptr<IEventReceiver> GetEvShared()                                                 = 0;

};

template<uint64_t EvID>
class IEventReceiverT: public IEventReceiver
{
public:
   ~IEventReceiverT()
   {
   }
   uint32_t GetEventID() const override
   {
      return EvID;
   }
};