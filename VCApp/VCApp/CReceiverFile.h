#pragma once
#include "CDataReceiver.h"
#include "CDataPacket.h"
class CReceiverFile : public CDataReceiver
{

   /* typedef struct FilePacket_s
   {
   bool bIsEndian;
   uint64_t nTimeStamp;
   uint64_t nPacketID;
   std::vector<uint8_t> vData;

   }FilePacket_t;*/
public:
   CReceiverFile();
   ~CReceiverFile();

   void ResetPackets() { m_vPackets.clear(); }
   void SortPackets();
   bool LoadFile(const std::string& sFileName);

   virtual std::string GetStat() const override;

   CTimeStampNS GetStartTime() const
   {
      return m_vPackets.front().GetNanoTime();
   }

   CTimeStampNS GetEndTime() const
   {
      return m_vPackets.back().GetNanoTime();
   }
private:
   void GetTimeMinMaxSKO();
   bool AddPacket(CDataPacket& pPacket);

protected:
   virtual void RecvThread() override;

   std::list<CDataPacket> m_vPackets;
   size_t                   m_CurEvID;
   mutable std::mutex       m_RdLock;
};

