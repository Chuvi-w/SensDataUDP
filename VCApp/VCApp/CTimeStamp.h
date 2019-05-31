#pragma once

#include <stdint.h>

class CTimeStampNS
{
 public:
   CTimeStampNS();
   CTimeStampNS(int64_t Ts);
   CTimeStampNS(const CTimeStampNS& pOther);
   bool         operator<(const CTimeStampNS& pOther) const;
   double       GetSeconds() const;
   bool         operator==(const CTimeStampNS& pOther) const;
   CTimeStampNS operator-(const CTimeStampNS& pOther) const { return CTimeStampNS(m_TS - pOther.m_TS); }

   bool operator<=(int32_t Time) { return m_TS <= Time; }

   int64_t m_TS;
};
