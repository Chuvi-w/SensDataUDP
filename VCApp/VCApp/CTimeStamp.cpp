#include "CTimeStamp.h"

CTimeStampNS::CTimeStampNS(int64_t Ts) : m_TS(Ts) {}

CTimeStampNS::CTimeStampNS(const CTimeStampNS& pOther) : m_TS(pOther.m_TS) {}

CTimeStampNS::CTimeStampNS() : m_TS(0) {}

bool CTimeStampNS::operator<(const CTimeStampNS& pOther) const { return m_TS < pOther.m_TS; }

double CTimeStampNS::GetSeconds() const { return (double)m_TS / 1000000000.0; }

bool CTimeStampNS::operator==(const CTimeStampNS& pOther) const { return m_TS == pOther.m_TS; }
