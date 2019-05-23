#pragma once

#include <stdint.h>

class CTimeStampNS
{
public:
   CTimeStampNS(uint64_t Ts);
   CTimeStampNS(const CTimeStampNS &pOther);
   bool operator<(const CTimeStampNS& pOther) const;
   double GetSeconds() const;
   uint64_t m_TS;
};
