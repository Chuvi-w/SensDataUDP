#include "CDataReceiver.h"
#include <inttypes.h>
#include <algorithm>
#include <sstream>
#include <iostream>
#include "CTouchEvent.h"
#include "CSensorEvent.h"
#include "CDataPacket.h"

CReceiverNetWork::CReceiverNetWork(uint16_t nUPDPort, uint16_t nTCPPort) : m_nUDPPort(nUPDPort), m_nTCPPort(nTCPPort), m_BindStatus(sf::Socket::Error), m_ListenStatus(sf::Socket::Error) {}

CReceiverNetWork::~CReceiverNetWork() { StopThread(); }

std::string CReceiverNetWork::GetStat() const { return ""; }

void CReceiverNetWork::RecvThread()
{

   std::string RecvName;

   m_SockUDP.setBlocking(false);
   std::vector<CDataPacket> vPack;
   CDataPacket              DataPack;
   size_t                   nPack = 0;
   size_t                   LoadOffset;
   int32_t                  LoadSize;
   do
   {
      if(1)
      {
         CheckUDPBroadcastRequests();
         CheckTCPEvents();
      }

   } while(!IsStopped());
}

void CReceiverNetWork::CheckTCPEvents()
{
   std::vector<uint8_t> vRecvData;
   size_t               LoadOffset;
   int32_t              LoadSize;
   CDataPacket          DataPack;
   if(m_ListenStatus != sf::Socket::Done)
   {
      m_ListenStatus = m_Listener.listen(m_nTCPPort);
      if(m_ListenStatus != sf::Socket::Done)
      {
         printf("Unable to listen on port %u\n", m_nTCPPort);
         m_nTCPPort++;
      }
      else
      {
         m_Selector.clear();
         m_Selector.add(m_Listener);
         printf("Start listening on port %u\n", m_nTCPPort);
      }
   }
   else
   {
      if(m_Selector.wait(sf::microseconds(10)))
      {
         //  std::cout << "Wait ok" << std::endl;
         if(m_Selector.isReady(m_Listener))
         {
            std::cout << "m_Listener ready" << std::endl;
            auto client = new CSocketClient;
            if(m_Listener.accept(client->GetSocket()) == sf::Socket::Done)
            {
               client->ResetConnectTime();
               m_clients.push_back(client);
               m_Selector.add(client->GetSocket());
            }
            else
            {
               // Error, we won't get a new connection, delete the socket
               delete client;
            }
         }
         else
         {
            for(auto& SockClient : m_clients)
            {
               if(m_Selector.isReady(SockClient->GetSocket()))
               {
                  vRecvData = SockClient->ReceivePacket();

                  if(!vRecvData.empty())
                  {
                     LoadOffset = 0;
                     LoadSize   = 0;
                     do
                     {
                        LoadSize = DataPack.LoadData(&vRecvData[LoadOffset], vRecvData.size() - LoadOffset, FindOrCreateRecvSrc(SockClient->GetSocket().getRemoteAddress(), SockClient->GetSocket().getRemotePort()));
                        if(LoadSize < 0)
                        {
                           break;
                        }
                        // printf("%u_%s %u\n", nPack++, RecvName.c_str(), NumRecvBytes);
                        ProcessPacket(DataPack);

                        LoadOffset += LoadSize;
                     } while(LoadOffset < vRecvData.size());
                  }
               }
            }
         }
      }

      m_clients.remove_if([this](CSocketClient* pClient) {
         if(pClient->IsTimeoutOrDisconnected())
         {
            m_Selector.remove(pClient->GetSocket());
            return true;
         }
         return false;
      });
   }
}

void CReceiverNetWork::CheckUDPBroadcastRequests()
{
   sf::IpAddress      RecvFrom;
   char               RecvData[1024 * 20];
   size_t             NumRecvBytes;
   sf::Socket::Status stat;
   unsigned short     RecvPort;
   if(m_BindStatus != sf::Socket::Done)
   {
      m_BindStatus = m_SockUDP.bind(m_nUDPPort, sf::IpAddress(0, 0, 0, 0));
      if(m_BindStatus != sf::Socket::Done)
      {
         printf("Unable to bind on port %u\n", m_nUDPPort);
         m_nUDPPort++;
      }
      else
      {
         printf("Start listening on UDP port %u\n", m_nUDPPort);
      }
   }
   else
   {
      stat = m_SockUDP.receive(RecvData, sizeof(RecvData), NumRecvBytes, RecvFrom, RecvPort);
      if(stat == sf::Socket::Done && NumRecvBytes > 0)
      {
         if(m_ListenStatus == sf::Socket::Done && NumRecvBytes == (sizeof(uint32_t) * 3) && ((uint32_t*)RecvData)[0] == 0xFF00EE11 && ((uint32_t*)RecvData)[1] == 0x22DD33CC && ((uint32_t*)RecvData)[2] == 0xBB44AA55)
         {
            NumRecvBytes             = (sizeof(uint32_t) * 4);
            ((uint32_t*)RecvData)[0] = 0x00FF11EE;
            ((uint32_t*)RecvData)[1] = 0xDD22CC33;
            ((uint32_t*)RecvData)[2] = 0x44BB55AA;
            ((uint32_t*)RecvData)[3] = m_nTCPPort;
            m_SockUDP.send(RecvData, NumRecvBytes, RecvFrom, RecvPort);
            printf("");
         }
      }
   }
}

IRecvSource::Ptr CReceiverNetWork::FindOrCreateRecvSrc(sf::IpAddress ip, uint16_t nPort)
{

   for(const auto& RecvSrc : m_vRecvSource)
   {

      auto* Ptr = dynamic_cast<CRecvSrcUDP*>(RecvSrc.get());
      if(Ptr)
      {
         if(ip == Ptr->GetIP() && nPort == Ptr->GetPort())
         {
            return RecvSrc;
         }
      }
   }

   m_vRecvSource.push_back(IRecvSource::Ptr(new CRecvSrcUDP(ip, nPort)));
   return m_vRecvSource.back();
}

CDataReceiver::~CDataReceiver() {}

std::atomic_bool CDataReceiver::IsStopped() const { return m_bStopThread; }

bool CDataReceiver::ProcessPacket(const CDataPacket& Packet)
{
   if(!Packet.IsValid())
   {
      return false;
   }
   auto SrcHash = Packet.GetRecvSrc()->GetHash();

   auto pProcessor = m_Receivers.find(SrcHash);
   if(pProcessor == m_Receivers.end())
   {
      if(m_fnMakeNewReceivers)
      {
         m_Receivers[SrcHash] = m_fnMakeNewReceivers();
      }
      else
      {
         m_Receivers[SrcHash] = std::vector<IEventReceiver::PTR>();
      }

      /* for(auto& ExtListener : m_vEvListenerCreator)
       {
          m_Receivers[SrcHash].push_back(ExtListener());
       }*/
      pProcessor = m_Receivers.find(SrcHash);
   }

   for(auto& Listener : pProcessor->second)
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

// bool CDataReceiver::AddEvListenerCreator(std::function<std::shared_ptr<IEventReceiver>()> fnCreate)
// {
//
//    size_t FuncTypeHash= fnCreate.target_type().hash_code();
//    size_t CmpType = 0;
//
//    for(auto& ExtListener : m_vEvListenerCreator)
//    {
//       CmpType = ExtListener.target_type().hash_code();
//       if(FuncTypeHash == CmpType)
//       {
//          return false;
//       }
//    }
//    m_vEvListenerCreator.push_back(fnCreate);
//    return true;
// }

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
   std::sort(m_vPackets.begin(), m_vPackets.end(), [](const CDataPacket& p1, const CDataPacket& p2) { return p1.GetNanoTime() < p2.GetNanoTime(); });
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
      CurRead = DataPack.LoadData(reinterpret_cast<void*>((size_t)pFileData + ReadOffset), FileSz - ReadOffset, IRecvSource::Ptr(new CRecvSrcFile()));
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
   std::stringstream           ss;
   ss << "Packets left=" << m_vPackets.size() - m_CurEvID << std::endl;
   return ss.str();
}

void CReceiverFile::GetTimeMinMaxSKO()
{
   std::sort(m_vPackets.begin(), m_vPackets.end(), [](const CDataPacket& p1, const CDataPacket& p2) { return p1.GetNanoTime() < p2.GetNanoTime(); });
   int64_t              DiffMin = INT64_MAX;
   int64_t              DiffMax = 0;
   CTimeStampNS         TimeDiff, nCurTS;
   auto                 nPrevTS = m_vPackets.front().GetNanoTime();
   std::vector<int64_t> vTimeDiff;

   double dTimeDiffSum = 0, dTimeDiffAvg;

   double dSKOSum = 0, dSKO;
   for(size_t i = 1; i < m_vPackets.size(); i++)
   {
      nCurTS   = m_vPackets[i].GetNanoTime();
      TimeDiff = nCurTS - nPrevTS;
      nPrevTS  = nCurTS;
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
   for(const auto& D : vTimeDiff)
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
   m_CurEvID               = 0;
   bool bNoMoreDataPrinted = false;
   do
   {
      if(m_RdLock.try_lock())
      {
         if(m_CurEvID < m_vPackets.size())
         {
            ProcessPacket(m_vPackets[m_CurEvID]);
            m_CurEvID++;
            if(bNoMoreDataPrinted)
            {
               bNoMoreDataPrinted = false;
            }
         }
         else
         {
            if(!bNoMoreDataPrinted)
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
