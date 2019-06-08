#include "CReceiverFile.h"
#include <algorithm>

#include <sstream>


CReceiverFile::CReceiverFile() {}

CReceiverFile::~CReceiverFile() { m_vPackets.clear(); }

void CReceiverFile::SortPackets()
{
   m_vPackets.sort([](const CDataPacket& p1, const CDataPacket& p2) { return p1.GetNanoTime() < p2.GetNanoTime(); });

//   m_vPackets.sort();
 //  std::sort(m_vPackets.begin(), m_vPackets.end(), [](const CDataPacket& p1, const CDataPacket& p2) { return p1.GetNanoTime() < p2.GetNanoTime(); });
}

bool CReceiverFile::LoadFile(const std::string& sFileName)
{
   FILE*   fIN = fopen(sFileName.c_str(), "rb");
   size_t  FileSz = 0;
   size_t  ReadOffset = 0;
   int32_t CurRead = 0;

   CDataPacket DataPack;
   void*       pFileData = nullptr;

   if (!fIN)
   {
      return false;
   }

   fseek(fIN, 0, SEEK_END);
   FileSz = ftell(fIN);
   fseek(fIN, 0, SEEK_SET);
   pFileData = new uint8_t[FileSz];
   fread(pFileData, FileSz, 1, fIN);
   fclose(fIN);

   do
   {
      CurRead = DataPack.LoadData(reinterpret_cast<void*>((size_t)pFileData + ReadOffset), FileSz - ReadOffset, IRecvSource::Ptr(new CRecvSrcFile()));
      if (CurRead > 0)
      {

         AddPacket(DataPack);
         ReadOffset += CurRead;
      }
   } while (CurRead > 0 && ReadOffset < FileSz);

  
   SortPackets();

   printf("sz=%i  %08.7f\n", m_vPackets.size(),GetEndTime().GetSeconds()-GetStartTime().GetSeconds());

   return true;
}

std::string CReceiverFile::GetStat() const
{
   std::lock_guard<std::mutex> AutoLock(m_RdLock);
   std::stringstream           ss;
   ss << "Packets left=" << m_vPackets.size() - m_CurEvID << std::endl;
   return ss.str();
}

void CReceiverFile::GetTimeMinMaxSKO()
{

   SortPackets();
  // m_vPackets.sort([](const CDataPacket& p1, const CDataPacket& p2) { return p1.GetNanoTime() < p2.GetNanoTime(); });
//   std::sort(m_vPackets.begin(), m_vPackets.end(), [](const CDataPacket& p1, const CDataPacket& p2) { return p1.GetNanoTime() < p2.GetNanoTime(); });
   int64_t              DiffMin = INT64_MAX;
   int64_t              DiffMax = 0;
   CTimeStampNS         TimeDiff, nCurTS;
   auto                 nPrevTS = m_vPackets.front().GetNanoTime();
   std::vector<int64_t> vTimeDiff;

   auto pCur = m_vPackets.begin();
   pCur++;
   double dTimeDiffSum = 0, dTimeDiffAvg;

   double dSKOSum = 0, dSKO;
   while (pCur!=m_vPackets.end())
   {
      nCurTS = (*pCur).GetNanoTime();
      TimeDiff = nCurTS - nPrevTS;
      nPrevTS = nCurTS;
      dTimeDiffSum += TimeDiff.m_TS;
      vTimeDiff.push_back(TimeDiff.m_TS);
      if (TimeDiff.m_TS < DiffMin)
      {
         DiffMin = TimeDiff.m_TS;
      }

      if (TimeDiff.m_TS > DiffMax)
      {
         DiffMax = TimeDiff.m_TS;
      }
   }
   dTimeDiffAvg = dTimeDiffSum / double(vTimeDiff.size());
   for (const auto& D : vTimeDiff)
   {
      dSKOSum = abs((double)D - dTimeDiffAvg) * abs((double)D - dTimeDiffAvg);
   }

   dSKO = sqrt(dSKOSum / double(vTimeDiff.size()));

   printf("Time Min=%llu. Max=%llu\n", DiffMin, DiffMax);

   printf("");
}

bool CReceiverFile::AddPacket(CDataPacket& pPacket)
{

   /*for(auto &Pack : m_vPackets)
   {
   if(Pack == pPacket)
   {
   return false;
   }
   }*/

   m_vPackets.push_back(pPacket);
   return true;
}

void CReceiverFile::RecvThread()
{
   m_CurEvID = 0;
   bool bNoMoreDataPrinted = false;
   auto pCur = m_vPackets.begin();
   
   do
   {
      if (m_RdLock.try_lock())
      {
         if (pCur!=m_vPackets.end())
         {
            ProcessPacket(*pCur);
            m_CurEvID++;
            pCur++;
            if (bNoMoreDataPrinted)
            {
               bNoMoreDataPrinted = false;
            }
         }
         else
         {
            if (!bNoMoreDataPrinted)
            {
               bNoMoreDataPrinted = true;

               printf("No more packets\n");
            }
         }
         m_RdLock.unlock();
      }

   } while (!IsStopped());

   printf("");
}