#include "CSocketClient.h"
#include <iostream>
#include <stdint.h>

CSocketClient::CSocketClient() : m_bDisconnected(false) {}

CSocketClient::~CSocketClient() { m_Socket.disconnect(); }

sf::TcpSocket& CSocketClient::GetSocket() { return m_Socket; }

void CSocketClient::ResetConnectTime()
{
   std::cout << "Client " << m_Socket.getRemoteAddress().toString().c_str() << ":" << m_Socket.getRemotePort() << " accepted" << std::endl;
   m_TimeConnect.restart();
}

void CSocketClient::ResetLastRecvTime() { m_TimeLastRecv.restart(); }

std::vector<uint8_t> CSocketClient::ReceivePacket()
{
   std::vector<uint8_t> vData(4096);
   size_t               nRecvd     = 0;
   auto                 RecvStatus = m_Socket.receive(vData.data(), vData.size(), nRecvd);
   if(RecvStatus == sf::Socket::Done)
   {
      vData.resize(nRecvd);
      // std::cout << "Recv " << nRecvd << " bytes from " << m_Socket.getRemoteAddress().toString().c_str() << ":" << m_Socket.getRemotePort() << std::endl;
      m_TimeLastRecv.restart();
      // m_Socket.send(Data, nRecvd);
   }
   else
   {
      vData.clear();
      if(RecvStatus == sf::Socket::Disconnected)
      {
         std::cout << "Client" << m_Socket.getRemoteAddress().toString().c_str() << ":" << m_Socket.getRemotePort() << " disconnected." << std::endl;
         m_bDisconnected = true;
      }
      else
      {
         std::cout << "Recv " << nRecvd << " bytes from " << m_Socket.getRemoteAddress().toString().c_str() << ":" << m_Socket.getRemotePort() << ". Status=" << RecvStatus << std::endl;
      }
   }

   return vData;
}

sf::Time CSocketClient::GetConnectTime() { return m_TimeConnect.getElapsedTime(); }

bool CSocketClient::IsTimeoutOrDisconnected() const
{
   if(m_bDisconnected || m_TimeLastRecv.getElapsedTime().asSeconds() > 200000.0)
   {
      std::cout << " Client " << m_Socket.getRemoteAddress().toString().c_str() << ":" << m_Socket.getRemotePort() << " timeout." << std::endl;

      return true;
   }
   return false;
}