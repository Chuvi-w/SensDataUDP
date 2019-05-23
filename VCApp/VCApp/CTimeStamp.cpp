#include "CTimeStamp.h"


CTimeStampNS::CTimeStampNS(uint64_t Ts): m_TS(Ts)
{

}

CTimeStampNS::CTimeStampNS(const CTimeStampNS &pOther) : m_TS(pOther.m_TS)
{

}

bool CTimeStampNS::operator<(const CTimeStampNS& pOther) const
{
   return m_TS < pOther.m_TS;
}

double CTimeStampNS::GetSeconds() const
{
   return (double)m_TS / 1000000000.0;
}

bool CTimeStampNS::operator==(const CTimeStampNS& pOther) const
{
   return m_TS == pOther.m_TS;
}
