#pragma once

#include "jni.h"
#include <intrin.h>
#include <vector>
#include <memory>
typedef struct CommPacket_s
{
   jlong IsEndian;
   jlong PacketID;
   jlong DataSize;
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

class IEventReceiver : public std::enable_shared_from_this<IEventReceiver>
{
 public:
   ~IEventReceiver() {}
   virtual bool                            ParseEvent(const void* pData, size_t nDataSize, bool bEndian) = 0;
   virtual uint32_t                        GetEventID()                                                  = 0;
   virtual std::shared_ptr<IEventReceiver> GetEvShared()                                                 = 0;
};