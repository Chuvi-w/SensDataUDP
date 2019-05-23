#ifndef CIMUReceiver_h__
#define CIMUReceiver_h__
//#include "MathLib/MathLib3D.h"
#include <stdint.h>
#include <SFML/Network.hpp>
#include <wchar.h>
#include <thread>
#include <atomic>
#include <map>
//#include "CIMUSensors.h"
#include <fstream>
#include "EvCommon.h"

class CDataReceiver
{

 protected:
   virtual ~CDataReceiver();
   virtual void RecvThread() = 0;

   std::atomic_bool IsStopped() const;

   bool ProcessPacket(const std::string& RecvName, const void* pPacket, size_t nDataSize);
   bool ProcessPacket(CDataPacket &Packet);
 public:
   CDataReceiver();
   bool AddListener(std::shared_ptr<IEventReceiver> pListener);
   void StopThread();
   void StartThread();
   void SetLogging(bool bLogging = true);

 private:
   std::thread                                  m_pThread;
   std::atomic_bool                             m_bStopThread;
   bool                                         m_bLogging;
   std::vector<std::shared_ptr<IEventReceiver>> m_vListeners;
};

class CReceiverUDP : public CDataReceiver
{
 public:
   CReceiverUDP(uint16_t nPort);
   ~CReceiverUDP();

 protected:
   virtual void RecvThread() override;

 private:
   uint16_t m_nPort;

   sf::UdpSocket m_Socket;
};


class CReceiverFile: public CDataReceiver
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
   bool LoadFile(const std::string &sFileName);



private:

   
   bool AddPacket(CDataPacket &pPacket);
protected:
   virtual void RecvThread() override;

   std::vector<CDataPacket> m_vPackets;
};

#endif // CIMUReceiver_h__
