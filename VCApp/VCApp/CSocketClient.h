#pragma once
#include <SFML/Network.hpp>
#include <stdint.h>
#include <vector>

class CSocketClient
{
 public:
   CSocketClient();
   ~CSocketClient();

   sf::TcpSocket& GetSocket();

   void                 ResetConnectTime();
   void                 ResetLastRecvTime();
   std::vector<uint8_t> ReceivePacket();
   sf::Time             GetConnectTime();
   bool                 IsTimeoutOrDisconnected() const;

 private:
   sf::TcpSocket m_Socket;
   sf::Clock     m_TimeConnect, m_TimeLastRecv;
   bool          m_bDisconnected;
};
