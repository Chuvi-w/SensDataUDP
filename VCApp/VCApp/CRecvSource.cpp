#include "CRecvSource.h"

CRecvSrcUDP::CRecvSrcUDP(sf::IpAddress IP /*= sf::IpAddress()*/, uint16_t RecvPort /*=0*/) : m_IP(IP), m_Port(RecvPort) {}

CRecvSrcUDP::CRecvSrcUDP(const CRecvSrcUDP& pOther) : m_IP(pOther.m_IP), m_Port(pOther.m_Port) {}

CRecvSrcUDP& CRecvSrcUDP::operator=(CRecvSrcUDP& pOther)
{
   if(this != &pOther)
   {
      this->m_IP   = pOther.m_IP;
      this->m_Port = pOther.m_Port;
   }
   return *this;
}

RecvSourceType_e CRecvSrcUDP::GetSourceType() const { return RecvSrcUDP; }

sf::IpAddress CRecvSrcUDP::GetIP() const { return m_IP; }

uint16_t CRecvSrcUDP::GetPort() const { return m_Port; }

size_t CRecvSrcUDP::GetHash() const
{
   uint64_t AddrVal = (((int64_t)m_IP.toInteger()) << 16) + m_Port;
   return std::hash<uint64_t>()(AddrVal);
}

RecvSourceType_e CRecvSrcFile::GetSourceType() const { return RecvSrcFile; }

size_t CRecvSrcFile::GetHash() const
{
   return 1000;
}
