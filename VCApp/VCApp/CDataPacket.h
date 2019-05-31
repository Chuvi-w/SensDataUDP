#pragma once
#include "EvCommon.h"
#include "CTimeStamp.h"

class CDataPacket
{
 public:
   CDataPacket();
   CDataPacket(const CDataPacket& pOther);
   ~CDataPacket();

   int32_t                    LoadData(void* pData, size_t nDataSize, const std::string& sSourceName);
   template <typename T> bool GetData(T* Data, size_t nElementsCount)
   {
      if(!m_nData || m_nReadPos + (sizeof(T) * nElementsCount) > m_nData->size())
      {
         return false;
      }
      memcpy(Data, reinterpret_cast<void*>((size_t)m_nData->data() + m_nReadPos), sizeof(T) * nElementsCount);
      if(!m_bEndian)
      {
         for(size_t i = 0; i < nElementsCount; i++)
         {
            bswap(Data[i]);
         }
      }
      m_nReadPos += (sizeof(T) * nElementsCount);
      return true;
   }
   template <typename T> bool GetData(T& Data)
   {
      if(!m_nData || m_nReadPos + sizeof(T) > m_nData->size())
      {
         return false;
      }
      memcpy(&Data, reinterpret_cast<void*>((size_t)m_nData->data() + m_nReadPos), sizeof(T));

      if(!m_bEndian)
      {
         bswap(Data);
      }
      m_nReadPos += sizeof(T);
      return true;
   }

   template <> bool GetData(std::string& sString)
   {

      uint32_t nStringLen;
      if(!GetData(nStringLen))
      {
         return false;
      }
      sString.resize(nStringLen);
      if(!GetData(&sString[0], nStringLen))
      {
         return false;
      }
      return true;
   }

   size_t       GetRemainDataSize() const;
   size_t       GetDataSize() const;
   bool         IsValid() const;
   CTimeStampNS GetNanoTime() const;

   uint64_t GetPacketID() const;

   bool operator==(const CDataPacket& pOther) const;

 private:
   // void *m_nData;
   std::shared_ptr<std::vector<uint8_t>> m_nData;
   size_t                                m_nReadPos;
   bool                                  m_bEndian;
   CTimeStampNS                          m_nNanoTime;
   uint64_t                              m_nPacketID;
   // uint64_t m_nRecvSize;
};