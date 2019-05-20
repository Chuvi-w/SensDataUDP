#include "CDataReceiver.h"
#include <inttypes.h>
#include "CTouchEvent.h"
#include "CSensorEvent.h"


CReceiverUDP::CReceiverUDP(uint16_t nPort):m_nPort(nPort)
{
   StartThread();
}

CReceiverUDP::~CReceiverUDP()
{
   StopThread();
}

void CReceiverUDP::RecvThread()
{
   sf::Socket::Status stat;
   bool bBindOK = m_Socket.bind(m_nPort, sf::IpAddress(0, 0, 0, 0)) != sf::Socket::Done;

   sf::IpAddress RecvFrom;
   char RecvData[1024 * 20];
   size_t NumRecvBytes;
   std::string RecvName;
   unsigned short RecvPort;
   m_Socket.setBlocking(false);
   do
   {
      stat = m_Socket.receive(RecvData, sizeof(RecvData), NumRecvBytes, RecvFrom, RecvPort);
      if(stat == sf::Socket::Done&&NumRecvBytes > 0)
      {
         if(NumRecvBytes < sizeof(RecvData))
         {
            RecvData[NumRecvBytes] = 0;           
         }
         RecvName = std::string("IMU_UDP_") + RecvFrom.toString() + "_" + std::to_string(RecvPort);
         ProcessPacket(RecvName, RecvData,NumRecvBytes);
      }

   }
   while(!IsStopped());
}

CDataReceiver::~CDataReceiver()
{

}

std::atomic_bool CDataReceiver::IsStopped() const
{
   return m_bStopThread;
}

bool CDataReceiver::ProcessPacket(const std::string &RecvName, const void *pPacket, size_t nDataSize)
{
   static int nPack = 0;

   const CommPacket_t* pComm = reinterpret_cast<const CommPacket_t*>(pPacket);
   uint64_t nPacketID = pComm->PacketID;
   uint64_t nPacketLen = pComm->DataSize;
  // printf("Recv_%s_%i %"PRIX64" %"PRIX64"\n", RecvName.c_str(), nPack++,pComm->IsEndian,pComm->PacketID);
  

   if (pComm->IsEndian != 1)
   {
      bswap(nPacketID);
      bswap(nPacketLen);
   }

   if (nPacketLen + sizeof(CommPacket_t) < nDataSize)
   {
      return false;
   }

   for (auto &Listener : m_vListeners)
   {
      if (Listener->GetEventID() == nPacketID)
      {
         Listener->ParseEvent((const void*)((size_t)pPacket + sizeof(CommPacket_t)), nDataSize - sizeof(CommPacket_t), pComm->IsEndian == 1);
      }
   }
    return true;

}

CDataReceiver::CDataReceiver()
{
   m_bStopThread = true;
   m_bLogging = false;
}

bool CDataReceiver::AddListener(std::shared_ptr<IEventReceiver> pListener)
{
   bool bExist = false;
   for (auto &ExtListener : m_vListeners)
   {
      if (ExtListener == pListener)
      {
         bExist = true;
      }
   }
   if (!bExist)
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
      m_pThread = std::thread([=]
      {
         this->RecvThread();
      });
   }
}

void CDataReceiver::SetLogging(bool bLogging /*= true*/)
{
   m_bLogging = bLogging;
}


