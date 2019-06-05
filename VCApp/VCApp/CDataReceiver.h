#ifndef CIMUReceiver_h__
#define CIMUReceiver_h__
//#include "MathLib/MathLib3D.h"
#include <stdint.h>
#include <SFML/Network.hpp>
#include <wchar.h>
#include <thread>
#include <atomic>
#include <map>
#include <fstream>
#include "EvCommon.h"
#include <mutex>
#include "CRecvSource.h"


class CDataReceiver
{

 protected:
   virtual ~CDataReceiver();
   virtual void RecvThread() = 0;

   std::atomic_bool IsStopped() const;

   bool ProcessPacket(const CDataPacket& Packet);

 public:

   CDataReceiver();
  
   //bool AddEvListenerCreator(std::function<std::shared_ptr<IEventReceiver>()> fnCreate);
   void StopThread();
   void StartThread();
   void SetLogging(bool bLogging = true);
   virtual std::string GetStat() const = 0;

   void SetNewReceiversFunc(std::function<std::vector<IEventReceiver::PTR>()> pFn)
   {
      m_fnMakeNewReceivers = pFn;
   }
 private:
   std::thread                                  m_pThread;
   std::atomic_bool                             m_bStopThread;
   bool                                         m_bLogging;
  // std::vector<std::function<std::shared_ptr<IEventReceiver>()>>   m_vEvListenerCreator;
   std::map<size_t, std::vector<IEventReceiver::PTR>> m_Receivers;
   std::function<std::vector<IEventReceiver::PTR>()> m_fnMakeNewReceivers;

   
};

class CReceiverUDP : public CDataReceiver
{
 public:
   CReceiverUDP(uint16_t nPort);
   ~CReceiverUDP();


   virtual std::string GetStat() const override;

protected:
   virtual void RecvThread() override;

 private:
    IRecvSource::Ptr FindOrCreateRecvSrc(sf::IpAddress ip, uint16_t nPort);

   uint16_t m_nPort;

   sf::UdpSocket m_Socket;


   std::vector<IRecvSource::Ptr> m_vRecvSource;
};

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

   void ResetPackets();
   void SortPackets();
   bool LoadFile(const std::string& sFileName);


   virtual std::string GetStat() const override;

private:

   void GetTimeMinMaxSKO();
   bool AddPacket(CDataPacket& pPacket);

 protected:
   virtual void RecvThread() override;

   std::vector<CDataPacket> m_vPackets;
   size_t m_CurEvID;
   mutable std::mutex m_RdLock;
  
};

#endif // CIMUReceiver_h__
