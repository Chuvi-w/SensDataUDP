#include "CDataReceiver.h"
#include <inttypes.h>
#include <algorithm>
#include <sstream>
#include "CTouchEvent.h"
#include "CSensorEvent.h"
#include "CDataPacket.h"

CReceiverUDP::CReceiverUDP(uint16_t nPort) : m_nPort(nPort) {}

CReceiverUDP::~CReceiverUDP() { StopThread(); }

std::string CReceiverUDP::GetStat() const
{
   return "";
}

void CReceiverUDP::RecvThread()
{
   sf::Socket::Status stat;
   bool               bBindOK = m_Socket.bind(m_nPort, sf::IpAddress(0, 0, 0, 0)) != sf::Socket::Done;

   sf::IpAddress  RecvFrom;
   char           RecvData[1024 * 20];
   size_t         NumRecvBytes;
   std::string    RecvName;
   unsigned short RecvPort;
   m_Socket.setBlocking(false);
   std::vector<CDataPacket> vPack;
   CDataPacket DataPack;
   size_t nPack = 0;
   size_t LoadOffset;
   int32_t LoadSize;
   do
   {
      stat = m_Socket.receive(RecvData, sizeof(RecvData), NumRecvBytes, RecvFrom, RecvPort);
      if(stat == sf::Socket::Done && NumRecvBytes > 0)
      {
         if(NumRecvBytes < sizeof(RecvData))
         {
            RecvData[NumRecvBytes] = 0;
         }
         RecvName = std::string("IMU_UDP_") + RecvFrom.toString() + "_" + std::to_string(RecvPort);
        

         LoadOffset = 0;
         vPack.clear();
         do 
         {
            LoadSize = DataPack.LoadData(&RecvData[LoadOffset], NumRecvBytes - LoadOffset, RecvName);
            if(LoadSize < 0)
            {
               break;
            }
            vPack.push_back(DataPack);
            LoadOffset += LoadSize;
         } while (true);
        
         for(const auto &p : vPack)
         {
            printf("%u_%s %u\n", nPack++, RecvName.c_str(), NumRecvBytes);
            ProcessPacket(p);
         }
       
      }

   } while(!IsStopped());
}

CDataReceiver::~CDataReceiver() {}

std::atomic_bool CDataReceiver::IsStopped() const { return m_bStopThread; }

bool CDataReceiver::ProcessPacket(const CDataPacket& Packet)
{
   if(!Packet.IsValid())
   {
      return false;
   }

   for(auto& Listener : m_vListeners)
   {
      if(Listener->GetEventID() == Packet.GetPacketID())
      {
         Listener->ParseEvent(Packet);
      }
   }
   return true;
}

CDataReceiver::CDataReceiver()
{
   m_bStopThread = true;
   m_bLogging    = false;
}

bool CDataReceiver::AddListener(std::shared_ptr<IEventReceiver> pListener)
{
   bool bExist = false;
   for(auto& ExtListener : m_vListeners)
   {
      if(ExtListener == pListener)
      {
         bExist = true;
      }
   }
   if(!bExist)
   {
      m_vListeners.push_back(pListener);
   }
   return !bExist;
}

void CDataReceiver::StopThread()
{
   if(m_bStopThread == false && m_pThread.joinable())
   {
      m_bStopThread = true;
      m_pThread.join();
   }
}

void CDataReceiver::StartThread()
{
   if(!m_pThread.joinable())
   {
      m_bStopThread = false;
      m_pThread     = std::thread([=] { this->RecvThread(); });
   }
}

void CDataReceiver::SetLogging(bool bLogging /*= true*/) { m_bLogging = bLogging; }

CReceiverFile::CReceiverFile() {}

CReceiverFile::~CReceiverFile() { m_vPackets.clear(); }

void CReceiverFile::ResetPackets() { m_vPackets.clear(); }

void CReceiverFile::SortPackets()
{
   std::sort(m_vPackets.begin(), m_vPackets.end(), [](const CDataPacket& p1, const CDataPacket& p2)
   {
      return p1.GetNanoTime() < p2.GetNanoTime();
   });
}

bool CReceiverFile::LoadFile(const std::string& sFileName)
{
   FILE*   fIN        = fopen(sFileName.c_str(), "rb");
   size_t  FileSz     = 0;
   size_t  ReadOffset = 0;
   int32_t CurRead    = 0;

   CDataPacket DataPack;
   void*       pFileData = nullptr;

   if(!fIN)
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
      CurRead = DataPack.LoadData(reinterpret_cast<void*>((size_t)pFileData + ReadOffset), FileSz - ReadOffset, "Data");
      if(CurRead > 0)
      {

         AddPacket(DataPack);
         ReadOffset += CurRead;
      }
   } while(CurRead > 0 && ReadOffset < FileSz);

   printf("sz=%i\n", m_vPackets.size());
   SortPackets();
   return true;
}

std::string CReceiverFile::GetStat() const
{
   std::lock_guard<std::mutex> AutoLock(m_RdLock);
   std::stringstream ss;
   ss << "Packets left=" << m_vPackets.size() - m_CurEvID << std::endl;
   return ss.str();
}

void CReceiverFile::GetTimeMinMaxSKO()
{
   std::sort(m_vPackets.begin(), m_vPackets.end(), [](const CDataPacket& p1, const CDataPacket& p2)
   {
      return p1.GetNanoTime() < p2.GetNanoTime();
   });
   int64_t DiffMin = INT64_MAX;
   int64_t DiffMax = 0;
   CTimeStampNS TimeDiff, nCurTS;
   auto nPrevTS = m_vPackets.front().GetNanoTime();
   std::vector<int64_t> vTimeDiff;

   double dTimeDiffSum = 0, dTimeDiffAvg;

   double dSKOSum = 0, dSKO;
   for(size_t i = 1; i < m_vPackets.size(); i++)
   {
      nCurTS = m_vPackets[i].GetNanoTime();
      TimeDiff = nCurTS - nPrevTS;
      nPrevTS = nCurTS;
      dTimeDiffSum += TimeDiff.m_TS;
      vTimeDiff.push_back(TimeDiff.m_TS);
      if(TimeDiff.m_TS < DiffMin)
      {
         DiffMin = TimeDiff.m_TS;
      }

      if(TimeDiff.m_TS > DiffMax)
      {
         DiffMax = TimeDiff.m_TS;
      }

   }
   dTimeDiffAvg = dTimeDiffSum / double(vTimeDiff.size());
   for(const auto &D : vTimeDiff)
   {
      dSKOSum = abs((double)D - dTimeDiffAvg)*abs((double)D - dTimeDiffAvg);
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
   do
   {
      if (m_RdLock.try_lock())
      {
         if (m_CurEvID < m_vPackets.size())
         {
            ProcessPacket(m_vPackets[m_CurEvID]);
            m_CurEvID++;
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

   } while(!IsStopped());

   printf("");
}
