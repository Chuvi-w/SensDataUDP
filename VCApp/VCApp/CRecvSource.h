#pragma once
#include <SFML/Network.hpp>
#include <stdint.h>
#include <memory>

typedef enum RecvSourceType_e
{
   RecvSrcUDP,
   RecvSrcFile,
   RecvSrcMax
} RecvSourceType_t;

class IRecvSource
{
 public:
   using Ptr = std::shared_ptr<IRecvSource>;

 protected:
   IRecvSource() {}

 public:
   virtual RecvSourceType_e GetSourceType() const                       = 0;
   virtual bool             operator==(const IRecvSource& pOther) const = 0;
   virtual size_t           GetHash() const                             = 0;
};

class CRecvSrcUDP : public IRecvSource
{

 public:
   CRecvSrcUDP(sf::IpAddress IP = sf::IpAddress(), uint16_t RecvPort = 0);
   CRecvSrcUDP(const CRecvSrcUDP& pOther);
   CRecvSrcUDP& operator=(CRecvSrcUDP& pOther);

   virtual RecvSourceType_e GetSourceType() const override;
   sf::IpAddress            GetIP() const;
   uint16_t                 GetPort() const;

   virtual bool operator==(const IRecvSource& pOther) const override
   {
      if(pOther.GetSourceType() != GetSourceType())
      {
         return false;
      }

      auto& ptr = dynamic_cast<const CRecvSrcUDP&>(pOther);
      if(ptr.GetIP() == GetIP() && ptr.GetPort() == GetPort())
      {
         return true;
      }
      return false;
   }

   virtual size_t GetHash() const override;

 private:
   void          SetStrVal();
   sf::IpAddress m_IP;
   uint32_t      m_Port;
   std::string   m_StrVal;
};

class CRecvSrcFile : public IRecvSource
{

 public:
   virtual RecvSourceType_e GetSourceType() const override;

   virtual bool operator==(const IRecvSource& pOther) const override { throw std::logic_error("The method or operation is not implemented."); }

   virtual size_t GetHash() const override;
};