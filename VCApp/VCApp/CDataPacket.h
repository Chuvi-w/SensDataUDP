#pragma once
#include "EvCommon.h"
#include "CTimeStamp.h"


class CDataPacket
{
public:
   CDataPacket();
   CDataPacket(const CDataPacket &pOther);
   ~CDataPacket();
  
   int32_t LoadData(void *pData, size_t nDataSize, const std::string &sSourceName);

   template<typename T> bool GetData(T &Data)
   {
      if(m_nReadPos + sizeof(T) > m_nRecvSize)
      {
         return false;
      }
      memcpy(&Data, reinterpret_cast<void*>((size_t)m_nData + m_nReadPos), sizeof(T));

      if(!m_bEndian)
      {
         bswap(Data);
      }
      m_nReadPos += sizeof(T);
      return true;
   }

   size_t GetRemainDataSize() const;
   size_t GetDataSize() const;
   bool IsValid() const;
   CTimeStampNS GetTimeStamp() const;
   uint64_t GetPacketID() const;
private:
   void *m_nData;
   size_t m_nReadPos;
   bool m_bEndian;
   CTimeStampNS m_nTimeStampNS;
   uint64_t m_nPacketID;
   uint64_t m_nRecvSize;
};