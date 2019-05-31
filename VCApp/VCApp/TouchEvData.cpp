#include "TouchEvData.h"

CTouchPointer::CTouchPointer(const PointerData_t& pData) : m_Ptr(pData) {}

CTouchPointer::CTouchPointer(const CTouchPointer& pOther) : m_Ptr(pOther.m_Ptr) {}

CTouchPointer::~CTouchPointer() {}

CMotionEvent::CMotionEvent(const MotionEvent_t& MEv) : m_Ev(MEv) {}

CMotionEvent::CMotionEvent(const CMotionEvent& pOther) : m_Ev(pOther.m_Ev) {}

bool CMotionEvent::AddPointer(const std::shared_ptr<CTouchPointer> TouchPtr)
{

   if(TouchPtr && !FindPtrID(TouchPtr->GetPointerId()))
   {
      m_Ptr.push_back(TouchPtr);
      return true;
   }
   return false;
}

std::shared_ptr<CTouchPointer> CMotionEvent::FindPtrID(int nID) const
{

   for(const auto& p : m_Ptr)
   {
      if(p->GetID() == nID)
      {
         return p;
      }
   }
   return nullptr;
}

std::shared_ptr<CTouchPointer> CMotionEvent::GetPtr(int nID) const
{
   if(nID >= m_Ptr.size())
   {
      return nullptr;
   }
   return m_Ptr[nID];
}
