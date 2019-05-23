#include "CDataPacket.h"



CDataPacket::CDataPacket():
m_nData(nullptr),
m_nReadPos(0),
m_bEndian(false),
m_nTimeStampNS(0),
m_nPacketID(0),
m_nRecvSize(0)
{

}

CDataPacket::CDataPacket(const CDataPacket &pOther):
m_nData(nullptr),
m_nReadPos(pOther.m_nReadPos),
m_bEndian(pOther.m_bEndian),
m_nTimeStampNS(pOther.m_nTimeStampNS),
m_nPacketID(pOther.m_nPacketID),
m_nRecvSize(pOther.m_nRecvSize)
{
   if(pOther.m_nRecvSize)
   {
      m_nData = new uint8_t[pOther.m_nRecvSize];
      if(m_nData)
      {
         memcpy(m_nData, pOther.m_nData, pOther.m_nRecvSize);
      }
   }

   
   
}

CDataPacket::~CDataPacket()
{
   if(m_nData)
   {
      delete [] m_nData;
      m_nData = nullptr;
   }
}

int32_t CDataPacket::LoadData(void *pData, size_t nDataSize, const std::string &sSourceName)
{
   if(nDataSize < sizeof(CommPacket_t))
   {
      return -1;
   }
   uint64_t IndSwap = 1;
   bswap(IndSwap);
   m_nRecvSize = 0;
   if(m_nData)
   {
      delete[] m_nData;
      m_nData = nullptr;
   }
   CommPacket_t CommHdr;
   uint64_t nTimeStamp;
   memcpy(&CommHdr, pData, sizeof(CommHdr));
   if(CommHdr.IsEndian == 1)
   {
      m_bEndian = true;
   }
   else if(CommHdr.IsEndian == IndSwap)
   {
      m_bEndian = false;
   }
   else
   {
      return -1;
   }

   nTimeStamp = CommHdr.NanoTime;
   m_nPacketID = CommHdr.PacketID;
   m_nRecvSize = CommHdr.DataSize;

   if(!m_bEndian)
   {
      bswap(nTimeStamp);
      bswap(m_nPacketID);
      bswap(m_nRecvSize);
   }

   m_nTimeStampNS = CTimeStampNS(nTimeStamp);
   if(nDataSize < sizeof(CommPacket_t) + m_nRecvSize)
   {
      return -1;
   }
   m_nData = new uint8_t[m_nRecvSize];
   memcpy(m_nData, reinterpret_cast<void*>((size_t)pData + sizeof(CommPacket_t)), m_nRecvSize);
   m_nReadPos = 0;
   return sizeof(CommPacket_t) + m_nRecvSize;

}

size_t CDataPacket::GetRemainDataSize() const
{
   if(m_nReadPos > m_nRecvSize)
   {
      return 0;
   }
   return m_nRecvSize - m_nReadPos;
}

size_t CDataPacket::GetDataSize() const
{
   return m_nRecvSize;
}

bool CDataPacket::IsValid() const
{
   return m_nData != 0 && m_nRecvSize > 0;
}

CTimeStampNS CDataPacket::GetTimeStamp() const
{
   return m_nTimeStampNS;
}

uint64_t CDataPacket::GetPacketID() const
{
   return m_nPacketID;
}
