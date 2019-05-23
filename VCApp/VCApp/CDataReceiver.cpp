#include "CDataReceiver.h"
#include <inttypes.h>
#include <algorithm>
#include "CTouchEvent.h"
#include "CSensorEvent.h"
#include "CDataPacket.h"

CReceiverUDP::CReceiverUDP(uint16_t nPort) : m_nPort(nPort) { StartThread(); }

CReceiverUDP::~CReceiverUDP() { StopThread(); }



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
   CDataPacket DataPack;

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

         DataPack.LoadData(RecvData, NumRecvBytes, RecvName);
         ProcessPacket(DataPack);
      }

   } while(!IsStopped());
}

CDataReceiver::~CDataReceiver() {}

std::atomic_bool CDataReceiver::IsStopped() const { return m_bStopThread; }


bool CDataReceiver::ProcessPacket(CDataPacket &Packet)
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

CReceiverFile::CReceiverFile()
{

}

CReceiverFile::~CReceiverFile()
{

}

void CReceiverFile::ResetPackets()
{
   m_vPackets.clear();
}

void CReceiverFile::SortPackets()
{
   std::sort(m_vPackets.begin(), m_vPackets.end(), [](const std::vector<uint8_t > &p1, const std::vector<uint8_t > &p2)
   {
      const CommPacket_t *HDR1, *HDR2;

      jlong Time1, Time2;
      HDR1 = reinterpret_cast<const CommPacket_t*>(p1.data());
      HDR2= reinterpret_cast<const CommPacket_t*>(p2.data());
      Time1 = HDR1->NanoTime;
      Time2 = HDR2->NanoTime;

      if(HDR1->IsEndian != 1)
      {
         bswap(Time1);
      }

      if(HDR2->IsEndian != 1)
      {
         bswap(Time2);
      }

      return Time1 < Time2;
   });
}
#if 0
bool CReceiverFile::LoadFile(const std::string &sFileName)
{
   FILE *fIN = fopen(sFileName.c_str(), "rb");
 
   std::vector<uint8_t> PackData;
   const CommPacket_t *CommHdr=nullptr;
   size_t FileSz = 0;

   if(!fIN)
   {
      return false;
   }

   fseek(fIN, 0, SEEK_END);
   FileSz = ftell(fIN);
   fseek(fIN, 0, SEEK_SET);
   size_t ReadSz;
   size_t DataSz = 0;

   do 
   {
      PackData.clear();
      PackData.resize(sizeof(CommHdr));
      CommHdr = reinterpret_cast<const CommPacket_t*>(PackData.data());
      ReadSz = fread(&CommHdr, 1, sizeof(CommHdr), fIN);
      if(ReadSz != sizeof(CommHdr))
      {
         break;
      }
      else
      {
         DataSz = CommHdr->DataSize;
         if(CommHdr->IsEndian!=1)
         {
            bswap(DataSz);
         }
         if(FileSz - ftell(fIN) < DataSz)
         {
            break;
         }
         else
         {
            PackData.resize(sizeof(CommHdr)+DataSz);

            ReadSz= fread(reinterpret_cast<void*>((size_t)PackData.data()+sizeof(CommHdr)), 1, DataSz, fIN);
           
            AddPacket(PackData);
         }
      }


   } while (ftell(fIN)<FileSz);
   fclose(fIN);

   SortPackets();
   return true;

}

#endif

bool CReceiverFile::LoadFile(const std::string &sFileName)
{
   FILE *fIN = fopen(sFileName.c_str(), "rb");
   size_t FileSz = 0;
   size_t ReadOffset = 0;
   int32_t CurRead = 0;

   CDataPacket DataPack;
   void *pFileData = nullptr;

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
         ProcessPacket(DataPack);
         ReadOffset += CurRead;
      }
   } while (CurRead>0&&ReadOffset<FileSz);
   size_t ReadSz;
   size_t DataSz = 0;
}

bool CReceiverFile::AddPacket(const vPacket &pPacket)
{
   bool bExist = false;

#if 0
   for(auto &Pack : m_vPackets)
   {
      if(Pack.nTimeStamp == pPacket.nTimeStamp)
      {
         if(Pack.nPacketID == pPacket.nPacketID&&Pack.vData.size() == pPacket.vData.size() && pPacket.bIsEndian == Pack.bIsEndian)
         {
            if(!memcmp(Pack.vData.data(), pPacket.vData.data(), pPacket.vData.size()))
            {
               bExist = true;
               break;
            }
         }

      }
   }
#endif
   if(!bExist)
   {
      m_vPackets.push_back(pPacket);
      return true;
   }
   return false;

}

void CReceiverFile::RecvThread()
{
  
}



bool vPacketIsEndian(const vPacket &p)
{
   if(p.size() < sizeof(CommPacket_t))
   {
      return 0;
   }
   const CommPacket_t *pComm = reinterpret_cast<const CommPacket_t*>(p.data());
   uint64_t nData = pComm->NanoTime;
}



uint64_t GetTimeFixed(const vPacket &p)
{
   if(p.size() < sizeof(CommPacket_t))
   {
      return 0;
   }
   const CommPacket_t *pComm = reinterpret_cast<const CommPacket_t*>(p.data());
   uint64_t nData = pComm->NanoTime;
   

}